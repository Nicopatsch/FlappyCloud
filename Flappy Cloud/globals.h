//
//  globals.h
//  FlappyCloud


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

extern float SCALE;

enum gameEntityType { ceilling, ground, cloud, storm, tornado, rainbow };



#endif /* globals_h */
