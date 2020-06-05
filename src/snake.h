#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <time.h>
#include <math.h>
#include <stdlib.h>

#define SNAKE_ARRAY_SIZE 310

#ifdef _WIN32
//Windows Libraries
#include <conio.h>
#include <Windows.h>
//Windows Constants
//Controls
#define UP_ARROW 72
#define LEFT_ARROW 75
#define RIGHT_ARROW 77
#define DOWN_ARROW 80

#define ENTER_KEY 13

const char SNAKE_HEAD = (char)64;
const char SNAKE_BODY = (char)48;
const char WALL = (char)127;
const char FOOD = (char)14;
const char BLANK = ' ';


/**
* 커서의 위치를 화면 왼쪽상단을 기준으로 x,y 만큼 이동
*
* @param int x : x좌표
* @param int y : y좌표
**/
void gotoxy(int x, int y)
{
	COORD Pos;
	Pos.X = (short)x;
	Pos.Y = (short)y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
	//커서값을 이동시키는 함수
	//getStdHandle() 파라미터로 넘겨준 값의 핸들값을 반환해준다.
}

/**
* 콘솔화면의 내용을 모두 지운다.
**/
void clrscr()
{
	system("cls");
	return;
}

#else
//Linux Libraries
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

//Linux Constants

//Controls (arrow keys for Ubuntu) 
#define UP_ARROW (char)'A' //Originally I used constants but borland started giving me errors, so I changed to #define - I do realize that is not the best way.
#define LEFT_ARROW (char)'D'
#define RIGHT_ARROW (char)'C'
#define DOWN_ARROW (char)'B'

#define ENTER_KEY 10

const char SNAKE_HEAD = 'X';
const char SNAKE_BODY = '#';
const char WALL = '#';
const char FOOD = '*';
const char BLANK = ' ';

//Linux Functions - These functions emulate some functions from the windows only conio header file
//Code: http://ubuntuforums.org/showthread.php?t=549023
void gotoxy(int x, int y)
{
	printf("%c[%d;%df", 0x1B, y, x);
}

//http://cboard.cprogramming.com/c-programming/63166-kbhit-linux.html
int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}

//http://www.experts-exchange.com/Programming/Languages/C/Q_10119844.html - posted by jos
char getch()
{
	char c;
	system("stty raw");
	c = getchar();
	system("stty sane");
	//printf("%c",c);
	return(c);
}

void clrscr()
{
	system("clear");
	return;
}
//End linux Functions
#endif

//This should be the same on both operating systems
#define EXIT_BUTTON 27 //ESC
#define PAUSE_BUTTON 112 //P
#define CUT_BUTTON 99 //C

// Function Prototypes.
char waitForAnyKey(void);
int getGameSpeed2(void);
int menuSelector(int x, int y, int yStart);
int selectGameMode(void);
void pauseMenu(void);
int checkKeysPressed(int direction);
int collisionSnake(int x, int y, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int detect);
int generateFood(int foodXY[], int width, int height, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength);
void moveSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction);
void move(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction);
int eatFood(int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[]);
int collisionDetection(int snakeXY[][SNAKE_ARRAY_SIZE], int consoleWidth, int consoleHeight, int snakeLength);
void refreshInfoBar(int score, int speed);
void createHighScores(void);
int getLowestScore();
void resetRankData();
void inputScore(int score);
void displayHighScores(void);
void youWinScreen(void);
void gameOverScreen(void);
int cutTail(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength);
void startGame(int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[], int consoleWidth, int consoleHeight, int snakeLength, int direction, int score, int speed);
void loadEnviroment(int consoleWidth, int consoleHeight);
void loadSnake(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength);
void prepairSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength);
void loadGame(void);
int menuSelector(int x, int y, int yStart);
void welcomeArt(void);
void controls(void);
void exitYN(void);
int mainMenu(void);

#endif
