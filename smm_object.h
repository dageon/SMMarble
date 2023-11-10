//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h


/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/
#define SMMNODE_TYPE_LECTURE		0
#define SMMNODE_TYPE_RESTAURANT		1
#define SMMNODE_TYPE_LABORATORY		2
#define SMMNODE_TYPE_HOME			3
#define SMMNODE_TYPE_GOTOLAB		4
#define SMMNODE_TYPE_FOODCHANCE		5
#define SMMNODE_TYPE_FESTIVAL		6


/* grade :
    A+,
    A0,
    A-,
    B+,
    B0,
    B-,
    C+,
    C0,
    C-
*/



//object generation
void smmObj_genNode(char* name, int type, int credit, int energy);

//member retrieving
char* smmObjname_getNodename(int node_nr);
int smmObjtype_getnodeCredit(int node_nr);
int smmObjtype_getnodeEnergy(int node_nr);

//element to string
char* smmObj_getNodetype(int node_nr);


#endif /* smm_object_h */
