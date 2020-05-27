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
gcc snake.c �lm �o snake.out

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
���� �̵���Ų �� ���� ����κа� ���� �Ӹ� ������ �缳���ϴ� �Լ�.
���� ��Ÿ�������� �迭, ���� ����, �̵������� �Ķ���ͷ� �޴´�.
�̵� �Ŀ� ���� ����κ� �迭�� ��迭�ϰ�, �̵����⿡ ���� ���� �Ӹ� ������ �����Ѵ�.
*/
void moveSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
	int i;
	
	//���� �迭�� �� �ε����� ���� ��ĭ�� �ڷ� �о� ���� ����κ��� �̵��� ���� �����Ѵ�.
	for( i = snakeLength-1; i >= 1; i-- )
	{
		snakeXY[0][i] = snakeXY[0][i-1]; //���� ��, �Ʒ��� ǥ���ϴ� �迭
		snakeXY[1][i] = snakeXY[1][i-1]; //���� ����, �������� ǥ���ϴ� �迭
	}	
	
	//�Ķ���ͷ� ���� ���⿡ ���� ���� �Ӹ� ������ �����Ѵ�.
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
* ���̿� ����������, ���̸� ���� �� �ִ��� Ȯ���ϰ� �����ϴٸ� �Դ� �Լ�.
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : Snake�� ��ǥ�� ����
* @param int foodXY[] : ������ ��ǥ
* @return 0: ���̰� �������� ����, 1: ���������� ���̸� ���� ���.
**/
int eatFood(int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[]) 
{
	if (snakeXY[0][0] == foodXY[0] && snakeXY[1][0] == foodXY[1]) //Snake head�� x, y ��ǥ�� ������ ��ġ�� ��ġ�� ���.
	{
		foodXY[0] = 0;
		foodXY[1] = 0; // fixme: ���� �ݺ��Ǵ� ���װ� �ִ� ������ �˷���.  ������ ���װ� �߻��ϴ��� Ȯ�� �ʿ�.
		
		printf("\7"); //������
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
* ȭ�� �ϴ��� ���� �ٸ� ����.
*
* @param int score : �÷��̾��� ���� ����
* @param int speed : ���� ������ �ӵ� (�ܰ�)
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
* �÷��̾ ���ӿ��� �̰��� ��� 'YOU WIN'
**/
void youWinScreen(void)
{
	int x = 6, y = 7; // �޽��� ���� ��ǥ ���� (6,7)
	gotoxy(x,y++); // x, y ��ǥ���� ���� �̵�, ���� �������̹Ƿ� (6, 7)�� �̵��� y���� 8�� ������.
	printf("'##:::'##::'#######::'##::::'##::::'##:::::'##:'####:'##::: ##:'####:");
	gotoxy(x,y++); // x ���� ������ ���·� �� �� �Ʒ��� �̵�... (���� �ݺ�)
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
	
	waitForAnyKey(); //����� Ű �Է��� ���������� ���.
	clrscr(); //clear the console
	return;
}

/**
* ���� �����Ǿ��� �� 'GAME OVER' �޽����� ascii art ���·� ���.
**/
void gameOverScreen(void)
{
	int x = 17, y = 3; // �޽��� ���� ��ǥ ���� (17,3)
	
	//http://www.network-science.de/ascii/ <- Ascii Art Gen : �ƽ�Ű ��Ʈ ���� ����Ʈ URL (������ �ּ�)
	
	gotoxy(x,y++); // x, y ��ǥ���� ���� �̵�, ���� �������̹Ƿ� (17, 3)���� �̵��� y���� 8�� ������.
	printf(":'######::::::'###::::'##::::'##:'########:\n");
	gotoxy(x,y++); // x ���� ������ ���·� �� �� �Ʒ��� �̵�... (���� �ݺ�)
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
	
	waitForAnyKey(); //����� Ű �Է��� ���������� ���.
	clrscr(); //ȭ���� �����
	return;
}

/**
* ���� ���۽� ȣ��Ǵ� ������ ���� ����.
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : �÷��̾��� X, Y ��ǥ��.
* @param int foodXY[][] : ������ X, Y ��ǥ��.
* @param int consoleWitdh : â�� ����
* @param int consoleHeight : â�� ����
* @param int snakeLength : snake�� �ʱ� ����
* @param int direction : �ʱ� ���� ����
* @param int score : �ʱ� ������
* @param int speed : �ʱ� ������ �ӵ� (�ܰ�)
**/
//Todo /maybe-later: �ڵ� ���� �ʿ�.
void startGame( int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[], int consoleWidth, int consoleHeight, int snakeLength, int direction, int score, int speed)
{
	int gameOver = 0; // gameOver ���� üũ ����. (0: ���� ����, 1: ���� ����, 2: �¸�)
	clock_t endWait; // ��� ���� �ð��� ���� ����.
	
	//CLOCKS_PER_SEC-(n-1)*(CLOCKS_PER_SEC/10)
	int waitMili = CLOCKS_PER_SEC-(speed)*(CLOCKS_PER_SEC/10);	// ���� ���� �ӵ��� �´� ��� �ð� ���� (��� �ð� : 1�� - ���Ӽӵ�(�ܰ�) * 0.1��)
	int tempScore = 10*speed; // �ӵ� ���� �������� ���� ���ھ�� ���� ���ذ��� ���� �ӽ� ����. �ʱⰪ : 10 * �ӵ�.
	int oldDirection = 0; // ���� ���Ⱚ�� �����ϱ� ���� ����
	int canChangeDirection = 1; // ���� ��ȯ�� ������ �������� ���� (0: �Ұ���, 1: ����)
	//int seconds = 1;

	endWait = clock() + waitMili; // ���� �ð� + ��� �ð��� ��� ���� �ð����� ����.

	do
	{
		if(canChangeDirection) //���� ��ȯ�� ������ ������ ���.
		{
			oldDirection = direction; // ���� ������ oldDirection�� ����.
			direction = checkKeysPressed(direction); //�� ���Ⱚ�� �޾� ����.
		}
		
		if(oldDirection != direction) // ���� ����� �ٸ� ���
			canChangeDirection = 0; //���� ��ȯ�� �Ұ����ϵ��� ����. (snake�� ������ �浹�ϴ� ���� ����)
			
		if(clock() >= endWait) // ��� ���� �ð��� ���� ���. (��ǻ�� �ӵ��� ���� ����.)
		{
			//gotoxy(1,1);
			//printf("%d - %d",clock() , endWait);
			move(snakeXY, snakeLength, direction); // Snake�� ������ �������� �̵�.
			canChangeDirection = 1;  //�ٽ� ���� ��ȯ�� �����ϵ���.

				
			if(eatFood(snakeXY, foodXY)) // ���� ��ǥ�� ���̰� �����Ѵٸ�
			{
				generateFood( foodXY, consoleWidth, consoleHeight, snakeXY, snakeLength); //���ο� ���� ����.
				snakeLength++; //Snake�� ���� ����.
				score+=speed; //���� �ӵ���ŭ ���� �ο�.
				//x++;
				//gotoxy(1,1);
				//printf("%d >= %d", 10*speed+score, tempScore);
				if( score >= 10*speed+tempScore) // ���� ������ ���� �ӵ� * 10 + ���ذ��� �Ǵ� ���� �ܰ� ���ھ�� ū ��� ���� �ӵ� ���� ó��.
				//if( 2 >= 2)
				{
					speed++; // ���� �ӵ� ����.
					tempScore = score; // �Ǵ� ���ذ��� ���� ���ھ�� ����.

					if(speed <= 9)//this needs to be fixed // ���� �ӵ��� 9 ������ ���
						waitMili = waitMili - (CLOCKS_PER_SEC/10); // ��� �ð� ���� : ���� ���ð� - 1/10��
					else // ���� �ӵ��� 9���� ū ���
					{
						if(waitMili >= 40) // ���� ��� �ð��� ���� Ŭ�� �ӵ� ���� 40 �̻��� ��쿡�� �ӵ� ����. (�׺��� �� ���� ��� ���������� ���� ���� �Ұ��� �ӵ��̹Ƿ�.)
							waitMili = waitMili - (CLOCKS_PER_SEC/200); // ���� ���ð� - 1/200��. (�ӵ� ������ ����)
						
					}
					//level++;
					//gotoxy(1,2);
					//printf("    ");
					//gotoxy(1,2);
					//printf("%d",waitMili);
					//x = 0;
				}
				
				refreshInfoBar(score, speed); // �ϴ� �� ����.
			}
			
			// HACK : clock() ���� ��� ã�Ƽ� ���� �ʿ�. (����� �����ϴ� ������ ����)
			endWait = clock() + waitMili; //���� �ð� �������� ��� ���� �ð� ����. 
		}
		
		gameOver = collisionDetection(snakeXY, consoleWidth, consoleHeight, snakeLength); // ���� ���� ���� üũ�� ���� �ڵ� ���� (Snake�� â ���� ��Ұų� Snake���� ���� ��� 1 ���� = ���� ����)

		if(snakeLength >= SNAKE_ARRAY_SIZE-5) // Snake�� ���̰� (������ �ִ� ���� - 5) �̻��� ��� ���� �¸�. (�迭 ũ�⺸�� ����� ��� ũ���� �߻��ϹǷ�.)
		{
			gameOver = 2;//You Win! <- doesn't seem to work - NEED TO FIX/TEST THIS : gameOver�� �¸��ø� ���ϴ� �ڵ� 2 ����. (while�� Ż�� ����) => �������� �ʴ´���.
			score+=1500; //�¸���, ���ʽ� ����Ʈ 1500�� ����.
		}
		
	} while (!gameOver); //gameOver���� 0�� �ƴҶ����� loop.
	
	switch(gameOver) //gameOver ���� ���� �б�
	{
		case 1: //�й��
			printf("\7"); //Beep
			printf("\7"); //Beep : ������ 2ȸ ���

			gameOverScreen(); // ���� ���� ȭ�� ȣ���� Ż��

			break;
		case 2: //�¸���
			youWinScreen(); // �¸� ȭ�� ȣ���� Ż��
			break;
	}
	
	if(score >= getLowestScore() && score != 0) //���ھ� ���� �����ϰ� ���� ��ŷ�� �� ���������� ���ھ ���� ��� (= �����ǿ� ���� ���)
	{
		inputScore(score); // ��ŷ �����Ϳ� ���� ���ھ� ���.
		displayHighScores(); // ��ŷ ���.
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
* ���� ȭ���� ����ϰ�, ����ڷκ��� �Է��� ���� ���� y��� ������ ����.
**/
void exitYN(void)
{
	char pressed; // ������� �Է°��� ���� ����. �� ���ڷ� char Ÿ��. 
	gotoxy(9,8); // �޽��� ����� ���� (9,8) ��ġ�� ��ǥ �̵�.
	printf("Are you sure you want to exit(Y/N)\n"); //���� ���� Ȯ�� �޽���.
	
	do //������ �ѹ��� ����
	{
		pressed = waitForAnyKey(); // waitForAnyKey() Func�κ��� Ű �Է¹޾� pressed��.
		pressed = tolower(pressed); // �빮���� ��� ���� �ҹ��ڷ� ��ȯ.
	} while (!(pressed == 'y' || pressed == 'n')); // y�Ǵ� n���� �ƴ� ��� ��� �ݺ�.
	
	if (pressed == 'y') // y���� ��� (=����ڰ� ���� ����)
	{
		clrscr(); //���ݱ��� �Ѹ� ȭ�� ���� ������.
		exit(1); //���α׷� ����. (code 1 : ���� ����)
	}
	return; //n�� ��� : �Լ� Ż��, ���� �۾� ��� ����.
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
