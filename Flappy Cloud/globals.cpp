//
//  globals.cpp
//  FlappyCloud
//
//  Created by Olivier Freyssinet on 24/08/2017.
//  Copyright © 2017 Appdea. All rights reserved.
//

#include "globals.h"


float SCALE = 30.f;

/*Variable globale permettant garder un oeil
 sur le nombre de grounds existant. Elle est incrémentée
 par Ground() et décrémentée par ~Ground().*/

int nbGrounds = 0;

/*on garde un compteur du nombre d'Obstacle créés*/
int nbObstacles = 0;


int nbCeillings = 0;
