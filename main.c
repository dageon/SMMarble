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
	int flag_escape; //실험실 탈출 확인하는 flag 
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
	if((cur_player[player].accumCredit >= GRADUATE_CREDIT) && cur_player[player].position == 0){
		cur_player[player].flag_graduate = 1;
	}
	return cur_player[player].flag_graduate;
}

//학점 출력을 위한 배열 
const char* gradename[] = {"A+", "A0", "A-", "B+", "B0", "B-", "C+", "C0", "C-"};
const double gradenum[] = {4.3, 4.0, 3.7, 3.3, 3.0, 2.7, 2.3, 2.0, 1.7};

void printGrades(int player)
{
	int i;
	void* gradePtr;
	double gradesum=0; //총 성적 
	int creditsum=0;  //총 이수학점 
	double gradeavg=0; //최종 평점 
	
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
	
	for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
	{
		//저장된 성적 데이터 읽어옵 
		gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		//성적 합산하기 (성적변환점수X이수학점) 
		gradesum = gradesum + gradenum[smmObj_getNodeGrade(gradePtr)] * smmObj_getNodeCredit(gradePtr);
		//이수학점 합산하기 
		creditsum = creditsum + smmObj_getNodeCredit(gradePtr);
	}
	//성적 계산하기 
	gradeavg = gradesum/creditsum;
	
	//성적이 있는 경우 
	if(gradeavg>0){
		printf("--> Printing player's grade (average (%f)) ::::\n", gradeavg);
	}
	//성적이 없는 경우 
	else{
		gradeavg = 0;
		printf("--> Printing player's grade (average (%f)) ::::\n", gradeavg);
	}
	
	for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
	{
		//저장된 성적 데이터 읽어오기 
		gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		//학점 출력
		printf("  => %s : %s\n", smmObj_getNodename(gradePtr), gradename[smmObj_getNodeGrade(gradePtr)]);
	}
	
	
}

void printPlayerStatus(void) 
{
	int i;
	printf("========================== PLAYER STATUS ==========================\n");
	for(i=0;i<player_nr;i++)
	{
		 printf("%s : credit %i, energy %i, position %i",
		 		cur_player[i].name,
		 		cur_player[i].accumCredit,
				cur_player[i].energy, 
				cur_player[i].position);
		if(cur_player[i].flag_escape == 1){
			printf("(exp)");
		}
		printf("\n");
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
		
		//set flag_escape
		cur_player[i].flag_escape = 0;
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


//실험을 진행하는 함수 
void play_exp(int player, int sucess) {
	int dicenum = rollDice();
	printf("\n주사위값 : %i\n", dicenum);
	if(dicenum>=sucess){
		printf("플레이어 탈출\n\n");
		cur_player[player].flag_escape=0;
	}
	else{
		printf("플레이어 탈출 실패\n\n");
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
	int type = smmObj_getNodeType(boardPtr); 

	
	int check=-1; //수강 의사 확인을 위한 변수 
	int lec_flag=0; //수강 여부 확인을 위한 flag 
	
	char c; //카드 뽑을 때 키보드로 아무거나 입력받을 변수
	
	int i; //for문 사용 변수
	
	int sucess; //실험 성공 기준값
	
	int turn = (turn+1)%player_nr;
	
	
    switch(type)
    {
        //case lecture:
        case SMMNODE_TYPE_LECTURE:
        	// 수강 여부를 물어모기 위한 코드 
        	printf("%s (credit:%d, energy:%d) 수강하시겠습니까? 수강을 원하면 1, 원하지 않으면 0을 입력하세요.",
						smmObj_getNodename(boardPtr), smmObj_getNodeCredit(boardPtr), smmObj_getNodeEnergy(boardPtr));
			
			// 1, 0이 아닌 다른 입력을 했을 경우 다시 입력하게 하기 위해 while문 사용 
        	while(1){
        		scanf("%d", &check);
        		fflush(stdin);
        		// 수강 
        		if(check==1){	
					//이미 수강한 강의인지 확인 
					for(i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++){
						gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
						if(strcmp(smmObj_getNodename(gradePtr),smmObj_getNodename(boardPtr))==0){
							printf("이미 수강한 강의입니다.\n") ;
							lec_flag=1;
							break;
						}
					}
					
					// 에너지 확인 
        			if(cur_player[player].energy < smmObj_getNodeEnergy(boardPtr)) {
        				printf("%s는 에너지가 부족해 강의 수강 불가능합니다.\n", cur_player[player].name);
        				break;
					}
					
					
					//수강하지 않은 강의라면 수강 
					if(lec_flag!=1){
						cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
        				cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        		
						gradePtr = smmObj_genObject(smmObj_getNodename(boardPtr), smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, rand()%smmObjGrade_COUNT);
						printf("%s 수강하였습니다 (grade:%s) \n\n", smmObj_getNodename(boardPtr), gradename[smmObj_getNodeGrade(gradePtr)]);
            			smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);

        				break;
					}
					break;
				}
				// 드랍 
        		else if(check==0) {
        			printf("%s 드랍하였습니다\n\n", smmObj_getNodename(boardPtr));
        			break;
				}
				else{
					printf("1(수강), 0(드랍) 중에서  입력해주세요.  ");
				}
			}
			break;

        //case restaurant:
        case SMMNODE_TYPE_RESTAURANT:
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	printf("%s는 %s에서 에너지(%i) 획득합니다.\n\n", cur_player[player].name, smmObj_getNodename(boardPtr), smmObj_getNodeEnergy(boardPtr));
        	break;
        	
        //case laboratory:
        case SMMNODE_TYPE_LABORATORY:
			if (cur_player[player].flag_escape == 1) {
				printf("실험을 진행합니다.(기준값: %d)\n", sucess);
				//실험 진행 
				play_exp(turn, sucess);
				//만약 실험 결과 성공했다면 
				if(cur_player[player].flag_escape==0) {
					cur_player[player].position = 12; //실험중으로 이동
				}
			}
			else {	
				printf("%s는 실험 중 상태가 아니라 그대로 실험실을 지나칩니다.\n\n",cur_player[player].name);
			}
        	break;
        	
        //case home:
        case SMMNODE_TYPE_HOME:
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	printf("%s 집에서 에너지 보충합니다.\n\n", cur_player[player].name);
        	break;
        	
        //case experiment:
        case SMMNODE_TYPE_GOTOLAB:
        	printf("실험 중 상태\n\n");
        	sucess = rollDice(); //실험 성공 기준값 설정 
        	cur_player[player].position = 8; //실험실로 이동 
        	cur_player[player].flag_escape = 1; 
			break;
			
		//case foodChance:
		case SMMNODE_TYPE_FOODCHANCE:
			foodPtr = smmdb_getData(LISTNO_FOODCARD, rand()%smmdb_len(LISTNO_FOODCARD));
			printf("%s의 음식 찬스!  ", cur_player[player].name);
			printf("음식 카드를 뽑기 위해 아무 키나 눌러주세요.  ");
			c = getchar();
			fflush(stdin);
			cur_player[player].energy += smmObj_getNodeEnergy(foodPtr);
			printf("%s는 %s 획득 (에너지: %i).\n\n",cur_player[player].name, smmObj_getNodename(foodPtr), smmObj_getNodeEnergy(foodPtr));
			break;
			
		//case festival:
		case SMMNODE_TYPE_FESTIVAL:
			festPtr = smmdb_getData(LISTNO_FESTCARD, rand()%smmdb_len(LISTNO_FESTCARD));
			printf("%s의 축제 참여!  ", cur_player[player].name);
			printf("페스티벌 카드를 뽑기 위해 아무 키나 눌러주세요.  ");
			c = getchar();
			fflush(stdin);
			printf("-> Mission : %s \n", smmObj_getNodename(festPtr));
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
	int i;
	
	// step(주사위 눈)을 더했을 때 위치 값이 보드판 번호를 벗어나는 경우 
    if((cur_player[player].position+step)>=16) {
    	// 보드 끝까지 이동 경로 출력 
		for(i=cur_player[player].position;i<=15;i++){
			boardPtr = smmdb_getData(LISTNO_NODE, i);
			printf("	%i칸으로 이동 => %s\n", i, smmObj_getNodename(boardPtr));
		}
		
		cur_player[player].position -= 16;
		cur_player[player].position += step;
		
		// 우리집 지날 때 에너지 보충 
		if(cur_player[player].position>=1){
			boardPtr = smmdb_getData(LISTNO_NODE, 0);
			cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	//printf("%s 집에서 에너지 보충합니다 => %d.\n\n", cur_player[player].name, cur_player[player].energy);
		}
		// 보드판 번호 다시 계산 후 경로 출력 
		for(i=0;i<=cur_player[player].position;i++){
			boardPtr = smmdb_getData(LISTNO_NODE, i);
			printf("	%i칸으로 이동 => %s\n", i, smmObj_getNodename(boardPtr));
		}
	}
	// 보드판 번호를 벗어나지 않는 경우
	else{
		for(i=cur_player[player].position+1;i<=cur_player[player].position+step; i++) {
    		boardPtr = smmdb_getData(LISTNO_NODE, i);
    		printf("	%i칸으로 이동 => %s\n", i, smmObj_getNodename(boardPtr));
    	}
    	cur_player[player].position += step;
	}

    // 플레이어의 도착 위치 출력
    printf("%s go to node %i (name: %s)\n", cur_player[player].name,
				cur_player[player].position,
				smmObj_getNodename(boardPtr));
}

//주사위 굴리는 함수 (실험에 쓰임)
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
    	void *foodObj = smmObj_genObject(food, smmObjType_card, 0, 0, charge, 0);
    	
        smmdb_addTail(LISTNO_FOODCARD, foodObj);
        
        food_nr++;
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    
    for(i=0;i<food_nr;i++)
    {
    	void *foodObj = smmdb_getData(LISTNO_FOODCARD, i);

    	printf("=> %i. %s charge:%i\n",
			i, smmObj_getNodename(foodObj),
			smmObj_getNodeEnergy(foodObj));
    }
    


    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while ((fscanf(fp, "%s", festival)) == 1) //read a festival card string
    {
    	void *festObj = smmObj_genObject(festival, smmObjType_card, 0, 0, 0, 0);
        smmdb_addTail(LISTNO_FESTCARD, festObj);
        
        festival_nr++;
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    
    for(i=0;i<festival_nr;i++)
    {
    	void *festObj = smmdb_getData(LISTNO_FESTCARD, i);
    	printf("=> %i. %s\n", i, smmObj_getNodename(festObj));
    }
    
    
    // 게임 시작 화면
	printf("\n\n\n=======================================================================\n");
	printf("-----------------------------------------------------------------------\n");
	printf("        Sookmyung Marble !! Let's Graduate (total credit : %d)!!\n", GRADUATE_CREDIT);
	printf("-----------------------------------------------------------------------\n");
	printf("=======================================================================\n\n\n");
    
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
        
        if(cur_player[turn].flag_escape != 1) {
        	die_result = rolldie(turn);
        	printf("--> die result: %i\n", die_result);
        	goForward(turn, die_result);
		}
		else {
			die_result = rolldie(turn);
		}
        /*
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        printf("--> die result: %i\n", die_result);
		*/
		
        //4-3. go forward
        //goForward(turn, die_result);
		
		//졸업한 사람이 생기면 while문을 빠져나감(=게임 종료) 
		if (isGraduated(turn) == 1) break;
		
		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn + 1) % player_nr;
    }
    
    printf("\n게임이 종료되었습니다.\n");
    
    //졸업한 플레이어 정보 출력 
    printf("졸업한 플레이어: %s\n", cur_player[turn].name);
    printGrades(turn);
    
    free(cur_player);

    return 0;
}
