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



static char smmObj_name[MAX_NODE][MAX_CHARNAME];
static int smmObj_type[MAX_NODE];
static int smmObj_credit[MAX_NODE];
static int smmObj_energy[MAX_NODE];
static int smmObj_noNode = 0;
static char smmNodeName[MAX_NODETYPE][MAX_CHARNAME] = {"lecture", "restaurant", "laboratory", "home", "experiment", "foodChance", "festival"};

static int smmObj_food[MAX_NODE][MAX_CHARNAME];
static int smmObj_charge[MAX_NODE];
static int smmObj_noFood = 0;
    
//object generation
void smmObj_genNode(char* name, int type, int credit, int energy)
{
	strcpy(smmObj_name[smmObj_noNode], name);
    smmObj_type[smmObj_noNode] = type;
    smmObj_credit[smmObj_noNode] = credit;
    smmObj_energy[smmObj_noNode] = energy;
    smmObj_noNode++;
}

void smmObj_genFood(char* food, int charge)
{
	strcpy(smmObj_food[smmObj_noFood], food);
    smmObj_charge[smmObj_noFood] = charge;
    smmObj_noFood++;
}

//member retrieving
char* smmObjname_getNodename(int node_nr)
{
	return smmObj_name[node_nr];
}
int smmObjtype_getnodeCredit(int node_nr)
{
	return smmObj_credit[node_nr];
}
int smmObjtype_getnodeEnergy(int node_nr)
{
	return smmObj_energy[node_nr];
}

char* smmObjname_getFoodname(int node_nr)
{
	return smmObj_food[node_nr];
}
int smmObjtype_getfoodCharge(int node_nr)
{
	return smmObj_charge[node_nr];
}

//element to string
char* smmObj_getNodetype(int node_nr)
{
	switch(smmObj_type[node_nr]) {
		case SMMNODE_TYPE_LECTURE:
			return smmNodeName[SMMNODE_TYPE_LECTURE];
		case SMMNODE_TYPE_RESTAURANT:
			return smmNodeName[SMMNODE_TYPE_RESTAURANT];
		case SMMNODE_TYPE_LABORATORY:
			return smmNodeName[SMMNODE_TYPE_LABORATORY];
		case SMMNODE_TYPE_HOME:
			return smmNodeName[SMMNODE_TYPE_HOME];
		case SMMNODE_TYPE_GOTOLAB:
			return smmNodeName[SMMNODE_TYPE_GOTOLAB];
		case SMMNODE_TYPE_FOODCHANCE:
			return smmNodeName[SMMNODE_TYPE_FOODCHANCE];
		case SMMNODE_TYPE_FESTIVAL:
			return smmNodeName[SMMNODE_TYPE_FESTIVAL];
	}

}
/*
char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}
*/
