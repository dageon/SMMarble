//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE		100

static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = {
	"lecture",
	"restaurant",
	"laboratory",
	"home",
	"experiment",
	"foodChance",
	"festival"
};

// 구조체 형식 정의 
typedef struct smmObject {
	char name[MAX_CHARNAME];
	smmObjType_e objType;
	int type;
	int credit;
	int energy;
	smmObjGrade_e grade;
} smmObject_t;

typedef struct smmFood{
	char food[MAX_CHARNAME];
	int charge;
} smmObject_f;

typedef struct smmFestival{
	char festival[MAX_CHARNAME];
} smmObject_v;

//static smmObject_t smm_node[MAX_NODE];
//static int smmObj_noNode = 0;

//static char smmObj_food[MAX_NODE][MAX_CHARNAME];
//static int smmObj_charge[MAX_NODE];
//static int smmObj_noFood = 0;

//static char smmObj_festival[MAX_NODE][MAX_CHARNAME];
//static int smmObj_noFestival = 0;

 
//object generation
void* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
{
	smmObject_t* ptr;
	ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
	
	strcpy(ptr->name, name);
	ptr->objType = objType;
	ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;

    return ptr;
}

//음식 카드 생성 
void* smmObj_genFood(char* food, int charge)
{
	smmObject_f* ptr;
	ptr = (smmObject_f*)malloc(sizeof(smmObject_f));
	
	strcpy(ptr->food, food);
	ptr->charge = charge;
	
	return ptr;
}

//페스티벌 카드 생성 
void* smmObj_genFestival(char* festival)
{
	smmObject_v* ptr;
	ptr = (smmObject_v*)malloc(sizeof(smmObject_v));
	
	strcpy(ptr->festival, festival);
    
    return ptr;
}

//member retrieving
char* smmObj_getNodename(void* obj)
{
	smmObject_t* ptr = (smmObject_t*)obj;
	return ptr->name;
}

int smmObj_getNodeType(void* obj)
{
	smmObject_t* ptr = (smmObject_t*)obj;
	return ptr->type;
}

int smmObj_getNodeCredit(void* obj)
{
	smmObject_t* ptr = (smmObject_t*)obj;
	return ptr->credit;
}

int smmObj_getNodeEnergy(void* obj)
{
	smmObject_t* ptr = (smmObject_t*)obj;
	return ptr->energy;
}


//음식 카드 초기 
char* smmObj_getFoodname(void* obj)
{
	smmObject_f* ptr = (smmObject_f*)obj;
	return ptr->food;
}
int smmObj_getFoodCharge(void* obj)
{
	smmObject_f* ptr = (smmObject_f*)obj;
	return ptr->charge;
}


//페스티벌 카드 초기화 
char* smmObj_getFestivalname(void* obj)
{
	smmObject_v* ptr = (smmObject_v*)obj;
	return ptr->festival;
}

//element to string
char* smmObj_getTypeName(int type)
{
	return (char*)smmNodeName[type];

}
/*
char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}
*/
