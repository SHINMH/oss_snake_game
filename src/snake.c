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
	
	const char SNAKE_HEAD = (char)177;
	const char SNAKE_BODY = (char)178;
	const char WALL = (char)219;	
	const char FOOD = (char)254;
	const char BLANK = ' ';


	/**
	* Ŀ���� ��ġ�� ȭ�� ���ʻ���� �������� x,y ��ŭ �̵�
	* 
	* @param int x : x��ǥ
	* @param int y : y��ǥ
	**/
	void gotoxy(int x,int y)
	{
		COORD Pos;
		Pos.X = (short)x;
		Pos.Y = (short)y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
		//Ŀ������ �̵���Ű�� �Լ�
		//getStdHandle() �Ķ���ͷ� �Ѱ��� ���� �ڵ鰪�� ��ȯ���ش�.
	}
	
	/**
	* �ܼ�ȭ���� ������ ��� �����.
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


/**
* Ű���� �Է��� ��ٸ��� �Էµ� ���� ��ȯ�Ѵ�.
*
* @return char : Ű����� �Էµ� ��
**/
char waitForAnyKey(void)
{
	int pressed;// Ű������ �Է� ���� ��Ÿ���� ����
	
	while(!_kbhit());
	// _kbhit()�Լ��� Ű���� �Է��� ���� 1, ���� 0�� ��ȯ�Ѵ�.
	// Ű���� �Է��� ���������� ������ �ݺ��� �Է��� ��ٸ���.

	pressed = _getch();//�Էµ� ������ pressed �ʱ�ȭ
	
	return((char)pressed);
}

/**
* ���� �ӵ��� �����ϴ� ȭ�� ǥ�ÿ� ���� �ӵ��� �Է� �޾� ��ȯ�Ѵ�.
*
* @return int : ���� �̵��ӵ�
**/
int getGameSpeed(void)
{
	int speed; //���� �̵��ӵ��� ���ϴ� ����
	clrscr(); //ȭ�� Ŭ����
	
	do
	{
		gotoxy(10,5); //Ŀ�� �̵��� ���� ���
		printf("Select The game speed between 1 and 9.");
		speed = waitForAnyKey()-48;
		//waitForAnyKey()�Լ��� ��ȯ���� char���̶� 48�� ���� ���ڿ� �ش���
	} while(speed < 1 || speed > 9);
	//���� �̵��ӵ� speed������ ���� 1~9�������� �Ǻ��ϰ� �ƴҽ� �ݺ� 
	return(speed);
}

/**
* �Է��� ���� ������ ������ pause ��ų �� ���Ǵ� �Լ��̴�.
* �ܼ��� �ϳ��� �Է��� ���ö����� ������ �ݺ����� ���鼭 
* ������ �m�ߴ� ���� ȿ���� �ش�.
**/
void pauseMenu(void)
{
	gotoxy(28,23);
	printf("**Paused**");
	
	waitForAnyKey();
	gotoxy(28,23);
	printf("            ");

	return;
}


/**
* ����Ű�� ecs,p Ű�� ���ȳ� �˻��ϰ� Ű�� �ش��ϴ� int���� ��ȯ�ϴ� �Լ��̴�.
* 
* @param int direction : ���� ���� �̵�����
* @return int : ���� �̵������� ���ϴ� Ű����κ��� �Էµ� ��
**/
int checkKeysPressed(int direction)
{
	int pressed;
	
	if(_kbhit()) //Ű���忡 �Է��� �־������� �˻���.
	{
		pressed=_getch();
		//getch�� �Է� ���۸� ������� �ʾ� ȭ���� ������ ����.
		if (direction != pressed) //�Է��� ������ ���� �̵� ����� �������� �˻�
		{
			//�� ���� �������� �Է� ������ �ݴ밡 �ƴ��� �˻���(�ݴ�� �ٷ� ����ٲٴ� ���� ��������)
			//�ݴ������ �ƴ϶�� �Է��� pressed�� �����ϰ� ������.
			//esc, p �� ��쿡�� pauseMenu()�Լ��� ȣ���Ͽ� ȭ���� ����
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


/**
*�Ķ���ͷ� �Ѿ�� x,y��ǥ�� �ش��ϴ� ���� ���� ���븸 Ȥ�� �Ӹ�, ���� ��ο� �浿�ϴ��� �˻��ϴ� �Լ��̴�. 
*
* @param int x : x��ǥ
* @param int y : y��ǥ
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : y��ǥ
* @param int snakeLength : ���� ����
* @param int detect : 0�� ��� ���� �Ӹ�, ���� ��ü�� ���� �浹�� �˻���, 
*					  1�� ��� ���� ����� �浹�� �˻��� 
* @return 0 : ���� �Ӹ� Ȥ�� ����� �浹���� ���� ,1 : ���� �Ӹ� Ȥ�� ����� �浹��
**/
int collisionSnake (int x, int y, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int detect)
{
	int i;
	for (i = detect; i < snakeLength; i++) //detech�� 0�̸� �Ӹ����� �浹 �˻�, 1�̸� ���븸 �浹�˻�
	{
		if ( x == snakeXY[0][i] && y == snakeXY[1][i]) // �浹�Ͽ����� �˻�
			return(1);
	}
	return(0);
}

/**
* �Ķ���ͷ� �Ѿ�� x,y��ǥ�� �ش��ϴ� ���� ���� ���븸 Ȥ�� �Ӹ�, ���� ��ο� �浿�ϴ��� �˻��ϴ� �Լ��̴�.
*
* @param int foodXY[] : ������ x,y��ǥ�� ���� �迭 foodXY[0]:x��ǥ, foodXY[1]:y��ǥ 
* @param int width : ���� �� ����
* @param int height : ���� �� ����
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : �� �Ӹ�, ���� ��ġ �迭
* @param int snakeLength : ���� ����
* @todo : ���ϰ��� ������� ����. void������ ���� ���
* @todo : �ٸ� �Լ����� �Ѿ�� ���� consolewidth, consoleheight���� �޴µ�, ���⼭�� width,height���� ���� ������
**/
int generateFood(int foodXY[], int width, int height, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{	
	do
	{
		srand ( (unsigned int)time(NULL) );
		foodXY[0] = rand() % (width-2) + 2;
		srand ( (unsigned int)time(NULL) );
		foodXY[1] = rand() % (height-6) + 2;
	}  while(collisionSnake(foodXY[0], foodXY[1], snakeXY, snakeLength, 0));
	//�� �Ӹ�,���� ���� ���̰� ������ ��, ���̸� ���������� �Ǳ� ������ ���� ��ġ�� �ٸ� ��ġ�� ���̰� �����ɶ����� �ݺ��Ѵ�. 
	
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



/**
* �� �Ӹ��� �� Ȥ�� ���뿡 �浹�ߴ��� �˻��Ͽ� ������� ��ȯ�Ѵ�.
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : �� �Ӹ�, ���� ��ġ �迭
* @param int consoleWidth : ���� �� ����
* @param int consoleHeight : ���� �� ����
* @param int snakeLength : ���� ����
* @return 0 : �� �Ӹ��� �� Ȥ�� ���뿡 �浹���� ����, 1 : ���� �Ӹ��� �� Ȥ�� ����� �浹��
**/
int collisionDetection(int snakeXY[][SNAKE_ARRAY_SIZE], int consoleWidth, int consoleHeight, int snakeLength )
{
	int colision = 0; //�浹�ߴ� ���� ��Ÿ���� ���� collision 0(�浹���� ����)���� �ʱ�ȭ

	//�� �Ӹ��� �� Ȥ�� ����� �浹�ߴ��� �˻�, �ε�ġ�� colision�� 1�� �ʱ�ȭ
	if ((snakeXY[0][0] == 1) || (snakeXY[1][0] == 1) || (snakeXY[0][0] == consoleWidth) || (snakeXY[1][0] == consoleHeight - 4))
		colision = 1;
	else
		if (collisionSnake(snakeXY[0][0], snakeXY[1][0], snakeXY, snakeLength, 1))
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
	if(fp == NULL)   //������ �������� ���� ���
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
		lowestScore = lowestScore + ((int)str[2+i] - 48) * (int)pow(10,(double)intLength-i-1);
	}

	return(lowestScore);	 //��� �߿��� ���� ���� ��ȯ
}

/**
 * ���� ��ŷ ������(�ְ� ������) ���� �ʱ�ȭ �Լ�
 **/
void resetRankData()
{
	FILE *fp;
	int i;
	char* highScoreName = (char*)malloc(sizeof(char) * 6);
	strcpy_s(highScoreName, sizeof(highScoreName), "EMPTY");
	
	clrscr();    //ȭ�� �ʱ�ȭ
	
	char pressed; // ������� �Է°��� ���� ����. �� ���ڷ� char Ÿ��. 
	gotoxy(9,8); // �޽��� ����� ���� (9,8) ��ġ�� ��ǥ �̵�.
	printf("Are you sure you want to reset rank data?(Y/N)\n"); //���� ���� Ȯ�� �޽���.
	
	pressed = waitForAnyKey(); // waitForAnyKey() Func�κ��� Ű �Է¹޾� pressed��.
	pressed = (char)tolower(pressed); // �빮���� ��� ���� �ҹ��ڷ� ��ȯ.
		
	clrscr();    //ȭ�� �ʱ�ȭ
	gotoxy(9,8); 
	if (pressed == 'y') // y���� ��� (=����ڰ� ���� ����) {
		fopen_s(&fp, "highscores.txt", "w+");	//highscore.txt�� ���� ���� ����

		for (i = 0; i < 5; i++)
		{
			//���Ͽ� ��� �Է�
			if (!fp == '0')	//highscores.txt ���� Ȯ��
				fprintf(fp, "%d\t%d\t\t\t%s\n", i + 1, 0, highScoreName);
		}

		if (!fp == '0')	 //highscores.txt ���� Ȯ��
			fclose(fp);	 //highscores.txt ����
			
		printf("DELETED!!!");
	} else {
		printf("CANCELED!!!");
	}

	return;
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
		    fScore = fScore + ((int)str[2+i] - 48) * (int)pow(10,(double)intLength-i-1);
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

/**
* ���� ���� �� ȭ���� ����� ���ӹ����� ����, �¿� ���� ����Ѵ�.
*
* @param int consoleWidth : ���� �� ����
* @param int consoleHeight : ���� �� ����
**/
void loadEnviroment(int consoleWidth, int consoleHeight)
{
	int x = 1, y = 1;
	int rectangleHeight = consoleHeight - 4; //y�� ���� 1~20�� ������
	clrscr(); //ȭ���� ����
	
	gotoxy(x,y);
	
	//���� ������ �� ���
	for (; y < rectangleHeight; y++)
	{
		gotoxy(x, y); //���ʺ� ���ۺκ����� Ŀ���̵�
		printf("%c",WALL);
		
		gotoxy(consoleWidth, y); //�����ʺ� ���ۺκ����� Ŀ���̵�
		printf("%c",WALL);
	}
	
	y = 1;
	//��� �ϴ� �� ���
	for (; x < consoleWidth+1; x++)
	{
		gotoxy(x, y); //��ܺ� ���ۺκ����� Ŀ���̵�
		printf("%c",WALL);
		
		gotoxy(x, rectangleHeight); //�ϴܺ� ���ۺκ����� Ŀ���̵�
		printf("%c",WALL);
	}
	
	return;
}


/**
* ���ӽ��� �� ���� ȭ�鿡 �׸���.
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : �� �Ӹ�, ���� ��ġ �迭
* @param int snakeLength : ���� ����
**/
void loadSnake(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
	int i;
	
	//���ǸӸ����� Ŀ���� �̵��ذ��� ȭ�鿡 ���
	for (i = 0; i < snakeLength; i++){
		gotoxy(snakeXY[0][i], snakeXY[1][i]);
		printf("%c", SNAKE_BODY);
	}
	
	return;
}


/**
* �� ���� ��ġ�� �ʱ�ȭ�Ѵ�.
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : �� �Ӹ�, ���� ��ġ �迭
* @param int snakeLength : ���� ����
**/
void prepairSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
	int i;
	int snakeX = snakeXY[0][0];
	int snakeY = snakeXY[1][0];
	
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
		pressed = (char)tolower(pressed); // �빮���� ��� ���� �ҹ��ڷ� ��ȯ.
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
