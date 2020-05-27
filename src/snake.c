/*
Author: Matthew Vlietstra
Version: 0.5
Date: 28/09/2010

Discription:
This is a console snake game that can (or should) work in linux & windows environments.

Windows:
Compile with borland

Linux:
Please note, tested under Ubuntu not sure if it works in other linux environments. I recommend compiling with borland under windows for best results.
Compile with gcc in linux using the following command:
gcc snake.c 뻦m 뻩 snake.out

Cygwin:
Although this program may compile/ run in Cygwin it runs slowly.	
				
*/

#include <stdio.h>
#include <time.h>
#include <math.h>

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
	
	const char SNAKE_HEAD = (char)177;
	const char SNAKE_BODY = (char)178;
	const char WALL = (char)219;	
	const char FOOD = (char)254;
	const char BLANK = ' ';

	void gotoxy(int x,int y)
	{
		COORD Pos;
		Pos.X = (short)x;
		Pos.Y = (short)y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
	}

	void clrscr()
	{
		system("cls");
		return;
	}
	
#else
	//Linux Libraries
	#include <stdlib.h>
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
	void gotoxy(int x,int y)
	{
		printf("%c[%d;%df",0x1B,y,x);
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

	  if(ch != EOF)
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
		c= getchar();
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

char waitForAnyKey(void)
{
	int pressed;
	
	while(!_kbhit());
	
	pressed = _getch();
	
	return((char)pressed);
}

int getGameSpeed(void)
{
	int speed;
	clrscr();
	
	do
	{
		gotoxy(10,5);
		printf("Select The game speed between 1 and 9.");
		speed = waitForAnyKey()-48;
	} while(speed < 1 || speed > 9);
	return(speed);
}

void pauseMenu(void)
{
	gotoxy(28,23);
	printf("**Paused**");
	
	waitForAnyKey();
	gotoxy(28,23);
	printf("            ");

	return;
}

//This function checks if a key has pressed, then checks if its any of the arrow keys/ p/esc key. It changes direction acording to the key pressed.
int checkKeysPressed(int direction)
{
	int pressed;
	
	if(_kbhit()) //If a key has been pressed
	{
		pressed=_getch();
		if (direction != pressed)
		{
			if(pressed == DOWN_ARROW && direction != UP_ARROW)
				direction = pressed;
			else if (pressed == UP_ARROW && direction != DOWN_ARROW)
				direction = pressed;
			else if (pressed == LEFT_ARROW && direction != RIGHT_ARROW)
				direction = pressed;
			else if (pressed == RIGHT_ARROW && direction != LEFT_ARROW)
				direction = pressed;
			else if (pressed == EXIT_BUTTON || pressed == PAUSE_BUTTON)
				pauseMenu();
		}
	}
	return(direction);
}

//Cycles around checking if the x y coordinates ='s the snake coordinates as one of this parts
//One thing to note, a snake of length 4 cannot collide with itself, therefore there is no need to call this function when the snakes length is <= 4
int collisionSnake (int x, int y, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int detect)
{
	int i;
	for (i = detect; i < snakeLength; i++) //Checks if the snake collided with itself
	{
		if ( x == snakeXY[0][i] && y == snakeXY[1][i])
			return(1);
	}
	return(0);
}

//Generates food & Makes sure the food doesn't appear on top of the snake <- This sometimes causes a lag issue!!! Not too much of a problem tho
int generateFood(int foodXY[], int width, int height, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{	
	do
	{
		srand ( (unsigned int)time(NULL) );
		foodXY[0] = rand() % (width-2) + 2;
		srand ( (unsigned int)time(NULL) );
		foodXY[1] = rand() % (height-6) + 2;
	} while (collisionSnake(foodXY[0], foodXY[1], snakeXY, snakeLength, 0)); //This should prevent the "Food" from being created on top of the snake. - However the food has a chance to be created ontop of the snake, in which case the snake should eat it...

	gotoxy(foodXY[0] ,foodXY[1]);
	printf("%c", FOOD);
	
	return(0);
}

/*
뱀을 이동시킨 후 뱀의 몸통부분과 뱀의 머리 방향을 재설정하는 함수.
뱀을 나타내기위한 배열, 뱀의 길이, 이동방향을 파라미터로 받는다.
이동 후에 뱀의 몸통부분 배열을 재배열하고, 이동방향에 따라 뱀의 머리 방향을 설정한다.
*/
void moveSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
	int i;
	
	//뱀의 배열의 각 인덱스의 값을 한칸씩 뒤로 밀어 뱀의 몸통부분이 이동한 것을 구현한다.
	for( i = snakeLength-1; i >= 1; i-- )
	{
		snakeXY[0][i] = snakeXY[0][i-1]; //뱀의 위, 아래를 표현하는 배열
		snakeXY[1][i] = snakeXY[1][i-1]; //뱀의 왼쪽, 오른쪽을 표현하는 배열
	}	
	
	//파라미터로 받은 방향에 따라 뱀의 머리 방향을 설정한다.
	switch(direction)
	{
		case DOWN_ARROW:
			snakeXY[1][0]++;
			break;
		case RIGHT_ARROW:
			snakeXY[0][0]++;
			break;
		case UP_ARROW:
			snakeXY[1][0]--;
			break;
		case LEFT_ARROW:
			snakeXY[0][0]--;
			break;			
	}
	
	return;
}

void move(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
	int x;
	int y;

	//Remove the tail ( HAS TO BE DONE BEFORE THE ARRAY IS MOVED!!!!! )
	x = snakeXY[0][snakeLength-1];
	y = snakeXY[1][snakeLength-1];
	
	gotoxy(x,y);
	printf("%c",BLANK);	
	
	//Changes the head of the snake to a body part
	gotoxy(snakeXY[0][0],snakeXY[1][0]);	
	printf("%c", SNAKE_BODY);
	
	moveSnakeArray(snakeXY, snakeLength, direction);
	
	gotoxy(snakeXY[0][0],snakeXY[1][0]);	
	printf("%c",SNAKE_HEAD);
	
	gotoxy(1,1); //Gets rid of the darn flashing underscore.
	
	return;
}

/**
* 먹이와 접촉했을때, 먹이를 먹을 수 있는지 확인하고 가능하다면 먹는 함수.
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : Snake의 좌표와 길이
* @param int foodXY[] : 먹이의 좌표
* @return 0: 먹이가 존재하지 않음, 1: 정상적으로 먹이를 먹은 경우.
**/
int eatFood(int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[]) 
{
	if (snakeXY[0][0] == foodXY[0] && snakeXY[1][0] == foodXY[1]) //Snake head의 x, y 좌표가 먹이의 위치와 일치할 경우.
	{
		foodXY[0] = 0;
		foodXY[1] = 0; // fixme: 무한 반복되는 버그가 있는 것으로 알려짐.  여전히 버그가 발생하는지 확인 필요.
		
		printf("\7"); //비프음
		return(1); // 1
	}		
	
	return(0);
}

int collisionDetection(int snakeXY[][SNAKE_ARRAY_SIZE], int consoleWidth, int consoleHeight, int snakeLength ) //Need to Clean this up a bit
{
	int colision = 0;
	if ((snakeXY[0][0] == 1) || (snakeXY[1][0] == 1) || (snakeXY[0][0] == consoleWidth) || (snakeXY[1][0] == consoleHeight - 4)) //Checks if the snake collided wit the wall or it's self
		colision = 1;
	else
		if (collisionSnake(snakeXY[0][0], snakeXY[1][0], snakeXY, snakeLength, 1)) //If the snake collided with the wall, theres no point in checking if it collided with itself.
			colision = 1;
			
	return(colision);
}

/**
* 화면 하단의 정보 바를 갱신.
*
* @param int score : 플레이어의 현재 점수
* @param int speed : 현재 게임의 속도 (단계)
**/
void refreshInfoBar(int score, int speed)
{
	gotoxy(5,23);
	printf("Score: %d", score);
	
	gotoxy(5,24);
	printf("Speed: %d", speed);

	gotoxy(52,23);
	printf("Coder: Matthew Vlietstra");

	gotoxy(52,24);
	printf("Version: 0.5");
	
	return;
}

//**************HIGHSCORE STUFF**************//

//-> The highscores system seriously needs to be clean. There are some bugs, entering a name etc

void createHighScores(void)
{
	FILE *file; 
	int i;

	file = fopen_s(&file,"highscores.txt","w+");
	
	if(file == NULL)
	{
		printf("FAILED TO CREATE HIGHSCORES!!! EXITING!");
		exit(0);	
	}
	
	for(i = 0; i < 5; i++)
	{
		fprintf(file,"%d",i+1);
		fprintf(file,"%s","\t0\t\t\tEMPTY\n");
	}	
	
	fclose(file);
	return;
}

int getLowestScore()
{
	FILE *fp;
	char* str = (char*)malloc(sizeof(char)*128);
	int lowestScore = 0;
	int i;
	int intLength;
	
	if((fp = fopen_s(&fp,"highscores.txt", "r")) == NULL)
	{
		//Create the file, then try open it again.. if it fails this time exit.
		createHighScores(); //This should create a highscores file (If there isn't one)
		if((fp = fopen("highscores.txt", "r")) == NULL)
			exit(1);
	}

	while(!feof(fp))
	{
		fgets(str, 126, fp);  
	}
	fclose(fp);	
	
	i=0;
	
	//Gets the Int length
	while(str[2+i] != '\t')
	{
		i++;
	}
	
	intLength = i;
	
	//Gets converts the string to int
	for(i=0;i < intLength; i++)
	{
		lowestScore = lowestScore + ((int)str[2+i] - 48) * (int)pow(10,intLength-i-1);
	}

	return(lowestScore);
}

void inputScore(int score) //This seriously needs to be cleaned up
{
	FILE *fp;
	FILE *file; 
	char* str = (char*)malloc(sizeof(char)*128);
	int fScore;
	int i, s, y;
	int intLength;
	int* scores = (int*)malloc(sizeof(int)*5);
	int x;
	char* highScoreName = (char*)malloc(sizeof(char)*20);
	char highScoreNames[5][20];
	
	char* name = (char*)malloc(sizeof(char)*20);
	
	int entered = 0;
	
	clrscr(); //clear the console
	
	fopen_s(&fp, "highscore.txt", "r");
	if(fp == NULL)
	    exit(1);
	gotoxy(10,5);
	printf("Your Score made it into the top 5!!!");
	gotoxy(10,6);
	printf("Please enter your name: ");
	gets_s(name,sizeof(name));
	
	x = 0;
	while(!feof(fp))
	{
		fgets(str, 126, fp);  //Gets a line of text
		
		i=0;
		
		//Gets the Int length
		while(str[2+i] != '\t')
		{
			i++;
		}
		
		s = i;
		intLength = i;
		i=0;
		while(str[5+s] != '\n')
		{
			//printf("%c",str[5+s]);
			highScoreName[i] = str[5+s];
			s++;
			i++;
		}
		//printf("\n");
		
		fScore = 0;
		//Gets converts the string to int
		for(i=0;i < intLength; i++)
		{
			//printf("%c", str[2+i]);
			fScore = fScore + ((int)str[2+i] - 48) * (int)pow(10,intLength-i-1);
		}
		
		if(score >= fScore && entered != 1)
		{
			scores[x] = score;
			strcpy(highScoreNames[x], name);
			
			//printf("%d",x+1);
			//printf("\t%d\t\t\t%s\n",score, name);		
			x++;
			entered = 1;
		}
		
		//printf("%d",x+1);
		//printf("\t%d\t\t\t%s\n",fScore, highScoreName);
		//strcpy(text, text+"%d\t%d\t\t\t%s\n");
		strcpy(highScoreNames[x], highScoreName);
		scores[x] = fScore;
		
		//highScoreName = "";
		for(y=0;y<20;y++)
		{
			highScoreName[y] = NULL;
		}
		
		x++;
		if(x >= 5)
			break;
	}
	
	fclose(fp);
	
	fopen_s(&fp, "highscores.txt", "w+");
	
	for(i=0;i<5;i++)
	{
		//printf("%d\t%d\t\t\t%s\n", i+1, scores[i], highScoreNames[i]);
		fprintf(fp, "%d\t%d\t\t\t%s\n", i+1, scores[i], highScoreNames[i]);	
	}

	if(!fp == '0')
	    fclose(fp);
	
	return;
}

void displayHighScores(void) //NEED TO CHECK THIS CODE!!!
{
	FILE *fp;
	char* str = (char*)malloc(sizeof(char)*128);
	int y = 5;
	
	clrscr(); //clear the console
	fopen_s(&fp, "highscores.txt", "r");

	if(fp == NULL) {
		//Create the file, then try open it again.. if it fails this time exit.
		createHighScores(); //This should create a highscores file (If there isn't one)
		if(!fopen_s(&fp, "highscores.txt", "r"))
			exit(1);
	}
	
	gotoxy(10,y++);
	printf("High Scores");	
	gotoxy(10,y++);
	printf("Rank\tScore\t\t\tName");
	while(!feof(fp)) {
		gotoxy(10,y++);
		if(fgets(str, 126, fp)) 
			printf("%s", str);
	}

	fclose(fp);	//Close the file
	gotoxy(10,y++);
	
	printf("Press any key to continue...");
	waitForAnyKey();	
	return;
}

//**************END HIGHSCORE STUFF**************//

/**
* 플레이어가 게임에서 이겼을 경우 'YOU WIN'
**/
void youWinScreen(void)
{
	int x = 6, y = 7; // 메시지 시작 좌표 지정 (6,7)
	gotoxy(x,y++); // x, y 좌표값을 따라 이동, 후위 연산자이므로 (6, 7)로 이동후 y값이 8로 증가됨.
	printf("'##:::'##::'#######::'##::::'##::::'##:::::'##:'####:'##::: ##:'####:");
	gotoxy(x,y++); // x 값을 유지한 상태로 한 줄 아래로 이동... (이후 반복)
	printf(". ##:'##::'##.... ##: ##:::: ##:::: ##:'##: ##:. ##:: ###:: ##: ####:");
	gotoxy(x,y++);
	printf(":. ####::: ##:::: ##: ##:::: ##:::: ##: ##: ##:: ##:: ####: ##: ####:");
	gotoxy(x,y++);
	printf("::. ##:::: ##:::: ##: ##:::: ##:::: ##: ##: ##:: ##:: ## ## ##:: ##::");
	gotoxy(x,y++);
	printf("::: ##:::: ##:::: ##: ##:::: ##:::: ##: ##: ##:: ##:: ##. ####::..:::");
	gotoxy(x,y++);
	printf("::: ##:::: ##:::: ##: ##:::: ##:::: ##: ##: ##:: ##:: ##:. ###:'####:");
	gotoxy(x,y++);
	printf("::: ##::::. #######::. #######:::::. ###. ###::'####: ##::. ##: ####:");
	gotoxy(x,y++);
	printf(":::..::::::.......::::.......:::::::...::...:::....::..::::..::....::");
	gotoxy(x,y++);	
	
	waitForAnyKey(); //사용자 키 입력이 있을때까지 대기.
	clrscr(); //clear the console
	return;
}

/**
* 게임 오버되었을 때 'GAME OVER' 메시지를 ascii art 형태로 출력.
**/
void gameOverScreen(void)
{
	int x = 17, y = 3; // 메시지 시작 좌표 지정 (17,3)
	
	//http://www.network-science.de/ascii/ <- Ascii Art Gen : 아스키 아트 생성 사이트 URL (참조용 주석)
	
	gotoxy(x,y++); // x, y 좌표값을 따라 이동, 후위 연산자이므로 (17, 3)으로 이동후 y값이 8로 증가됨.
	printf(":'######::::::'###::::'##::::'##:'########:\n");
	gotoxy(x,y++); // x 값을 유지한 상태로 한 줄 아래로 이동... (이후 반복)
	printf("'##... ##::::'## ##::: ###::'###: ##.....::\n");
	gotoxy(x,y++);
	printf(" ##:::..::::'##:. ##:: ####'####: ##:::::::\n");
	gotoxy(x,y++);
	printf(" ##::'####:'##:::. ##: ## ### ##: ######:::\n");
	gotoxy(x,y++);
	printf(" ##::: ##:: #########: ##. #: ##: ##...::::\n");
	gotoxy(x,y++);
	printf(" ##::: ##:: ##.... ##: ##:.:: ##: ##:::::::\n");
	gotoxy(x,y++);
	printf(". ######::: ##:::: ##: ##:::: ##: ########:\n");
	gotoxy(x,y++);
	printf(":......::::..:::::..::..:::::..::........::\n");
	gotoxy(x,y++);
	printf(":'#######::'##::::'##:'########:'########::'####:\n");
	gotoxy(x,y++);
	printf("'##.... ##: ##:::: ##: ##.....:: ##.... ##: ####:\n");
	gotoxy(x,y++);
	printf(" ##:::: ##: ##:::: ##: ##::::::: ##:::: ##: ####:\n");
	gotoxy(x,y++);
	printf(" ##:::: ##: ##:::: ##: ######::: ########::: ##::\n");
	gotoxy(x,y++);
	printf(" ##:::: ##:. ##:: ##:: ##...:::: ##.. ##::::..:::\n");
	gotoxy(x,y++);
	printf(" ##:::: ##::. ## ##::: ##::::::: ##::. ##::'####:\n");
	gotoxy(x,y++);
	printf(". #######::::. ###:::: ########: ##:::. ##: ####:\n");
	gotoxy(x,y++);
	printf(":.......::::::...:::::........::..:::::..::....::\n");
	
	waitForAnyKey(); //사용자 키 입력이 있을때까지 대기.
	clrscr(); //화면을 비워줌
	return;
}

/**
* 게임 시작시 호출되는 게임의 메인 로직.
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : 플레이어의 X, Y 좌표값.
* @param int foodXY[][] : 먹이의 X, Y 좌표값.
* @param int consoleWitdh : 창의 넓이
* @param int consoleHeight : 창의 높이
* @param int snakeLength : snake의 초기 길이
* @param int direction : 초기 진행 방향
* @param int score : 초기 점수값
* @param int speed : 초기 게임의 속도 (단계)
**/
//Todo /maybe-later: 코드 정리 필요.
void startGame( int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[], int consoleWidth, int consoleHeight, int snakeLength, int direction, int score, int speed)
{
	int gameOver = 0; // gameOver 여부 체크 변수. (0: 게임 진행, 1: 게임 오버, 2: 승리)
	clock_t endWait; // 대기 종료 시간을 담을 변수.
	
	//CLOCKS_PER_SEC-(n-1)*(CLOCKS_PER_SEC/10)
	int waitMili = CLOCKS_PER_SEC-(speed)*(CLOCKS_PER_SEC/10);	// 현재 게임 속도에 맞는 대기 시간 설정 (대기 시간 : 1초 - 게임속도(단계) * 0.1초)
	int tempScore = 10*speed; // 속도 증가 시점에서 현재 스코어와 비교할 기준값을 위한 임시 변수. 초기값 : 10 * 속도.
	int oldDirection = 0; // 직전 방향값을 저장하기 위한 변수
	int canChangeDirection = 1; // 방향 전환이 가능한 상태인지 저장 (0: 불가능, 1: 가능)
	//int seconds = 1;

	endWait = clock() + waitMili; // 현재 시간 + 대기 시간을 대기 종료 시간으로 설정.

	do
	{
		if(canChangeDirection) //방향 전환이 가능한 상태인 경우.
		{
			oldDirection = direction; // 현재 방향을 oldDirection에 저장.
			direction = checkKeysPressed(direction); //새 방향값을 받아 대입.
		}
		
		if(oldDirection != direction) // 직전 방향과 다른 경우
			canChangeDirection = 0; //방향 전환이 불가능하도록 설정. (snake가 스스로 충돌하는 것을 방지)
			
		if(clock() >= endWait) // 대기 종료 시간이 지난 경우. (컴퓨터 속도에 따라 동작.)
		{
			//gotoxy(1,1);
			//printf("%d - %d",clock() , endWait);
			move(snakeXY, snakeLength, direction); // Snake를 지정한 방향으로 이동.
			canChangeDirection = 1;  //다시 방향 전환이 가능하도록.

				
			if(eatFood(snakeXY, foodXY)) // 현재 좌표에 먹이가 존재한다면
			{
				generateFood( foodXY, consoleWidth, consoleHeight, snakeXY, snakeLength); //새로운 먹이 생성.
				snakeLength++; //Snake의 길이 증가.
				score+=speed; //현재 속도만큼 점수 부여.
				//x++;
				//gotoxy(1,1);
				//printf("%d >= %d", 10*speed+score, tempScore);
				if( score >= 10*speed+tempScore) // 현재 점수가 게임 속도 * 10 + 기준값이 되는 현재 단계 스코어보다 큰 경우 게임 속도 증가 처리.
				//if( 2 >= 2)
				{
					speed++; // 게임 속도 증가.
					tempScore = score; // 판단 기준값을 현재 스코어로 변경.

					if(speed <= 9)//this needs to be fixed // 게임 속도가 9 이하인 경우
						waitMili = waitMili - (CLOCKS_PER_SEC/10); // 대기 시간 단축 : 기존 대기시간 - 1/10초
					else // 게임 속도가 9보다 큰 경우
					{
						if(waitMili >= 40) // 현재 대기 시간이 게임 클럭 속도 기준 40 이상인 경우에만 속도 증가. (그보다 더 빠를 경우 현실적으로 게임 진행 불가한 속도이므로.)
							waitMili = waitMili - (CLOCKS_PER_SEC/200); // 기존 대기시간 - 1/200초. (속도 증가폭 낮춤)
						
					}
					//level++;
					//gotoxy(1,2);
					//printf("    ");
					//gotoxy(1,2);
					//printf("%d",waitMili);
					//x = 0;
				}
				
				refreshInfoBar(score, speed); // 하단 바 갱신.
			}
			
			// HACK : clock() 리셋 방법 찾아서 수정 필요. (현재는 동작하는 것으로 보임)
			endWait = clock() + waitMili; //현재 시간 기준으로 대기 종료 시간 생신. 
		}
		
		gameOver = collisionDetection(snakeXY, consoleWidth, consoleHeight, snakeLength); // 게임 오버 조건 체크해 종료 코드 대입 (Snake가 창 끝에 닿았거나 Snake끼리 닿은 경우 1 대입 = 게임 오버)

		if(snakeLength >= SNAKE_ARRAY_SIZE-5) // Snake의 길이가 (설정한 최대 길이 - 5) 이상인 경우 게임 승리. (배열 크기보다 길어질 경우 크래시 발생하므로.)
		{
			gameOver = 2;//You Win! <- doesn't seem to work - NEED TO FIX/TEST THIS : gameOver에 승리시를 뜻하는 코드 2 삽입. (while문 탈출 유도) => 동작하지 않는다함.
			score+=1500; //승리시, 보너스 포인트 1500원 지급.
		}
		
	} while (!gameOver); //gameOver값이 0이 아닐때까지 loop.
	
	switch(gameOver) //gameOver 값에 따라 분기
	{
		case 1: //패배시
			printf("\7"); //Beep
			printf("\7"); //Beep : 비프음 2회 출력

			gameOverScreen(); // 게임 오버 화면 호출후 탈출

			break;
		case 2: //승리시
			youWinScreen(); // 승리 화면 호출후 탈출
			break;
	}
	
	if(score >= getLowestScore() && score != 0) //스코어 값이 존재하고 기존 랭킹의 최 하위값보다 스코어가 높은 경우 (= 순위권에 들어온 경우)
	{
		inputScore(score); // 랭킹 데이터에 현재 스코어 기록.
		displayHighScores(); // 랭킹 출력.
	}
	
	return;
}

void loadSnake(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
	int i;
	/*
	First off, The snake doesn't actually have enough XY coordinates (only 1 - the starting location), thus we use
	these XY coordinates to "create" the other coordinates. For this we can actually use the function used to move the snake.
	This helps create a "whole" snake instead of one "dot", when someone starts a game.
	*/
	//moveSnakeArray(snakeXY, snakeLength); //One thing to note ATM, the snake starts of one coordinate to whatever direction it's pointing...
	
	//This should print out a snake :P
	for (i = 0; i < snakeLength; i++)
	{
		gotoxy(snakeXY[0][i], snakeXY[1][i]);
		printf("%c", SNAKE_BODY); //Meh, at some point I should make it so the snake starts off with a head...
	}
	
	return;
}

/* NOTE, This function will only work if the snakes starting direction is left!!!! 
Well it will work, but the results wont be the ones expected.. I need to fix this at some point.. */
void prepairSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
	int i, x;
	int snakeX = snakeXY[0][0];
	int snakeY = snakeXY[1][0];
	
	// this is used in the function move.. should maybe create a function for it...
	/*switch(direction)
	{
		case DOWN_ARROW:
			snakeXY[1][0]++;
			break;
		case RIGHT_ARROW:
			snakeXY[0][0]++;
			break;
		case UP_ARROW:
			snakeXY[1][0]--;
			break;
		case LEFT_ARROW:
			snakeXY[0][0]--;
			break;			
	}
	*/
	
	
	for(i = 1; i <= snakeLength; i++)
	{
		snakeXY[0][i] = snakeX + i;
		snakeXY[1][i] = snakeY;
	}
	
	return;
}

//This function loads the enviroment, snake, etc
void loadGame(void)
{
	int snakeXY[2][SNAKE_ARRAY_SIZE]; //Two Dimentional Array, the first array is for the X coordinates and the second array for the Y coordinates
	
	int snakeLength = 4; //Starting Length
	
	int direction = LEFT_ARROW; //DO NOT CHANGE THIS TO RIGHT ARROW, THE GAME WILL INSTANTLY BE OVER IF YOU DO!!! <- Unless the prepairSnakeArray function is changed to take into account the direction....
	
	int foodXY[] = {5,5};// Stores the location of the food
	
	int score = 0;
	//int level = 1;
	
	//Window Width * Height - at some point find a way to get the actual dimensions of the console... <- Also somethings that display dont take this dimentions into account.. need to fix this...
	int consoleWidth = 80;
	int consoleHeight = 25;
	
	int speed = getGameSpeed();
	
	//The starting location of the snake
	snakeXY[0][0] = 40; 
	snakeXY[1][0] = 10;
	
	loadEnviroment(consoleWidth, consoleHeight); //borders
	prepairSnakeArray(snakeXY, snakeLength);
	loadSnake(snakeXY, snakeLength);
	generateFood( foodXY, consoleWidth, consoleHeight, snakeXY, snakeLength);
	refreshInfoBar(score, speed); //Bottom info bar. Score, Level etc
	startGame(snakeXY, foodXY, consoleWidth, consoleHeight, snakeLength, direction, score, speed);

	return;
}

//**************MENU STUFF**************//

int menuSelector(int x, int y, int yStart)
{
	char key;
	int i = 0;
	x = x - 2;
	gotoxy(x,yStart);
	
	printf(">");
	
	gotoxy(1,1);

	
	do
	{
		key = waitForAnyKey();
		//printf("%c %d", key, (int)key);
		if ( key == (char)UP_ARROW )
		{
			gotoxy(x,yStart+i);
			printf(" ");
			
			if (yStart >= yStart+i ) 
				i = y - yStart - 2;
			else
				i--;
			gotoxy(x,yStart+i);
			printf(">");
		}
		else
			if ( key == (char)DOWN_ARROW )
			{
				gotoxy(x,yStart+i);
				printf(" ");
				
				if (i+2 >= y - yStart ) 
					i = 0;
				else
					i++;
				gotoxy(x,yStart+i);
				printf(">");				
			}	
			//gotoxy(1,1);
			//printf("%d", key);
	} while(key != (char)ENTER_KEY); //While doesn't equal enter... (13 ASCII code for enter) - note ubuntu is 10
	return(i);
}

void welcomeArt(void)
{
	clrscr(); //clear the console
	//Ascii art reference: http://www.chris.com/ascii/index.php?art=animals/reptiles/snakes
	printf("\n");	
	printf("\t\t    _________         _________ 			\n");	
	printf("\t\t   /         \\       /         \\ 			\n");	
	printf("\t\t  /  /~~~~~\\  \\     /  /~~~~~\\  \\ 			\n");	
	printf("\t\t  |  |     |  |     |  |     |  | 			\n");		
	printf("\t\t  |  |     |  |     |  |     |  | 			\n");
	printf("\t\t  |  |     |  |     |  |     |  |         /	\n");
	printf("\t\t  |  |     |  |     |  |     |  |       //	\n");
	printf("\t\t (o  o)    \\  \\_____/  /     \\  \\_____/ / 	\n");
	printf("\t\t  \\__/      \\         /       \\        / 	\n");
	printf("\t\t    |        ~~~~~~~~~         ~~~~~~~~ 		\n");
	printf("\t\t    ^											\n");
	printf("\t		Welcome To The Snake Game!			\n");
	printf("\t			    Press Any Key To Continue...	\n");			
	printf("\n");
	
	waitForAnyKey();
	return;
}

void controls(void)
{
	int x = 10, y = 5;
	clrscr(); //clear the console
	gotoxy(x,y++);
	printf("Controls\n");
	gotoxy(x++,y++);
	printf("Use the following arrow keys to direct the snake to the food: ");
	gotoxy(x,y++);
	printf("Right Arrow");
	gotoxy(x,y++);
	printf("Left Arrow");
	gotoxy(x,y++);
	printf("Top Arrow");
	gotoxy(x,y++);
	printf("Bottom Arrow");
	gotoxy(x,y++);
	gotoxy(x,y++);
	printf("P & Esc pauses the game.");
	gotoxy(x,y++);
	gotoxy(x,y++);
	printf("Press any key to continue...");
	waitForAnyKey();
	return;
}

/**
* 종료 화면을 출력하고, 사용자로부터 입력을 받은 값이 y라면 게임을 종료.
**/
void exitYN(void)
{
	char pressed; // 사용자의 입력값을 담을 변수. 한 글자로 char 타입. 
	gotoxy(9,8); // 메시지 출력을 위해 (9,8) 위치로 좌표 이동.
	printf("Are you sure you want to exit(Y/N)\n"); //종료 여부 확인 메시지.
	
	do //무조건 한번은 실행
	{
		pressed = waitForAnyKey(); // waitForAnyKey() Func로부터 키 입력받아 pressed에.
		pressed = tolower(pressed); // 대문자일 경우 전부 소문자로 변환.
	} while (!(pressed == 'y' || pressed == 'n')); // y또는 n값이 아닌 경우 계속 반복.
	
	if (pressed == 'y') // y값인 경우 (=사용자가 종료 선택)
	{
		clrscr(); //지금까지 뿌린 화면 전부 삭제후.
		exit(1); //프로그램 종료. (code 1 : 정상 종료)
	}
	return; //n인 경우 : 함수 탈출, 기존 작업 계속 수행.
}

int mainMenu(void)
{
	int x = 10, y = 5;
	int yStart = y;
	
	int selected;
	
	clrscr(); //clear the console
	//Might be better with arrays of strings???
	gotoxy(x,y++);
	printf("New Game\n");
	gotoxy(x,y++);
	printf("High Scores\n");
	gotoxy(x,y++);
	printf("Controls\n");
	gotoxy(x,y++);
	printf("Exit\n");
	gotoxy(x,y++);

	selected = menuSelector(x, y, yStart);

	return(selected);
}

//**************END MENU STUFF**************//

int main() //Need to fix this up
{

	welcomeArt();
	
	do
	{	
		switch(mainMenu())
		{
			case 0:
				loadGame();
				break;
			case 1:
				displayHighScores();
				break;	
			case 2:
				controls();
				break;		
			case 3:
				exitYN(); 
				break;			
		}		
	} while(1);	//
	
	return(0);
}
