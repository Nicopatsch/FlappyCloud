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


#endif /* globals_h */
