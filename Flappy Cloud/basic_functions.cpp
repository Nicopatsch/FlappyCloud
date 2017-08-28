 //
//  basic_functions.cpp
//  FlappyCloud


#include "basic_functions.h"



float distanceBetween(std::pair<float, float> a, std::pair<float, float> b) {
    return sqrt((a.first - b.first) * (a.first - b.first) + (a.second - b.second) * (a.second - b.second));
}

int randomIntBetween(int inf, int sup) {
    static std::random_device rd;
    static std::default_random_engine engine(rd());
    std::uniform_int_distribution<unsigned> distribution(inf, sup);
    return distribution(engine);
}
