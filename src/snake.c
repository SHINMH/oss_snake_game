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
gcc snake.c -lm -o snake.out

Cygwin:
Although this program may compile/ run in Cygwin it runs slowly.	
				
*/

#include <stdio.h>
#include "snake.h"

/**
* 키보드 입력을 기다리고 입력된 값을 반환한다.
*
* @return char : 키보드로 입력된 값
**/
char waitForAnyKey(void)
{
	int pressed;// 키보드의 입력 값을 나타내는 변수
	
	while(!_kbhit());
	// _kbhit()함수는 키보드 입력이 있음 1, 없음 0을 반환한다.
	// 키보드 입력이 있을때까지 무한히 반복해 입력을 기다린다.

	pressed = _getch();//입력된 값으로 pressed 초기화
	
	return((char)pressed);
}

/**
* 게임 시작 선택 후 게임의 난이도를 선택하는 화면.
* 게임 모드는 EASY, NORMAL, HARD 모드가 존재한다.
*
* @return int : 뱀의 이동속도
**/
int selectGameMode(void)
{
	int x = 10;
	int y = 3;
	int yStart = y+2;

	int selected;

	clrscr();
	gotoxy(x,y++);
	printf("Select The game mode!!!");
	gotoxy(x,y++);

	gotoxy(x,y++);
	printf("EASY\n");
	gotoxy(x,y++);
	printf("NORMAL\n");
	gotoxy(x, y++);
	printf("HARD\n");
	gotoxy(x,y++);

	selected = menuSelector(x,y,yStart);

	selected = (selected+1) * 3;
	//EASY -> 3, NOMAL -> 6, HARD -> 9
	
	return(selected);
}

/**
* 입력이 있을 때까지 게임을 pause 시킬 때 사용되는 함수이다.
* 단순히 하나의 입력이 들어올때까지 무한히 반복문이 돌면서 
* 게임이 멈추는 듯한 효과를 준다.
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
* 방향키와 ecs,p 키가 눌렸나 검사하고 키에 해당하는 int값을 반환하는 함수이다.
* 
* @param int direction : 뱀의 기존 이동방향
* @return int : 뱀의 이동방향을 뜻하는 키보드로부터 입력된 값
**/
int checkKeysPressed(int direction)
{
	int pressed;
	
	if(_kbhit()) //키보드에 입력이 있었는지를 검사함.
	{
		pressed=_getch();
		//getch는 입력 버퍼를 사용하지 않아 화면이 멈추지 않음.
		if (direction != pressed) //입력이 기존의 뱀의 이동 방향과 동일한지 검사
		{
			//각 뱀의 진행반향과 입력 방향이 반대가 아닌지 검사함(반대로 바로 방향바꾸는 것을 막기위해)
			//반대방향이 아니라면 입력을 pressed에 저장하고 리턴함.
			//esc, p 일 경우에는 pauseMenu()함수를 호출하여 화면을 멈춤
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
			else if (pressed == CUT_BUTTON)
				direction = pressed;
		}
	}
	return(direction);
}


/**
*파라미터로 넘어온 x,y좌표에 해당하는 값이 뱀의 몸통만 혹은 머리, 몸통 모두와 충동하는지 검사하는 함수이다. 
*
* @param int x : x좌표
* @param int y : y좌표
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : y좌표
* @param int snakeLength : 뱀의 길이
* @param int detect : 0일 경우 뱀의 머리, 몸통 전체에 대해 충돌을 검사함, 
*					  1일 경우 뱀의 몸통과 충돌을 검사함 
* @return 0 : 뱀의 머리 혹은 몸통과 충돌하지 않음 ,1 : 뱀의 머리 혹은 몸통과 충돌함
**/
int collisionSnake (int x, int y, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int detect)
{
	int i;
	for (i = detect; i < snakeLength; i++) //detech가 0이면 머리포함 충돌 검사, 1이면 몸통만 충돌검사
	{
		if ( x == snakeXY[0][i] && y == snakeXY[1][i]) // 충돌하였는지 검사
			return(1);
	}
	return(0);
}

/**
* 파라미터로 넘어온 x,y좌표에 해당하는 값이 뱀의 몸통만 혹은 머리, 몸통 모두와 충동하는지 검사하는 함수이다.
*
* @param int foodXY[] : 먹이의 x,y좌표를 가진 배열 foodXY[0]:x좌표, foodXY[1]:y좌표 
* @param int width : 게임 판 넓이
* @param int height : 게임 판 높이
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : 뱀 머리, 몸통 위치 배열
* @param int snakeLength : 뱀의 길이
* @todo : 리턴값을 사용하지 않음. void형으로 변경 요망
* @todo : 다른 함수들은 넘어온 값을 consolewidth, consoleheight으로 받는데, 여기서는 width,height으로 받음 변경요망
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
	//뱀 머리,몸통 위에 먹이가 생성될 시, 먹이를 먹은것으로 되기 때문에 뱀의 위치와 다른 위치에 먹이가 생성될때까지 반복한다. 
	
	gotoxy(foodXY[0] ,foodXY[1]);
	printf("%c", FOOD);
	
	return(0);
}

/**
* 뱀을 이동시킨 후 뱀의 몸통부분과 뱀의 머리 방향을 재설정하는 함수.
* 이동 후에 뱀의 몸통부분 배열을 재배열하고, 이동방향에 따라 뱀의 머리 방향을 설정
* 
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : 뱀을 나타내기위한 함수
* @param int snakeLength : 뱀 길이
* @param int direction : 이동 방향
**/
void moveSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
	int i;
	
	//뱀의 배열의 각 인덱스의 값을 한칸씩 뒤로 밀어 뱀의 몸통부분이 이동한 것을 구현
	for( i = snakeLength-1; i >= 1; i-- )
	{
		snakeXY[0][i] = snakeXY[0][i-1]; //뱀의 위, 아래를 표현하는 배열
		snakeXY[1][i] = snakeXY[1][i-1]; //뱀의 왼쪽, 오른쪽을 표현하는 배열
	}	
	
	//파라미터로 받은 방향에 따라 뱀의 머리 방향을 설정
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
* 커서 숨기는 함수.
* ConsoleCursor.bVisible로 커서를 나타내는 것을 컨트롤한다.
**/
void CursorView(char show)
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = show; //true : 보임, false : 안보임
	ConsoleCursor.dwSize = 1; //커서사이즈

	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
	return;
}

/**
* 방향키 입력 후 뱀의 모습을 나타내는 함수.
* 꼬리 부분을 먼저 삭제하고, 이동 전의 머리부분을 몸통으로 바꿈
* 그 후, moveSnakeArray 함수를 이용해 몸통부분을 바꾸고, 뱀의 머리방향을 설정
* 마지막으로 새로운 머리의 위치에 머리를 출력
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : 뱀을 나타내기위한 배열
* @param int snakeLength : 뱀 길이
* @param int direction : 이동방향
**/
void move(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
	//커서 위치 좌표
	int x, y;

	//이동 전 위치의 뱀 꼬리의 위치를 저장
	x = snakeXY[0][snakeLength-1];
	y = snakeXY[1][snakeLength-1];
	
	//꼬리 부분의 위치로 이동해 꼬리 삭제
	gotoxy(x,y);
	printf("%c",BLANK);	
	
	//이동 전 뱀의 머리부분으로 이동해 그 위치를 몸통으로 바꿈
	gotoxy(snakeXY[0][0],snakeXY[1][0]);	
	printf("%c", SNAKE_BODY);
	
	//이동 후 뱀의 몸통부분을 재배열하고, 뱀의 머리방향 설정
	moveSnakeArray(snakeXY, snakeLength, direction);
	
	//새로운 머리 부분으로 이동해 머리 출력
	gotoxy(snakeXY[0][0],snakeXY[1][0]);	
	printf("%c",SNAKE_HEAD);
	
	//gotoxy(1,1); //(1,1)로 커서 이동
	CursorView(0); //커서 숨김
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


/**
* 뱀 머리가 벽 충돌했는지 검사하여 결과값을 반환한다.
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : 뱀 머리, 몸통 위치 배열
* @param int consoleWidth : 게임 판 넓이
* @param int consoleHeight : 게임 판 높이
* @return 0 : 뱀 머리가 벽 혹은 몸통에 충돌하지 않음, 1 : 뱀의 머리가 벽 혹은 몸통과 충돌함
**/
int collisionWall(int snakeXY[][SNAKE_ARRAY_SIZE], int consoleWidth, int consoleHeight) {
	if ((snakeXY[0][0] == 1) || (snakeXY[1][0] == 1) || (snakeXY[0][0] == consoleWidth) || (snakeXY[1][0] == consoleHeight - 4)) {
		return 1;
	}
	return 0;
}

/**
* 뱀 머리가 벽 혹은 몸통에 충돌했는지 검사하여 결과값을 반환한다.
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : 뱀 머리, 몸통 위치 배열
* @param int consoleWidth : 게임 판 넓이
* @param int consoleHeight : 게임 판 높이
* @param int snakeLength : 뱀의 길이
* @return 0 : 뱀 머리가 벽 혹은 몸통에 충돌하지 않음, 1 : 뱀의 머리가 벽 혹은 몸통과 충돌함
**/
int collisionDetection(int snakeXY[][SNAKE_ARRAY_SIZE], int consoleWidth, int consoleHeight, int snakeLength )
{
	//뱀 머리가 벽 혹은 몸통과 충돌했는지 검사, 부딪치면 colision을 1로 초기화
	if (collisionWall(snakeXY, consoleWidth, consoleHeight)){
		return 1;
	}

	if (collisionSnake(snakeXY[0][0], snakeXY[1][0], snakeXY, snakeLength, 1)){
		return 1;
	}
		
	return 0;
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

/**
 * 최고점수 화면 출력
**/
void createHighScores(void)
{
	FILE *file; 
	int i;

	fopen_s(&file,"highscores.txt","w+"); //highscores.txt 파일을 생성함. 파일이 이미 있으면 그 파일을 지우고 생성
	
	if(file == NULL)  // 파일이 존재하지 않을 경우
	{
		printf("FAILED TO CREATE HIGHSCORES!!! EXITING!");
		exit(0);    //에러 메시지 출력 후 종료
	}
	
	for(i = 0; i < 5; i++) // 역대 최고 점수를 출력
	{
		fprintf(file,"%d",i+1);
		fprintf(file,"%s","\t0\t\t\tEMPTY\n");
	}	
	
	fclose(file);	//파일을 닫음
	return;
}
/**
 * 기록된 최고 점수 중 가장 낮은 점수를 불러옴
 *
 * @return lowestScore : 기록 중에서 최저 점수
 * */
int getLowestScore(void)
{
	FILE *fp;
	char* str = (char*)calloc(SIZE_STR, sizeof(char));
	int lowestScore = 0;
	int i;
	int intLength;
	fopen_s(&fp,"highscores.txt", "r"); //highscores.txt를 읽기 모드로 열기
	if(fp == NULL)   //파일이 존재하지 않을 경우
	{
		createHighScores();   // createHighScores()함수에서 파일 생성
		fopen_s(&fp, "highscores.txt", "r");
		if(fp == NULL)	 //파일이 존재하지 않으면
			exit(1);   //종료
	}
	rewind(fp);

	while(!feof(fp))  //highscores.txt의 마지막 줄을 가져오기 위한 while문
	{
		fgets(str, SIZE_STR, fp);   //str 변수에 fp의 문장 한 줄을 복사
	}
	fclose(fp);  //파일 닫기
	
	i=0;
	
	while(str[2+i] != '\t')	 //문자열에서 점수의 자릿수 구하기
	{
		i++;
	}
	
	intLength = i;
	
	for(i=0;i < intLength; i++)  //문자열에서 점수를 추출
	{
		lowestScore = lowestScore + ((int)str[2+i] - 48) * (int)pow(10,(double)intLength-i-1);
	}

	return(lowestScore);	 //기록 중에서 최저 점수 반환
}

/**
 * 기존 랭킹 데이터(최고 점수들) 파일 초기화 함수
 **/
void resetRankData(void)
{
	FILE *fp;
	int i;
	clrscr();    //화면 초기화

	char pressed; // 사용자의 입력값을 담을 변수. 한 글자로 char 타입. 
	gotoxy(9, 8); // 메시지 출력을 위해 (9,8) 위치로 좌표 이동.
	printf("Are you sure you want to reset rank data? (y/n)\n"); //종료 여부 확인 메시지.

	pressed = waitForAnyKey(); // waitForAnyKey() Func로부터 키 입력받아 pressed에.
	pressed = (char)tolower(pressed); // 대문자일 경우 전부 소문자로 변환.

	clrscr();    //화면 초기화
	gotoxy(9, 8);
	if (pressed == 'y') // y값인 경우 (=사용자가 삭제 선택) 
	{
		fopen_s(&fp, "highscores.txt", "w+");	//highscore.txt를 쓰기 모드로 열음
		
		if (fp == NULL)
		{
			printf("FAILED!!!");
			waitForAnyKey();
			return;
		}

		for (i = 0; i < 5; i++)
		{
			fprintf(fp, "%d\t0\t\t\tEMPTY\n", i + 1);
			printf("(%d of %d)... ", i + 1, 5);
		}

		fclose(fp);	 //highscores.txt 닫음

		printf("DELETED!!!");
		gotoxy(9, 9);
		printf("Press any key to continue...");
		waitForAnyKey();
	}
	else 
	{
		printf("CANCELED!!!");
		gotoxy(9, 9);
		printf("Press any key to continue...");
		waitForAnyKey();
	}

	return;
}

/**
 * 최고 점수 기록 함수
 * 게임이 끝났을 때 결과 점수가
 * 기록된 최고 점수 기록 중
 * 가장 낮은 점수보다 높을 때 호출
 *
 * @param score : 게임 종료 점수
 **/
void inputScore(int score)
{
	FILE *fp;
	char* str = (char*)calloc(SIZE_STR,sizeof(char));
	int fScore;
	int i, s, y;
	int intLength;
	int* scores = (int*)calloc(5,sizeof(int));
	int x;
	char* highScoreName = (char*)calloc(SIZE_NAME,sizeof(char));
	char highScoreNames[5][20];
	
	char* name = (char*)calloc(SIZE_NAME,sizeof(char));
	
	int entered = 0;
	
	clrscr();    //화면 초기화
	
	fopen_s(&fp, "highscores.txt", "r");  //highscore.txt를 읽기 모드로 열음
	if(fp == NULL)	 //파일이 없으면 종료
	    exit(1);
	gotoxy(10,5);	//커서 이동
	printf("Your Score made it into the top 5!!!");
	gotoxy(10,6);	//커서 이동
	printf("Please enter your name: ");
	gets_s(name,SIZE_NAME);  //기록 할 name을 입력 받음
	
	x = 0;
	while(!feof(fp))    //파일 내용이 끝날 때 까지 반복
	{
		fgets(str, SIZE_STR, fp);    //문장을 파일에서 한줄씩 가져와 문자열 str에 넣음
		
		i=0;
		
		//저장된 점수의 자릿수 구하기
		while(str[2+i] != '\t')
		{
			i++;
		}
		
		s = i;
		intLength = i;
		i=0;
		while(str[5+s] != '\n')	 //문자열 str에서 name을 가져오기 위한 반복문
		{
			highScoreName[i] = str[5+s];
			s++;
			i++;
		}
				
		fScore = 0;
		for(i=0;i < intLength; i++) //문자열 str에서 점수를 가져오기 위한 반복문
		{
		    fScore = fScore + ((int)str[2+i] - 48) * (int)pow(10,(double)intLength-i-1);
		}
		
		if(score >= fScore && entered != 1) //저장 되어 있는 점수보다 새로운 점수가 더 크거나 같으면서 등록된 적이 없을때
		{
			scores[x] = score;  //새로운 x번째 배열에 저장
			strcpy_s(highScoreNames[x], SIZE_NAME, name);	//입력된 이름을 기록 x번째 배열에 저장
			
			x++; // 5위까지 카운트 하기 위한 변수 증가
			entered = 1; //새로운 점를 저장여부 flag
		}
		
		strcpy_s(highScoreNames[x],SIZE_NAME, highScoreName);	//원래 기록의 이름을 기록 x번째 배열에 저장
		scores[x] = fScore;	//기존 기록을 x번째 배열에 저장
		
		for(y=0;y<20;y++)   //문자열 변수 초기화
		{
			highScoreName[y] = '\0';
		}
		
		x++;	//5위까지 카운트 하기 위한 변수 증가
		if(x >= 5)  //기록 5개 기록시 while문 벗어남
			break;
	}
	
	fclose(fp); //highscores.txt를 닫음
	
	fopen_s(&fp, "highscores.txt", "w+");	//highscore.txt를 쓰기 모드로 열음
	
	for(i=0;i<5;i++)
	{
		//파일에 기록 입력
		if(!fp == NULL)	//highscores.txt 존재 확인
		    fprintf(fp, "%d\t%d\t\t\t%s\n", i+1, scores[i], highScoreNames[i]);	
	}

	if(!fp == NULL)	 //highscores.txt 존재 확인
	    fclose(fp);	 //highscores.txt 닫음
	
	return;
}
/**
 * 최고 점수 기록 화면을 출력하는 함수 호출
 * */

void displayHighScores(void) 
{
	FILE *fp;
	char* str = (char*)calloc(SIZE_STR, sizeof(char));
	int y = 5;
	
	clrscr();  //화면 초기화
	fopen_s(&fp, "highscores.txt", "r"); //highscores.txt 파일을 읽기 모드로 열기

	if(fp == NULL) {    //파일이 없는 경우 파일을 새로 생성
		createHighScores(); //최고 점수 기록 화면을 출력하는 함수 호출
		fopen_s(&fp, "highscores.txt", "r");    //highscores.txt 파일을 여는데 실패하면 프로그램 종료
		if(fp == NULL)
			exit(1);
	}
	
	gotoxy(10,y++);	//출력을 위한 커서 이동
	printf("High Scores");	
	gotoxy(10,y++);
	printf("Rank\tScore\t\t\tName");
	while(!feof(fp)) { //파일이 끝날 때까지 파일의 내용 한 줄 씩 출력하기 위한 while문
		gotoxy(10,y++);
		if(fgets(str, SIZE_STR, fp)) //highscores.txt 파일 한 줄을 가져옴
			printf("%s", str);
	}

	fclose(fp); 	//파일 닫기
	gotoxy(10,y++);
	
	printf("Press any key to continue...");
	gotoxy(10, y++);

	printf("If you want to reset rank data, press 'c' key.");

	char pressed; // 사용자의 입력값을 담을 변수. 한 글자로 char 타입. 
	pressed = waitForAnyKey(); // waitForAnyKey() Func로부터 키 입력받아 pressed에.
	pressed = (char)tolower(pressed); // 대문자일 경우 전부 소문자로 변환.

	if (pressed == 'c') //c버튼 클릭시 초기화 출력
		resetRankData();

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
* 점수에 따라 게임 진행 속도를 빠르게 함.
*
* @param int score 게임 점수
* @param int speed 게임 속도
*
* return speed 변경된 게임 속도
**/
int setSpeed(int score, int speed) {
	if (score >= speed * 20) //점수가 속도*20 보다 크거나 같을 경우 속도 증가
		return ++speed;
	else
		return speed;
}
/**
* 점수에 따라 게임 진행 속도를 빠르게 함.
*
* @param int waitMili 대기시간 (게임 속도와 직결됨)
* @param int score 게임 점수
* @param int speed 게임 속도
*
* return waitMili 변경된 대기시간
**/
int setWaitMili(int waitMili, int score, int speed) {
	if (score >= speed * 20) { //점수가 속도*20 보다 크거나 같을 경우 대기시간 감소.
		if (speed <= 9)
			return waitMili -= (CLOCKS_PER_SEC / 10);
		else {
			if (waitMili >= 40) //대기시간이 40 이하일 경우 게임 진행이 사실상 힘들기 때문에 감소 폭을 줄임.
				waitMili -= (CLOCKS_PER_SEC / 200);
		}
	}
	else
		return waitMili;
}

/**
 * 'C'키를 눌렀을 때 뱀의 길이를 절반으로 줄여줌.
 *
 * @param int snakeXY[][SNAKE_ARRAY_SIZE] 플레이어의 X, Y 좌표값.
 * @param int snakeLength 뱀의 길이
 *
 * return snakeLength 변경된 뱀의 길이
 * */

int cutTail(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength) {

	for (int i = snakeLength - 1; i >= snakeLength / 2; i--) { //잘린 뱀의 꼬리를 구하는 for문
		gotoxy(snakeXY[0][i], snakeXY[1][i]); //잘린 꼬리의 위치로 이동 후,
		printf("%c", BLANK); //공백을 출력하여 잘린 꼬리를 지움.
	}
	snakeLength /= 2; //길이를 절반으로 줄임.

	return snakeLength; //변경된 snakeLength 를 반환.
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
void startGame( int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[], int consoleWidth, int consoleHeight, int snakeLength, int direction, int score, int speed)
{
	int gameOver = 0; // gameOver 여부 체크 변수. (0: 게임 진행, 1: 게임 오버, 2: 승리)
	clock_t endWait; // 대기 종료 시간을 담을 변수.

	int waitMili = CLOCKS_PER_SEC-(speed)*(CLOCKS_PER_SEC/10);	// 현재 게임 속도에 맞는 대기 시간 설정 (대기 시간 : 1초 - 게임속도(단계) * 0.1초)
<<<<<<< HEAD
=======
	int currentLevelScore = 10*speed; // 속도 증가 시점에서 현재 스코어와 비교할 기준값을 위한 임시 변수. 초기값 : 10 * 속도.
>>>>>>> a200997af65c5122c7e67dafd846bcb8fbfc3396
	int oldDirection = 0; // 직전 방향값을 저장하기 위한 변수
	int canChangeDirection = 1; // 방향 전환이 가능한 상태인지 저장 (0: 불가능, 1: 가능)

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
		if (direction == CUT_BUTTON) { //'C'키가 눌리고,
		    if(snakeLength > 8) { //뱀의 길이가 8보다 크면
			score /= 2;    //점수를 절반으로 깎고
			snakeLength = cutTail(snakeXY, snakeLength); //cuTail 함수를 호출하여 뱀의 길이를 절반으로 줄임.
			refreshInfoBar(score, speed); //하단바 갱신
		    }
		    direction = oldDirection; //뱀의 이동방향은 전의 그대로
		}

		if(clock() >= endWait) // 대기 종료 시간이 지난 경우. (컴퓨터 속도에 따라 동작.)
		{
			move(snakeXY, snakeLength, direction); // Snake를 지정한 방향으로 이동.
			canChangeDirection = 1;  //다시 방향 전환이 가능하도록.

				
			if(eatFood(snakeXY, foodXY)) // 현재 좌표에 먹이가 존재한다면
			{
				generateFood( foodXY, consoleWidth, consoleHeight, snakeXY, snakeLength); //새로운 먹이 생성.
				snakeLength++; //Snake의 길이 증가.
				score+=speed; //현재 속도만큼 점수 부여.
<<<<<<< HEAD
				speed = setSpeed(score, speed);
				waitMili = setWaitMili(waitMili, score, speed);
=======

				if( score >= getNextLevelScore(speed, currentLevelScore)) // 현재 점수가 게임 속도 * 10 + 기준값이 되는 현재 단계 스코어보다 큰 경우 게임 속도 증가 처리.
				{
					speed++; // 게임 속도 증가.
					currentLevelScore = score; // 판단 기준값을 현재 스코어로 변경.

					//게임 속도가 9 이하인 경우
					if(speed <= 9) //TODO : 점검 필요
						waitMili = waitMili - (CLOCKS_PER_SEC/10); // 대기 시간 단축 : 기존 대기시간 - 1/10초
					else // 게임 속도가 9보다 큰 경우
					{
						if(waitMili >= 40) // 현재 대기 시간이 게임 클럭 속도 기준 40 이상인 경우에만 속도 증가. (그보다 더 빠를 경우 현실적으로 게임 진행 불가한 속도이므로.)
							waitMili = waitMili - (CLOCKS_PER_SEC/200); // 기존 대기시간 - 1/200초. (속도 증가폭 낮춤)
						
					}
				}
>>>>>>> a200997af65c5122c7e67dafd846bcb8fbfc3396
				
				refreshInfoBar(score, speed); // 하단 바 갱신.
			}
			
			// HACK : clock() 리셋 방법 찾아서 수정 필요. (현재는 동작하는 것으로 보임)
			endWait = clock() + waitMili; //현재 시간 기준으로 대기 종료 시간 생신. 
		}
		
		gameOver = collisionDetection(snakeXY, consoleWidth, consoleHeight, snakeLength); // 게임 오버 조건 체크해 종료 코드 대입 (Snake가 창 끝에 닿았거나 Snake끼리 닿은 경우 1 대입 = 게임 오버)

		if(snakeLength >= SNAKE_ARRAY_SIZE-5) // Snake의 길이가 (설정한 최대 길이 - 5) 이상인 경우 게임 승리. (배열 크기보다 길어질 경우 크래시 발생하므로.)
		{
			gameOver = 2; //gameOver에 승리시를 뜻하는 코드 2 삽입. (while문 탈출 유도) 
			//TODO : 위 코드가 동작하지 않는 것으로 보인다고 함, 테스트 및 필요시 수정.
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

/**
* 게임 시작 후 화면을 지우고 게임범위의 상하, 좌우 벽을 출력한다.
*
* @param int consoleWidth : 게임 판 넓이
* @param int consoleHeight : 게임 판 높이
**/
void loadEnviroment(int consoleWidth, int consoleHeight)
{
	int x = 1, y = 1;
	int rectangleHeight = consoleHeight - 4; //y의 범위 1~20로 설정함
	clrscr(); //화면을 지움
	
	gotoxy(x,y);
	
	//왼쪽 오른쪽 벽 출력
	for (; y < rectangleHeight; y++)
	{
		gotoxy(x, y); //왼쪽벽 시작부분으로 커서이동
		printf("%c",WALL);
		
		gotoxy(consoleWidth, y); //오른쪽벽 시작부분으로 커서이동
		printf("%c",WALL);
	}
	
	y = 1;
	//상단 하단 벽 출력
	for (; x < consoleWidth+1; x++)
	{
		gotoxy(x, y); //상단벽 시작부분으로 커서이동
		printf("%c",WALL);
		
		gotoxy(x, rectangleHeight); //하단벽 시작부분으로 커서이동
		printf("%c",WALL);
	}
	
	return;
}


/**
* 게임시작 시 뱀을 화면에 그린다.
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : 뱀 머리, 몸통 위치 배열
* @param int snakeLength : 뱀의 길이
**/
void loadSnake(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
	int i;
	
	//뱀의머리부터 커서를 이동해가며 화면에 출력
	for (i = 0; i < snakeLength; i++){
		gotoxy(snakeXY[0][i], snakeXY[1][i]);
		printf("%c", SNAKE_BODY);
	}
	
	return;
}


/**
* 뱀 몸통 위치를 초기화한다.
*
* @param int snakeXY[][SNAKE_ARRAY_SIZE] : 뱀 머리, 몸통 위치 배열
* @param int snakeLength : 뱀의 길이
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
* 게임시작 직전, 모든 환경을 설정한 후, 게임시작함수를 호출하는 함수.
* 벽 생성, 뱀 표현, 음식 생성, 게임정보 출력 후 게임을 시작한다.
**/
void loadGame(void)
{
	int snakeXY[2][SNAKE_ARRAY_SIZE]; //뱀을 나타내기위한 배열
	
	int snakeLength = 4; //초기 뱀 길이
	
	int direction = LEFT_ARROW; //게임 시작 후 뱀 머리의 첫 방향
	
	int foodXY[] = {5,5}; //음식 위치 저장
	
	int score = 0; //초기 점수
	
	//벽 너비, 높이 설정	
	int consoleWidth = 80;
	int consoleHeight = 25;
	
	//게임 스피드 설정
	int speed = selectGameMode();
	
	//초기 뱀머리 위치 설정
	snakeXY[0][0] = 40; 
	snakeXY[1][0] = 10;
	
	loadEnviroment(consoleWidth, consoleHeight); //벽 생성
	prepairSnakeArray(snakeXY, snakeLength); //뱀을 나타내기위한 배열 생성
	loadSnake(snakeXY, snakeLength); //뱀 출력
	generateFood( foodXY, consoleWidth, consoleHeight, snakeXY, snakeLength); //음식 생성
	refreshInfoBar(score, speed); //게임 정보 출력
	startGame(snakeXY, foodXY, consoleWidth, consoleHeight, snakeLength, direction, score, speed); //본격적인 게임 시작

	return;
}

/**
* 메뉴 선택 구현 함수.
* 
* @param int x : 커서 x좌표
* @param int y : 커서 y좌표
* @param int yStart : 초기 커서 y좌표
* @return i : 메뉴에 할당된 값 (i = 0, 1, 2, 3)
**/
int menuSelector(int x, int y, int yStart)
{
	char key; //키 저장 변수
	int i = 0;
	x = x - 2; // ">"를 나타내기 위해 메뉴가 써있는 위치보다 왼쪽으로 두칸 이동
	//초기 ">"의 위치 출력
	gotoxy(x,yStart);
	printf(">");
	
	//커서 좌표 (1,1) 이동
	//gotoxy(1,1);
	CursorView(0); //커서 숨김

	//방향키 위, 아래를 입력하며 메뉴 선택 구현
	do
	{
		key = waitForAnyKey(); //입력 대기

		if ( key == (char)UP_ARROW ) //위 방향키 입력
		{
			gotoxy(x,yStart+i); //이동했으니 원래 있던 위치에 있는 ">"를 지움
			printf(" ");
			
			if (yStart >= yStart+i ) //위로 이동할 때, 제일 위에있는 메뉴의 위치보다 더 위로 올라가면 위치를 재조정하여 제일 아래로 오게함
				i = y - yStart - 2;
			else //단순히 위로 올라가는 것 구현
				i--;
			gotoxy(x,yStart+i); //y좌표를 계산한 것에 따라 ">" 출력
			printf(">");
		}
		else
			if ( key == (char)DOWN_ARROW ) //아래 방향키 입력
			{
				gotoxy(x,yStart+i); //이동했으니 원래 있던 위치에 있는 ">"를 지움
				printf(" ");
				
				if (i+2 >= y - yStart ) //아래로 이동할 때, 제일 아래에 있는 메뉴의 위치보다 더 아래로 내려가면 위치를 재조정하여 제일 위로 오게함
					i = 0;
				else //단순히 아래로 내려가는 것 구현
					i++;
				gotoxy(x,yStart+i); //y좌표를 계산한 것에 따라 ">" 출력
				printf(">");				
			}	

	} while(key != (char)ENTER_KEY); //Enter키 입력 시 종료

	return(i);
}
/**
* 초기화면 출력 함수.
* 초기화면을 출력한 후, waitForAnyKey()를 이용해 입력대기
**/
void welcomeArt(void)
{
	clrscr(); //전체화면 지움

	printf("\n");	
	printf("\t\t    _________         _________ 			\n");	
	printf("\t\t   /          )      /          ) 			\n");	
	printf("\t\t  /  /~~~~~|  |     /  /~~~~~|  | 			\n");	
	printf("\t\t  |  |     |  |     |  |     |  | 			\n");		
	printf("\t\t  |  |     |  |     |  |     |  | 			\n");
	printf("\t\t  |  |     |  |     |  |     |  |      /	\n");
	printf("\t\t  |  |     |  |     |  |     |  |     //	\n");
	printf("\t\t (o  o)    (  |_____/  /     (  |____/ / 	\n");
	printf("\t\t  (__/      (         /       (        / 	\n");
	printf("\t\t    |        ~~~~~~~~~         ~~~~~~~~ 		\n");
	printf("\t\t    ^											\n");
	printf("\t		Welcome To The Snake Game!			\n");
	printf("\t			    Press Any Key To Continue...	\n");			
	printf("\n");
	
	waitForAnyKey(); //입력대기
	return;
}
/**
* 조작법 출력 함수.
* 조작법 출력 후, waitForAnyKey()를 이용해 입력대기
**/
void controls(void)
{
	//커서 좌표
	int x = 10, y = 5;
	clrscr(); //전체화면 지움

	//gotoxy()를 이용해 커서를 이동하며 조작법 출력
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
	waitForAnyKey(); //입력대기
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
		pressed = (char)tolower(pressed); // 대문자일 경우 전부 소문자로 변환.
	} while (!(pressed == 'y' || pressed == 'n')); // y또는 n값이 아닌 경우 계속 반복.
	
	if (pressed == 'y') // y값인 경우 (=사용자가 종료 선택)
	{
		clrscr(); //지금까지 뿌린 화면 전부 삭제후.
		exit(1); //프로그램 종료. (code 1 : 정상 종료)
	}
	return; //n인 경우 : 함수 탈출, 기존 작업 계속 수행.
}
/**
* 메인메뉴 화면 출력 후, 메뉴를 고르는 함수.
*
* @return selected : 각 메뉴에 할당된 값
**/
int mainMenu(void)
{
	int x = 10, y = 5; //x, y에 커서의 이동 위치 저장
	int yStart = y; //초기에 커서를 나타낼 위치를 위한 값으로 menuSelector()에 파라미터로 필요한 값
	
	int selected; //각 메뉴에 할당된 값
	
	clrscr(); //전체화면 지움

	//gotoxy()를 이용해 커서 이동하며 메인메뉴 출력
	gotoxy(x,y++);
	printf("New Game\n");
	gotoxy(x,y++);
	printf("High Scores\n");
	gotoxy(x,y++);
	printf("Controls\n");
	gotoxy(x,y++);
	printf("Exit\n");
	gotoxy(x,y++);
	
	//menuSelector()를 이용해 각 메뉴에 대한 값을 반환하여 selected에 저장
	selected = menuSelector(x, y, yStart);

	return(selected);
}

/**
 * 다음 레벨의 기준 포인트 값을 구하는 함수
 *
 * @params int speed : 현재 게임의 속도
 * @params int currentLevelPoint : 현 레벨의 기준 포인트
 *
 * @return int basePoint : 다음 레벨의 기준 포인트
 **/
int getNextLevelScore(int speed, int currentLevelPoint)
{
	int basePoint = speed * 10 + currentLevelPoint;
	
	return basePoint;
}

/**
* main함수.
* 초기화면 출력 후, mainMenu()를 이용해 반환되는 값에 따라 각각 함수 실행
*
* @return 0
**/

int main() 
{

	welcomeArt(); //초기화면 출력
	
	do
	{	
		switch(mainMenu()) //각 메뉴에 대한 값 반환
		{
			case 0: //게임 시작
				loadGame();
				break;
			case 1: //게임 기록 확인
				displayHighScores();
				break;	
			case 2: //조작법 확인
				controls();
				break;		
			case 3: //게임종료 확인
				exitYN(); 
				break;			
		}		
	} while(1);
	
	return(0);
}

