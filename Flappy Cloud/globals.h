//
//  globals.h
//  FlappyCloud
//
//  Created by Olivier Freyssinet on 24/08/2017.
//  Copyright © 2017 Appdea. All rights reserved.
//

#ifndef globals_h
#define globals_h

#include <stdio.h>
#include "iostream"

#include "ResourcePath.hpp"

#include "pugixml.hpp"
#include "pugiconfig.hpp"


using namespace pugi;
using namespace std;


void loadVariables();


/*Toutes les variables globales qui vont être initialisées
 depuis le fichier data.xml*/

extern float SCALE;
extern float gravityX, gravityY;
extern float velocityY, velocityX;
extern float scoreCoeff;
extern float timeStep, velocityIterations, positionIterations;
extern float blockLength; //Length of a block
extern int obstPerBlock; //Number of each type of obstacles per block
extern float stormVelocityY; // Storm objects vertical velocity (moving obstacles)
extern float epsilon;


//extern float circleRadius;
//static float circleRadius = .5f;


#endif /* globals_h */
