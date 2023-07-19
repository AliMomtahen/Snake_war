#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include<time.h>

#pragma warning(disable:4996)

//colors
#define RED 12
#define BLUE 3
#define GREEN 10
#define YELLOW 14
#define GRAY 8
#define PINK 13
#define WHITE 15
#define WAIT_TIME_MILI_SEC 100
//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
// general
#define BOARD_SIZE 40
#define INITIAL_SNAKE_LENGTH 3
#define MINIMUM_SNAKE_LENGTH 2
#define MAX_LEN_SNAKES 30
#define NUMBER_OF_MOUSES 20
//board_characters
#define EMPTY '0'
#define MOUSE 'm'
#define PLAYER1_SNAKE_HEAD '1'
#define PLAYER2_SNAKE_HEAD '2'
#define PLAYER1_SNAKE_BODY 'a'
#define PLAYER2_SNAKE_BODY 'b'
//Bernard, Poison and golden star
#define BERNARD_CLOCK 'c' //on the board character
#define GOLDEN_STAR '*' //on the board character
#define POISON 'x' //on the board character
#define NUMBER_OF_POISONS 5
#define NUMBER_OF_GOLDEN_STARS 3
#define BERNARD_CLOCK_APPEARANCE_CHANCE_PERCENT 20
#define BERNARD_CLOCK_APPEARANCE_CHECK_PERIOD_MILI_SEC 2000
#define BERNARD_CLOCK_FROZEN_TIME_MILI_SEC 4000
//the moods(ok or no))
#define ok 1
#define no 0
//the forms of resolt
#define player1_win 1
#define player2_win 2
#define equal       3

CONSOLE_FONT_INFOEX former_cfi;
CONSOLE_CURSOR_INFO former_info;
COORD former_screen_size;

void reset_console() {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleDisplayMode(consoleHandle, CONSOLE_WINDOWED_MODE, &former_screen_size);
	SetCurrentConsoleFontEx(consoleHandle, FALSE, &former_cfi);
	SetConsoleCursorInfo(consoleHandle, &former_info);
}

void hidecursor(HANDLE consoleHandle)
{
	GetConsoleCursorInfo(consoleHandle, &former_info);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void set_console_font_and_font_size(HANDLE consoleHandle) {
	former_cfi.cbSize = sizeof(former_cfi);
	GetCurrentConsoleFontEx(consoleHandle, FALSE, &former_cfi);
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 20;
	cfi.dwFontSize.Y = 20;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	wcscpy(cfi.FaceName, L"Courier");
	SetCurrentConsoleFontEx(consoleHandle, FALSE, &cfi);
}

void set_full_screen_mode(HANDLE consoleHandle) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	former_screen_size.X = csbi.dwSize.X; former_screen_size.Y = csbi.dwSize.Y;
	COORD coord;
	SetConsoleDisplayMode(consoleHandle, CONSOLE_FULLSCREEN_MODE, &coord);
}

void init_screen()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	set_full_screen_mode(consoleHandle);
	hidecursor(consoleHandle);
	set_console_font_and_font_size(consoleHandle);

}

void wait_and_get_direction(int* player1_snake_direction, int* player2_snake_direction) {
	DWORD64 start_time, check_time;
	start_time = GetTickCount64();
	check_time = start_time + WAIT_TIME_MILI_SEC; //GetTickCount returns time in miliseconds
	char key = 0;
	char player1_key_hit = 0;
	char player2_key_hit = 0;

	while (check_time > GetTickCount64()) {
		if (_kbhit()) {
			key = _getch();
			if (key == 0)
				key = _getch();
			if (key == 'w' || key == 'a' || key == 's' || key == 'd')
				player1_key_hit = key;
			if (key == 'i' || key == 'j' || key == 'k' || key == 'l')
				player2_key_hit = key;
		}
	}

	switch (player1_key_hit) {
	case 'w': if (*player1_snake_direction != DOWN) *player1_snake_direction = UP; break;
	case 'a': if (*player1_snake_direction != RIGHT) *player1_snake_direction = LEFT; break;
	case 's': if (*player1_snake_direction != UP) *player1_snake_direction = DOWN; break;
	case 'd': if (*player1_snake_direction != LEFT) *player1_snake_direction = RIGHT; break;
	default: break;
	}

	switch (player2_key_hit) {
	case 'i': if (*player2_snake_direction != DOWN) *player2_snake_direction = UP; break;
	case 'j': if (*player2_snake_direction != RIGHT) *player2_snake_direction = LEFT; break;
	case 'k': if (*player2_snake_direction != UP) *player2_snake_direction = DOWN; break;
	case 'l': if (*player2_snake_direction != LEFT) *player2_snake_direction = RIGHT; break;
	default: break;
	}
}

void draw_point(char point_content) {
	switch (point_content) {
	case PLAYER1_SNAKE_HEAD: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED); printf("@"); break;
	case PLAYER2_SNAKE_HEAD: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);  printf("@"); break;
	case PLAYER1_SNAKE_BODY: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);  printf("o"); break;
	case PLAYER2_SNAKE_BODY: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);  printf("o"); break;
	case MOUSE: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GRAY); printf("m"); break;
	case GOLDEN_STAR: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW); printf("*"); break;
	case POISON: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GREEN); printf("x"); break;
	case BERNARD_CLOCK: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), PINK); printf("c"); break;
	default: printf(" ");
	}
}

void draw_horizonatal_walls() {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	for (int i = 0; i < BOARD_SIZE + 2; ++i)
		printf("-");
	printf("\n");
}

void draw_board(char board_content[BOARD_SIZE][BOARD_SIZE]) {
	system("cls");
	draw_horizonatal_walls();
	for (int i = 0; i < BOARD_SIZE; i++) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		printf("|"); // vertical wall 
		for (int j = 0; j < BOARD_SIZE; j++)
			draw_point(board_content[i][j]);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		printf("|\n"); // vertical wall
	}
	draw_horizonatal_walls();
}

// prototypes
void init_screen();
void reset_console();
void wait_and_get_direction(int* player1_snake_direction, int* player2_snake_direction);
void draw_board(char board_content[BOARD_SIZE][BOARD_SIZE]);


//my function
DWORD time_hala();
void put_mouse(int numberMouse, char board_content[BOARD_SIZE][BOARD_SIZE]);
void  put_clock(int number, char board_content[BOARD_SIZE][BOARD_SIZE]);
void END_GAME(short resolt);



//this function return time
DWORD time_hala() {
	DWORD hala_time;
	hala_time = GetTickCount64();
	return hala_time;
}


//put mouse in board
void put_mouse(int numberMouse,char board_content[BOARD_SIZE][BOARD_SIZE]) {
	
	for (int i = 0; i < numberMouse; i++) {
		int x_mos, y_mos;
		
		///select random cell 
		x_mos = (rand() % BOARD_SIZE) ;
		y_mos = (rand() % BOARD_SIZE) ;

		///check that cell is empty or not
		switch (board_content[y_mos][x_mos])
		{
		case PLAYER1_SNAKE_HEAD: i -= 1; break;
		case PLAYER2_SNAKE_HEAD: i -= 1; break;
		case PLAYER1_SNAKE_BODY: i -= 1; break;
		case PLAYER2_SNAKE_BODY: i -= 1; break;
		case BERNARD_CLOCK:		 i -= 1; break;
		case MOUSE:				 i -= 1; break;
		default:
			board_content[y_mos][x_mos] = MOUSE;
			
			break;
		}
	}
	

}



//put BERNARD CLOCK in board     board_content[BOARD_SIZE][BOARD_SIZE]
void  put_clock(int number,char board_content[BOARD_SIZE][BOARD_SIZE]){

	for (int i = 0; i < number ; i++) {
		int x_clock, y_clock;
		x_clock = (rand() % BOARD_SIZE)  ;
		y_clock = (rand() % BOARD_SIZE)  ;
		switch (board_content[y_clock][x_clock])
		{
		case PLAYER1_SNAKE_HEAD: i -= 1; break;
		case PLAYER2_SNAKE_HEAD: i -= 1; break;
		case PLAYER1_SNAKE_BODY: i -= 1; break;
		case PLAYER2_SNAKE_BODY: i -= 1; break;
		case BERNARD_CLOCK:		 i -= 1; break;
		case MOUSE:				 i -= 1; break;
		default:
			board_content[y_clock][x_clock] = BERNARD_CLOCK;
			break;
		}
	}



}


//this function  check the resolt of game and show it
void END_GAME(short resolt) {
	reset_console();

	if (resolt == player1_win)
		printf("player1 win very good player1\ngood by\n");
	else if (resolt == player2_win)
		printf("player2 win very good player2\ngood by\n");
	else if (resolt == equal)
		printf("we don't have winner you are equal\ngood by\n");
}


int main() {


	init_screen(); //call this to make window adjustments

	 //First define the variables you need during the game
		
	 
	 
		//this show the position evry thing in game	
	char board_content[BOARD_SIZE][BOARD_SIZE];



	//this show the part and size of evry snake	
	char snake1[MAX_LEN_SNAKES] = { PLAYER1_SNAKE_HEAD,PLAYER1_SNAKE_BODY,PLAYER1_SNAKE_BODY };
	short len_snake1 = INITIAL_SNAKE_LENGTH;

	char snake2[MAX_LEN_SNAKES] = { PLAYER2_SNAKE_HEAD,PLAYER2_SNAKE_BODY,PLAYER2_SNAKE_BODY };
	short len_snake2 = INITIAL_SNAKE_LENGTH;


	// show the position parts of snakes whit x and y
	short x_snake1[MAX_LEN_SNAKES] = { 0,0,0 };
	short y_snake1[MAX_LEN_SNAKES];
	for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++)
		y_snake1[i] = INITIAL_SNAKE_LENGTH - i;

	short x_snake2[MAX_LEN_SNAKES] = { 39,39,39 };
	short y_snake2[MAX_LEN_SNAKES] ;
	for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++)
		y_snake2[i] =BOARD_SIZE - INITIAL_SNAKE_LENGTH + i;


	//Initialize value of board
	for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++) {
		board_content[y_snake1[i]][x_snake1[i]] = snake1[i];

		board_content[y_snake2[i]][x_snake2[i]] = snake2[i];
	}


	//this variable is for snake direction
	int player1_snake_direction, player2_snake_direction;
	player1_snake_direction = DOWN;
	player2_snake_direction = UP;

	//this variable save the next and former position of snak
	//use for snake moving  
	short next_x_1, former_x_1, next_y_1, former_y_1;
	short next_x_2, former_x_2, next_y_2, former_y_2;

	//this handle the resolt of game
	short resolt = no ;

	//this variables use to handle time for bernard clock
	DWORD now_time, befor_time, freez_time;
	
	//show that the snake are freez or not
	short freez_mood_1 = no , freez_mood_2 = no;


	//this is show clock is in board or no
	short exist_clock = 0;

	


	
	// put 20 mouse in  random cell
	srand(time(0));
	put_mouse(NUMBER_OF_MOUSES, board_content);






	befor_time = time_hala();




	//Do not forget to Initialize your variables, specially board_content
	//init_screen(); //call this to make window adjustments
	while (TRUE) {




		now_time = time_hala();

		//check the period of apperenc of BERNARD CLOCK and existing of it
		if ((now_time - befor_time) > BERNARD_CLOCK_APPEARANCE_CHECK_PERIOD_MILI_SEC &&
			exist_clock == no ) {

			//chang befor time for handle time
			befor_time = now_time;
			
			//this variable show the chanc of appearance BERNARD CLOCK
			int possibility = rand() % 100;

			//chec that the chance of appearance BERNARD CLOCK is ok or not
			if (possibility < BERNARD_CLOCK_APPEARANCE_CHANCE_PERCENT) {
				
				//chang the mood of existing of CLOCK to ok
				exist_clock = ok ;
				
				//isert a new BERNARD CLOCK in board content
				put_clock(1, board_content);
				
			}		
		}









					
							///condition of end game
		
		int encount = 0;//count the number of encounter
		
		//check to encounter the snake2 head whit snake1 body
		// if condition is true player 2 lose game
		for (int i = 1; i < len_snake1; i++) {
			if (x_snake2[0] == x_snake1[i] && y_snake2[0] == y_snake1[i]) {
				resolt = player1_win;
				encount += 1;
				break;
			}
		}

		//check to encounter the snake1 head whit snake2 body
		// if condition is true player 1 lose game
		for (int i = 1; i < len_snake2; i++) {
			if (x_snake1[0] == x_snake2[i] && y_snake1[0] == y_snake2[i]) {
				resolt = player2_win;
				encount += 1;
				break;
			}
		}

		//check for encounter  snake1 head whit snake 2 head
		// or both snake heads encounter whit other snake body
		//the size of snake determine the winner 
		if ( (x_snake1[0] == x_snake2[0] && y_snake1[0] == y_snake2[0]) || encount==2) {

			if (len_snake1 > len_snake2)
				resolt = player1_win;

			else if (len_snake1 < len_snake2)
				resolt = player2_win;

			else
				resolt = equal ;

		}


		//check that the resolt is determined or not
		//is yes exit the while and show resolt
		if (resolt > no )
			break;



		//print board
		draw_board(board_content);
		
		//do some check after key press, and let snakes move forward one house
		wait_and_get_direction(&player1_snake_direction, &player2_snake_direction);




							/// moving pleyer 1 snake 1

		//this switch handle moving snak1 for pleyer 1 
		//and find the next position of snake head
		//attention!!!
		//beacuas function of _%_ for number<0 is wrong insert the (+ 40) 
		switch (player1_snake_direction)
		{
		case UP://former_x_1 = x_snake1[0]; former_y_1 = y_snake1[0];
			next_x_1 = x_snake1[0]; next_y_1 = (y_snake1[0] - 1 + 40) % 40;
			break;

		case DOWN://former_x_1 = x_snake1[0]; former_y_1 = y_snake1[0];
			next_x_1 = x_snake1[0]; next_y_1 = (y_snake1[0] + 1 + 40) % 40;
			break;

		case LEFT://former_x_1 = x_snake1[0]; former_y_1 = y_snake1[0];
			next_x_1 = (x_snake1[0] - 1 + 40) % 40; next_y_1 = (y_snake1[0]) % 40;
			break;

		case RIGHT://former_x_1 = x_snake1[0]; former_y_1 = y_snake1[0];
			next_x_1 = (x_snake1[0] + 1 + 40) % 40; next_y_1 = (y_snake1[0]) % 40;
			break;

		default:
			break;
		}


		now_time = time_hala();

		//check that the condition of end freezing is ok or not
		if (freez_mood_1 != ok || (now_time-freez_time)>BERNARD_CLOCK_FROZEN_TIME_MILI_SEC) {
			
			//chang the freezing form of snake1
			freez_mood_1 = no ;
			

			
			
			//handle moving snake 1 and 
			//find the next and former position of evry part of snake 1
			for (int i = 0; i < len_snake1; i++) {
				former_x_1 = x_snake1[i];
				former_y_1 = y_snake1[i];

				x_snake1[i] = next_x_1;
				y_snake1[i] = next_y_1;

				next_x_1 = former_x_1;
				next_y_1 = former_y_1;

			}

		}
		
		
		//check for eating mouse
		if (board_content[y_snake1[0]][x_snake1[0]] == MOUSE) {


			//check for size of snake if equal 30 dont increas size
			if (len_snake1 < MAX_LEN_SNAKES) {
				/// increas the size of snake 
				len_snake1 += 1;
				x_snake1[len_snake1 - 1] = former_x_1;
				y_snake1[len_snake1 - 1] = former_y_1;
				snake1[len_snake1 - 1] = PLAYER1_SNAKE_BODY;


			}
			/// put a new mouse in board
			put_mouse(1, board_content);



		}
		//if mouse not eated or snake size=30
		// we clear the last former position of last snake part
		if (board_content[y_snake1[0]][x_snake1[0]] != MOUSE || len_snake1 == MAX_LEN_SNAKES)
			board_content[former_y_1][former_x_1] = EMPTY;



		//check for encounter snake head whit its body
		for (int i = 1; i < len_snake1; i++) {
			if (x_snake1[0] == x_snake1[i] && y_snake1[0] == y_snake1[i]) {
				resolt = player2_win;
				break;
			}
		}







					/// moving pleyer 2 snake 2

		//this switch handle moving snake 2 for pleyer 2 
		//and find the next position of snake head
		//attention!!!
		//beacuas function of _%_ for number<0 is wrong insert the (+ 40) 
		switch (player2_snake_direction)
		{
		case UP:former_x_2 = x_snake2[0]; former_y_2 = y_snake2[0];
			next_x_2 = x_snake2[0]; next_y_2 = (y_snake2[0] - 1 + 40) % 40;
			break;

		case DOWN:former_x_2 = x_snake2[0]; former_y_2 = y_snake2[0];
			next_x_2 = x_snake2[0]; next_y_2 = (y_snake2[0] + 1 + 40) % 40;
			break;

		case LEFT:former_x_2 = x_snake2[0]; former_y_2 = y_snake2[0];
			next_x_2 = (x_snake2[0] - 1 + 40) % 40; next_y_2 = (y_snake2[0]) % 40;
			break;

		case RIGHT:former_x_2 = x_snake2[0]; former_y_2 = y_snake2[0];
			next_x_2 = (x_snake2[0] + 1 + 40) % 40; next_y_2 = (y_snake2[0]) % 40;
			break;

		default:
			break;
		}




		now_time = time_hala();
		//check that the condition of end freezing is ok or not
		if (freez_mood_2 != ok || (now_time - freez_time) > BERNARD_CLOCK_FROZEN_TIME_MILI_SEC) {
			
			//chang the freezing form of snake2
			freez_mood_2 = no ;


			//handle moving snake 2  and 
			//find the next and former position of evry part of snake 2
			for (int i = 0; i < len_snake2; i++) {
				former_x_2 = x_snake2[i];
				former_y_2 = y_snake2[i];


				x_snake2[i] = next_x_2;
				y_snake2[i] = next_y_2;

				next_x_2 = former_x_2;
				next_y_2 = former_y_2;

			}
		}

		//check for eating mouse
		if (board_content[y_snake2[0]][x_snake2[0]] == MOUSE) {


			//check for size of snake if larger than 29 dont increas size
			if (len_snake2 < MAX_LEN_SNAKES) {
				/// increas the size of snake 
				len_snake2 += 1;
				x_snake2[len_snake2 - 1] = former_x_2;
				y_snake2[len_snake2 - 1] = former_y_2;
				snake2[len_snake2 - 1] = PLAYER2_SNAKE_BODY;
			}

			/// put a new mouse in board
			put_mouse(1, board_content);
		}

		//if mouse not eated or snake size=30
		// we clear the last former position of last snake part
		if (board_content[y_snake2[0]][x_snake2[0]] != MOUSE || len_snake2 == MAX_LEN_SNAKES)
			board_content[former_y_2][former_x_2] = EMPTY;



		//check for encounter snake 2 head whit its body
		for (int i = 1; i < len_snake2; i++) {
			if (x_snake2[0] == x_snake2[i] && y_snake2[0] == y_snake2[i]) {
				resolt = player1_win;
				break;
			}
		}



		//check that snake1 eat BERNARD CLOCK or not and handle time freezing
		if (board_content[y_snake1[0]][x_snake1[0]] == BERNARD_CLOCK) {
			
			//chang freezing form for snake2
			freez_mood_2 = ok ;
						
			//chang the mood of existing CLOCK in board
			exist_clock = no ;
			
			//from now start freezing time
			freez_time = time_hala();
		}

		
		//check that snake2 eat BERNARD CLOCK or not and handle time freezing
		if (board_content[y_snake2[0]][x_snake2[0]] == BERNARD_CLOCK) {
			
			//chang freezing form for snake1
			freez_mood_1 = ok ;
			
			//chang the mood of existing CLOCK in board
			exist_clock = no ;
			
			//from now start freezing time
			freez_time = time_hala();
		}






		// insert new position of snakse in board_content		
		for (int i = 0; i < len_snake1; i++) {
			board_content[y_snake1[i]][x_snake1[i]] = snake1[i];


		}

		for (int i = 0; i < len_snake2; i++) {
			board_content[y_snake2[i]][x_snake2[i]] = snake2[i];

		}




	}// end total while

	
	 
	 //show the resolt of game
	END_GAME(resolt);


	_getch();

	return 0;
}