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


//static smmObject_t smm_node[MAX_NODE];
//static int smmObj_noNode = 0;

static char smmObj_food[MAX_NODE][MAX_CHARNAME];
static int smmObj_charge[MAX_NODE];
static int smmObj_noFood = 0;

static char smmObj_festival[MAX_NODE][MAX_CHARNAME];
static int smmObj_noFestival = 0;



// 3. 관련 함수 변경  
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

void smmObj_genFood(char* food, int charge)
{
	strcpy(smmObj_food[smmObj_noFood], food);
    smmObj_charge[smmObj_noFood] = charge;
    smmObj_noFood++;
}

void smmObj_genFestival(char* festival)
{
	strcpy(smmObj_festival[smmObj_noFestival], festival);
    smmObj_noFestival++;
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


char* smmObj_getFoodname(int node_nr)
{
	return smmObj_food[node_nr];
}
int smmObj_getFoodCharge(int node_nr)
{
	return smmObj_charge[node_nr];
}

char* smmObj_getFestivalname(int node_nr)
{
	return smmObj_festival[node_nr];
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
