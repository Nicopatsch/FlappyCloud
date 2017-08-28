//
//  basic_functions.h
//  FlappyCloud


#ifndef basic_functions_h
#define basic_functions_h

#include <stdio.h>
#include "iostream"
#include <sstream>
#include <memory>
#include <random>

#include "globals.h"

using namespace std;

float distanceBetween(std::pair<float, float> a, std::pair<float, float> b);

int randomIntBetween(int inf, int sup);
#endif /* basic_functions_h */
