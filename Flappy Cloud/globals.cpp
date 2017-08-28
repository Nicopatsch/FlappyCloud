//
//  globals.cpp
//  FlappyCloud


#include "globals.h"


float SCALE = 30.f;

/*Variable globale permettant garder un oeil
 sur le nombre de grounds existant. Elle est incrémentée
 par Ground() et décrémentée par ~Ground().*/

int nbGrounds = 0;

/*on garde un compteur du nombre d'Obstacle créés*/
int nbObstacles = 0;

int nbCeillings = 0;

int nbBlocks = 0;
