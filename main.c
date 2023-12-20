//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;



typedef struct plyaer {
	int energy;
	int position;
	char name[MAX_CHARNAME];
	int accumCredit;
	int flag_graduate;
	int flag_escape;
} player_t;

static player_t *cur_player;
//static plyaer_t cur_player[MAX_PLAYER];


#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif

//function prototypes
#if 0
int isGraduated(void); //check if any player is graduated
void generatePlayers(int n, int initEnergy); //generate a new player
void printGrades(int player); //print grade history of the player
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif

int isGraduated(int player) {
	if((cur_player[player].accumCredit >= GRADUATE_CREDIT) && cur_player[player].position == SMMNODE_TYPE_HOME){
		cur_player[player].flag_graduate = 1;
	}
	return cur_player[player].flag_graduate;
}

const char* gradename[] = {"A+", "A0", "A-", "B+", "B0", "B-", "C+", "C0", "C-"};

void printGrades(int player)
{
	int i;
	void* gradePtr;
	
	for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
	{
		//저장된 성적 데이터 읽어옴 
		gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		//일단은 숫자가 아닌 알파벳으로 출력 
		printf("--> Printing player's grade (average ((구현))) ::::\n") ; //이부분 수정 필요 
		printf("%s : %s\n", smmObj_getNodename(gradePtr), gradename[smmObj_NodeGrade(gradePtr)]);
	}
}

void printPlayerStatus(void) 
{
	int i;
	printf("========================== PLAYER STATUS ==========================\n");
	for(i=0;i<player_nr;i++)
	{
		 printf("%s : credit %i, energy %i, position %i\n",
		 		cur_player[i].name,
		 		cur_player[i].accumCredit,
				cur_player[i].energy, 
				cur_player[i].position);
	}
	printf("========================== PLAYER STATUS ==========================\n\n");
}

void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i;
	//n time loop
	for(i=0;i<n;i++)
	{
		//input name
		printf("Input plaer %i's name:", i); //안내 문구 
		scanf("%s", cur_player[i].name);
		fflush(stdin);

		
		//set position
		cur_player[i].position = 0;
		
		//set energy
		cur_player[i].energy = initEnergy;
		
		//set accumCredit
		cur_player[i].accumCredit = 0;
		
		//set flag_graduate
		cur_player[i].flag_graduate = 0;
	}
}

//보드판 주사위 굴리는 함수
int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    

    if (c == 'g')
    	printGrades(player);

    
    return (rand()%MAX_DIE + 1);
}

void play_exp(int player, int sucess) {
	int dicenum = rollDice();
	printf("\n주사위값 : %i\n", dicenum);
	if(dicenum>=sucess){
		printf("플레이어 탈출\n");
		cur_player[player].flag_escape=0;
	}
	else{
		printf("플레이어 탈출 실패\n");
		cur_player[player].flag_escape=1;
	}
}



//action code when a player stays at a node
void actionNode(int player)
{
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
	void *foodPtr;
	void *festPtr;
	void *gradePtr;
	//int type = smmObj_getNodeType(cur_player[player].position);
	int type = smmObj_getNodeType(boardPtr);
	char *name = smmObj_getNodename(boardPtr);
	int grade;
	int sucess;
	int turn = (turn+1)%player_nr;
	char c;
	
    switch(type)
    {
        //case lecture:
        case SMMNODE_TYPE_LECTURE:
        	//수강할건지 물어봐야함 
        	//이전에 듣지 않은 강의여야 함 
        	if(cur_player[player].energy < smmObj_getNodeEnergy(boardPtr)) {
        		printf("%s는 에너지가 부족해 강의 수강 불가능\n", cur_player[player].name); 
        		break;
			}
        	cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
        	cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);

			gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, rand()%smmObjGrade_COUNT);
			printf("%s 넣었다\n", smmObj_getNodename(boardPtr));
            smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
        	break;
        //case restaurant:
        case SMMNODE_TYPE_RESTAURANT:
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	printf("%s는 %s에서 에너지(%i) 보충합니다.\n\n", cur_player[player].name, smmObj_getNodename(boardPtr), smmObj_getNodeEnergy(boardPtr));
        	break;
        //case laboratory:
        case SMMNODE_TYPE_LABORATORY:
        	printf("실험실\n\n");
			if (cur_player[player].flag_escape == 1) {
				play_exp(turn, sucess);
				printf("성공했을까");
				break;
			}
			else {	
				printf("%s는 그냥 실험실을 지나칩니다.\n",cur_player[player].name);
			}
			/*
        	if(cur_player[player].flag_graduate == 1) {
        		int play = rollDice();
        		if(play>sucess) {
        			printf("end the exp\n\n");
        			cur_player[player].flag_graduate = 0;
        			cur_player[player].position = 12;
				}
        		else {
					printf("못끝냄\n");
				}
			}
			else {
				printf("그냥 지나침\n");
			}
			*/
        	break;
        //case home:
        case SMMNODE_TYPE_HOME:
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	printf("%s 집에서 에너지 보충합니다.\n\n", cur_player[player].name);
        	break;
        //case experiment:
        case SMMNODE_TYPE_GOTOLAB:
        	printf("실험 중 상태\n\n");
        	sucess = rollDice();
        	cur_player[player].position = 8; //이부분 고쳐야 함 
        	cur_player[player].flag_escape = 1;
        	play_exp(turn, sucess);
			break;
		//case foodChance:
		case SMMNODE_TYPE_FOODCHANCE:
			foodPtr = smmdb_getData(LISTNO_FOODCARD, rand()%smmdb_len(LISTNO_FOODCARD));
			printf("%s의 음식 찬스!  ", cur_player[player].name);
			printf("음식 카드를 뽑기 위해 아무 키나 눌러주세요.  ");
			c = getchar();
			fflush(stdin);
			cur_player[player].energy += smmObj_getFoodCharge(foodPtr);
			printf("%s는 %s 먹고  에너지(%i) 보충합니다.\n\n",cur_player[player].name, smmObj_getFoodname(foodPtr), smmObj_getFoodCharge(foodPtr));
			break;
		//case가 festival:
		case SMMNODE_TYPE_FESTIVAL:
			festPtr = smmdb_getData(LISTNO_FESTCARD, rand()%smmdb_len(LISTNO_FESTCARD));
			printf("%s의 축제 참여!  ", cur_player[player].name);
			printf("페스티벌 카드를 뽑기 위해 아무 키나 눌러주세요.  ");
			c = getchar();
			fflush(stdin);
			printf("-> Mission : %s \n", smmObj_getFestivalname(festPtr));
    		printf("미션 완료 후 아무 키나 눌러주세요.  ");
    		c = getchar();
    		fflush(stdin);
			break; 
        default:
            break;
    }
}

void goForward(int player, int step) {
	void *boardPtr;

	// step을 더했을 때 위치 값이 보드판 번호를 벗어나는 경우 조절 
    if((cur_player[player].position+step)>=16) {
			cur_player[player].position -= 16;
			printf("다시 처음");
	}
	
	cur_player[player].position += step;
	
	boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	
	printf("%s go to node %i (name: %s)\n", cur_player[player].name,
				cur_player[player].position,
				smmObj_getNodename(boardPtr));
}

int rollDice() {
	return rand() % MAX_DIE + 1;
}



int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int initEnergy;
    int i;
    int turn = 0;
    
    char food[MAX_CHARNAME];
    int charge;
    
    char festival[MAX_CHARNAME];
    

    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while ((fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy)) == 4) //read a node parameter set
    {
        void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj);
        
        if (type == SMMNODE_TYPE_HOME)
        	initEnergy = energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    
    for(i=0;i<board_nr;i++)
    {
    	void *boardObj = smmdb_getData(LISTNO_NODE, i);

    	printf("=> %i. %s (%s), credit:%i, energy:%i\n",
			i, smmObj_getNodename(boardObj),
			smmObj_getTypeName(smmObj_getNodeType(boardObj)),
			smmObj_getNodeCredit(boardObj),
			smmObj_getNodeEnergy(boardObj));
	}
    
    
    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while ((fscanf(fp, "%s%i", food, &charge)) == 2) //read a food parameter set
    {
    	//store the parameter set
    	void *foodObj = smmObj_genFood(food, charge);
        smmdb_addTail(LISTNO_FOODCARD, foodObj);
        
        food_nr++;
    }
    fclose(fp);
    
    
    for(i=0;i<food_nr;i++)
    {
    	void *foodObj = smmdb_getData(LISTNO_FOODCARD, i);

    	printf("=> %i. %s charge:%i\n",
			i, smmObj_getFoodname(foodObj),
			smmObj_getFoodCharge(foodObj));
    	
    	//printf("=> %i. %s, credit:%i\n", i, smmObj_getFoodname(i), smmObj_getFoodCharge(i));
    }
    printf("Total number of food cards : %i\n", food_nr);


    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while ((fscanf(fp, "%s", festival)) == 1) //read a festival card string
    {
    	void *festObj = smmObj_genFestival(festival);
        smmdb_addTail(LISTNO_FESTCARD, festObj);
        
        festival_nr++;
    }
    fclose(fp);
    
    for(i=0;i<festival_nr;i++)
    {
    	void *festObj = smmdb_getData(LISTNO_FESTCARD, i);
    	printf("=> %i. %s\n", i, smmObj_getFestivalname(festObj));
    }
    printf("Total number of festival cards : %i\n", festival_nr);
    
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("input player no.: ");
		scanf("%d", &player_nr);
		fflush(stdin);
    }
    while (player_nr < 0 || player_nr > MAX_PLAYER);
	
	cur_player = (player_t*)malloc(sizeof(player_t));
    generatePlayers(player_nr, initEnergy);
    
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated(turn) == 0) //is anybody graduated?
    {
        int die_result;
        
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        printf("--> die result: %i\n\n", die_result);
		
        //4-3. go forward
        goForward(turn, die_result);

		
		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn + 1) % player_nr;
    }
    
    free(cur_player);

    return 0;
}
