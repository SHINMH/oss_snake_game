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

/**
* ���� �̵���Ų �� ���� ����κа� ���� �Ӹ� ������ �缳���ϴ� �Լ�.
* �̵� �Ŀ� ���� ����κ� �迭�� ��迭�ϰ�, �̵����⿡ ���� ���� �Ӹ� ������ ����
* 
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : ���� ��Ÿ�������� �Լ�
* @param int snakeLength : �� ����
* @param int direction : �̵� ����
**/
void moveSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
	int i;
	
	//���� �迭�� �� �ε����� ���� ��ĭ�� �ڷ� �о� ���� ����κ��� �̵��� ���� ����
	for( i = snakeLength-1; i >= 1; i-- )
	{
		snakeXY[0][i] = snakeXY[0][i-1]; //���� ��, �Ʒ��� ǥ���ϴ� �迭
		snakeXY[1][i] = snakeXY[1][i-1]; //���� ����, �������� ǥ���ϴ� �迭
	}	
	
	//�Ķ���ͷ� ���� ���⿡ ���� ���� �Ӹ� ������ ����
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
/**
* ����Ű �Է� �� ���� ����� ��Ÿ���� �Լ�.
* ���� �κ��� ���� �����ϰ�, �̵� ���� �Ӹ��κ��� �������� �ٲ�
* �� ��, moveSnakeArray �Լ��� �̿��� ����κ��� �ٲٰ�, ���� �Ӹ������� ����
* ���������� ���ο� �Ӹ��� ��ġ�� �Ӹ��� ���
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : ���� ��Ÿ�������� �迭
* @param int snakeLength : �� ����
* @param int direction : �̵�����
**/
void move(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
	//Ŀ�� ��ġ ��ǥ
	int x, y;

	//�̵� �� ��ġ�� �� ������ ��ġ�� ����
	x = snakeXY[0][snakeLength-1];
	y = snakeXY[1][snakeLength-1];
	
	//���� �κ��� ��ġ�� �̵��� ���� ����
	gotoxy(x,y);
	printf("%c",BLANK);	
	
	//�̵� �� ���� �Ӹ��κ����� �̵��� �� ��ġ�� �������� �ٲ�
	gotoxy(snakeXY[0][0],snakeXY[1][0]);	
	printf("%c", SNAKE_BODY);
	
	//�̵� �� ���� ����κ��� ��迭�ϰ�, ���� �Ӹ����� ����
	moveSnakeArray(snakeXY, snakeLength, direction);
	
	//���ο� �Ӹ� �κ����� �̵��� �Ӹ� ���
	gotoxy(snakeXY[0][0],snakeXY[1][0]);	
	printf("%c",SNAKE_HEAD);
	
	gotoxy(1,1); //(1,1)�� Ŀ�� �̵�
	
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

/**
 * �ְ����� ȭ�� ���
**/
void createHighScores(void)
{
	FILE *file; 
	int i;

	fopen_s(&file,"highscores.txt","w+"); //highscores.txt ������ ������. ������ �̹� ������ �� ������ ����� ����
	
	if(file == NULL)  // ������ �������� ���� ���
	{
		printf("FAILED TO CREATE HIGHSCORES!!! EXITING!");
		exit(0);    //���� �޽��� ��� �� ����
	}
	
	for(i = 0; i < 5; i++) // ���� �ְ� ������ ���
	{
		fprintf(file,"%d",i+1);
		fprintf(file,"%s","\t0\t\t\tEMPTY\n");
	}	
	
	fclose(file);	//������ ����
	return;
}
/**
 * ��ϵ� �ְ� ���� �� ���� ���� ������ �ҷ���
 *
 * @return lowestScore : ��� �߿��� ���� ����
 * */
int getLowestScore()
{
	FILE *fp;
	char* str = (char*)malloc(sizeof(char)*128);
	int lowestScore = 0;
	int i;
	int intLength;
	fopen_s(&fp,"highscores.txt", "r"); //highscores.txt�� �б� ���� ����
	rewind(fp);	//��ġ �����ڸ� �� ó������
	if((fp == NULL)   //������ �������� ���� ���
	{
		createHighScores();   // createHighScores()�Լ����� ���� ����
		if(fp == NULL)	 //������ �������� ������
			exit(1);   //����
	}

	while(!feof(fp))  //highscores.txt�� ������ ���� �������� ���� while��
	{
		fgets(str, sizeof(str), fp);   //str ������ fp�� ���� �� ���� ����
	}
	fclose(fp);  //���� �ݱ�
	
	i=0;
	
	while(str[2+i] != '\t')	 //���ڿ����� ������ �ڸ��� ���ϱ�
	{
		i++;
	}
	
	intLength = i;
	
	for(i=0;i < intLength; i++)  //���ڿ����� ������ ����
	{
		lowestScore = lowestScore + ((int)str[2+i] - 48) * (int)pow(10,intLength-i-1);
	}

	return(lowestScore);	 //��� �߿��� ���� ���� ��ȯ
}

/**
 * �ְ� ���� ��� �Լ�
 * ������ ������ �� ��� ������
 * ��ϵ� �ְ� ���� ��� ��
 * ���� ���� �������� ���� �� ȣ��
 *
 * @param score : ���� ���� ����
 **/
void inputScore(int score)
{
	FILE *fp;
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
	
	clrscr();    //ȭ�� �ʱ�ȭ
	
	fopen_s(&fp, "highscore.txt", "r");  //highscore.txt�� �б� ���� ����
	if(fp == NULL)	 //������ ������ ����
	    exit(1);
	gotoxy(10,5);	//Ŀ�� �̵�
	printf("Your Score made it into the top 5!!!");
	gotoxy(10,6);	//Ŀ�� �̵�
	printf("Please enter your name: ");
	gets_s(name,sizeof(name));  //��� �� name�� �Է� ����
	
	x = 0;
	while(!feof(fp))    //���� ������ ���� �� ���� �ݺ�
	{
		fgets(str, 126, fp);    //������ ���Ͽ��� ���پ� ������ ���ڿ� str�� ����
		
		i=0;
		
		//����� ������ �ڸ��� ���ϱ�
		while(str[2+i] != '\t')
		{
			i++;
		}
		
		s = i;
		intLength = i;
		i=0;
		while(str[5+s] != '\n')	 //���ڿ� str���� name�� �������� ���� �ݺ���
		{
			highScoreName[i] = str[5+s];
			s++;
			i++;
		}
				
		fScore = 0;
		for(i=0;i < intLength; i++) //���ڿ� str���� ������ �������� ���� �ݺ���
		{
		    fScore = fScore + ((int)str[2+i] - 48) * (int)pow(10,intLength-i-1);
		}
		
		if(score >= fScore && entered != 1) //���� �Ǿ� �ִ� �������� ���ο� ������ �� ũ�ų� �����鼭 ��ϵ� ���� ������
		{
			scores[x] = score;  //���ο� x��° �迭�� ����
			strcpy_s(highScoreNames[x], sizeof(highScoreNames[x]),name);	//�Էµ� �̸��� ��� x��° �迭�� ����
			
			x++; // 5������ ī��Ʈ �ϱ� ���� ���� ����
			entered = 1; //���ο� ���� ���忩�� flag
		}
		
		strcpy_s(highScoreNames[x],sizeof(highScoreNames[x]), highScoreName);	//���� ����� �̸��� ��� x��° �迭�� ����
		scores[x] = fScore;	//���� ����� x��° �迭�� ����
		
		for(y=0;y<20;y++)   //���ڿ� ���� �ʱ�ȭ
		{
			highScoreName[y] = '\0';
		}
		
		x++;	//5������ ī��Ʈ �ϱ� ���� ���� ����
		if(x >= 5)  //��� 5�� ��Ͻ� while�� ���
			break;
	}
	
	fclose(fp); //highscores.txt�� ����
	
	fopen_s(&fp, "highscores.txt", "w+");	//highscore.txt�� ���� ���� ����
	
	for(i=0;i<5;i++)
	{
		//���Ͽ� ��� �Է�
		if(!fp == '0')	//highscores.txt ���� Ȯ��
		    fprintf(fp, "%d\t%d\t\t\t%s\n", i+1, scores[i], highScoreNames[i]);	
	}

	if(!fp == '0')	 //highscores.txt ���� Ȯ��
	    fclose(fp);	 //highscores.txt ����
	
	return;
}
/**
 * �ְ� ���� ��� ȭ���� ����ϴ� �Լ� ȣ��
 * */

void displayHighScores(void) 
{
	FILE *fp;
	char* str = (char*)malloc(sizeof(char)*128);
	int y = 5;
	
	clrscr();  //ȭ�� �ʱ�ȭ
	fopen_s(&fp, "highscores.txt", "r"); //highscores.txt ������ �б� ���� ����

	if(fp == NULL) {    //������ ���� ��� ������ ���� ����
		createHighScores(); //�ְ� ���� ��� ȭ���� ����ϴ� �Լ� ȣ��
		if(!fopen_s(&fp, "highscores.txt", "r"))    //highscores.txt ������ ���µ� �����ϸ� ���α׷� ����
			exit(1);
	}
	
	gotoxy(10,y++);	//����� ���� Ŀ�� �̵�
	printf("High Scores");	
	gotoxy(10,y++);
	printf("Rank\tScore\t\t\tName");
	while(!feof(fp)) { //������ ���� ������ ������ ���� �� �� �� ����ϱ� ���� while��
		gotoxy(10,y++);
		if(fgets(str, 126, fp)) //highscores.txt ���� �� ���� ������
			printf("%s", str);
	}

	fclose(fp); 	//���� �ݱ�
	gotoxy(10,y++);
	
	printf("Press any key to continue...");
	waitForAnyKey();    //�ƹ� Ű�� �Է� �Ǳ� ��ٸ�
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
	int oldDirection // ���� ���Ⱚ�� �����ϱ� ���� ����
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


void loadEnviroment(int consoleWidth, int consoleHeight)//This can be done in a better way... FIX ME!!!! Also i think it doesn't work properly in ubuntu <- Fixed
{
	int i;
	int x = 1, y = 1;
	int rectangleHeight = consoleHeight - 4;
	clrscr(); //clear the console
	
	gotoxy(x,y); //Top left corner
	
	for (; y < rectangleHeight; y++)
	{
		gotoxy(x, y); //Left Wall 
		printf("%c",WALL);
		
		gotoxy(consoleWidth, y); //Right Wall
		printf("%c",WALL);
	}
	
	y = 1;
	for (; x < consoleWidth+1; x++)
	{
		gotoxy(x, y); //Left Wall 
		printf("%c",WALL);
		
		gotoxy(x, rectangleHeight); //Right Wall
		printf("%c",WALL);
	}
	
/*
	for (i = 0; i < 80; i++)
	{
		printf("%c",WALL);
	}
	
	for (i = 0; i < 17; i++)
	{
		printf("%c\n",WALL);
	}

	for (i = 0; i < 21; i++)
	{
		printf("%c\n",WALL);
		gotoxy(80,i);
	}
	
	for (i = 0; i < 81; i++)
	{
		printf("%c",WALL);
	}	
*/	
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

/**
* ���ӽ��� ����, ��� ȯ���� ������ ��, ���ӽ����Լ��� ȣ���ϴ� �Լ�.
* �� ����, �� ǥ��, ���� ����, �������� ��� �� ������ �����Ѵ�.
**/
void loadGame(void)
{
	int snakeXY[2][SNAKE_ARRAY_SIZE]; //���� ��Ÿ�������� �迭
	
	int snakeLength = 4; //�ʱ� �� ����
	
	int direction = LEFT_ARROW; //���� ���� �� �� �Ӹ��� ù ����
	
	int foodXY[] = {5,5}; //���� ��ġ ����
	
	int score = 0; //�ʱ� ����
	
	//�� �ʺ�, ���� ����	
	int consoleWidth = 80;
	int consoleHeight = 25;
	
	//���� ���ǵ� ����
	int speed = getGameSpeed();
	
	//�ʱ� ��Ӹ� ��ġ ����
	snakeXY[0][0] = 40; 
	snakeXY[1][0] = 10;
	
	loadEnviroment(consoleWidth, consoleHeight); //�� ����
	prepairSnakeArray(snakeXY, snakeLength); //���� ��Ÿ�������� �迭 ����
	loadSnake(snakeXY, snakeLength); //�� ���
	generateFood( foodXY, consoleWidth, consoleHeight, snakeXY, snakeLength); //���� ����
	refreshInfoBar(score, speed); //���� ���� ���
	startGame(snakeXY, foodXY, consoleWidth, consoleHeight, snakeLength, direction, score, speed); //�������� ���� ����

	return;
}

/**
* �޴� ���� ���� �Լ�.
* 
* @param int x : Ŀ�� x��ǥ
* @param int y : Ŀ�� y��ǥ
* @param int yStart : �ʱ� Ŀ�� y��ǥ
* @return i : �޴��� �Ҵ�� �� (i = 0, 1, 2, 3)
**/
int menuSelector(int x, int y, int yStart)
{
	char key; //Ű ���� ����
	int i = 0;
	x = x - 2; // ">"�� ��Ÿ���� ���� �޴��� ���ִ� ��ġ���� �������� ��ĭ �̵�
	//�ʱ� ">"�� ��ġ ���
	gotoxy(x,yStart);
	printf(">");
	
	//Ŀ�� ��ǥ (1,1) �̵�
	gotoxy(1,1);

	//����Ű ��, �Ʒ��� �Է��ϸ� �޴� ���� ����
	do
	{
		key = waitForAnyKey(); //�Է� ���

		if ( key == (char)UP_ARROW ) //�� ����Ű �Է�
		{
			gotoxy(x,yStart+i); //�̵������� ���� �ִ� ��ġ�� �ִ� ">"�� ����
			printf(" ");
			
			if (yStart >= yStart+i ) //���� �̵��� ��, ���� �����ִ� �޴��� ��ġ���� �� ���� �ö󰡸� ��ġ�� �������Ͽ� ���� �Ʒ��� ������
				i = y - yStart - 2;
			else //�ܼ��� ���� �ö󰡴� �� ����
				i--;
			gotoxy(x,yStart+i); //y��ǥ�� ����� �Ϳ� ���� ">" ���
			printf(">");
		}
		else
			if ( key == (char)DOWN_ARROW ) //�Ʒ� ����Ű �Է�
			{
				gotoxy(x,yStart+i); //�̵������� ���� �ִ� ��ġ�� �ִ� ">"�� ����
				printf(" ");
				
				if (i+2 >= y - yStart ) //�Ʒ��� �̵��� ��, ���� �Ʒ��� �ִ� �޴��� ��ġ���� �� �Ʒ��� �������� ��ġ�� �������Ͽ� ���� ���� ������
					i = 0;
				else //�ܼ��� �Ʒ��� �������� �� ����
					i++;
				gotoxy(x,yStart+i); //y��ǥ�� ����� �Ϳ� ���� ">" ���
				printf(">");				
			}	

	} while(key != (char)ENTER_KEY); //EnterŰ �Է� �� ����

	return(i);
}
/**
* �ʱ�ȭ�� ��� �Լ�.
* �ʱ�ȭ���� ����� ��, waitForAnyKey()�� �̿��� �Է´��
**/
void welcomeArt(void)
{
	clrscr(); //��üȭ�� ����

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
	
	waitForAnyKey(); //�Է´��
	return;
}
/**
* ���۹� ��� �Լ�.
* ���۹� ��� ��, waitForAnyKey()�� �̿��� �Է´��
**/
void controls(void)
{
	//Ŀ�� ��ǥ
	int x = 10, y = 5;
	clrscr(); //��üȭ�� ����

	//gotoxy()�� �̿��� Ŀ���� �̵��ϸ� ���۹� ���
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
	waitForAnyKey(); //�Է´��
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
/**
* ���θ޴� ȭ�� ��� ��, �޴��� ���� �Լ�.
*
* @return selected : �� �޴��� �Ҵ�� ��
**/
int mainMenu(void)
{
	int x = 10, y = 5; //x, y�� Ŀ���� �̵� ��ġ ����
	int yStart = y; //�ʱ⿡ Ŀ���� ��Ÿ�� ��ġ�� ���� ������ menuSelector()�� �Ķ���ͷ� �ʿ��� ��
	
	int selected; //�� �޴��� �Ҵ�� ��
	
	clrscr(); //��üȭ�� ����

	//gotoxy()�� �̿��� Ŀ�� �̵��ϸ� ���θ޴� ���
	gotoxy(x,y++);
	printf("New Game\n");
	gotoxy(x,y++);
	printf("High Scores\n");
	gotoxy(x,y++);
	printf("Controls\n");
	gotoxy(x,y++);
	printf("Exit\n");
	gotoxy(x,y++);
	
	//menuSelector()�� �̿��� �� �޴��� ���� ���� ��ȯ�Ͽ� selected�� ����
	selected = menuSelector(x, y, yStart);

	return(selected);
}

/**
* main�Լ�.
* �ʱ�ȭ�� ��� ��, mainMenu()�� �̿��� ��ȯ�Ǵ� ���� ���� ���� �Լ� ����
*
* @return 0
**/

int main() 
{

	welcomeArt(); //�ʱ�ȭ�� ���
	
	do
	{	
		switch(mainMenu()) //�� �޴��� ���� �� ��ȯ
		{
			case 0: //���� ����
				loadGame();
				break;
			case 1: //���� ��� Ȯ��
				displayHighScores();
				break;	
			case 2: //���۹� Ȯ��
				controls();
				break;		
			case 3: //�������� Ȯ��
				exitYN(); 
				break;			
		}		
	} while(1);
	
	return(0);
}
