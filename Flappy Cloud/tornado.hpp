//
//  tornado.hpp
//  FlappyCloud


#ifndef tornado_hpp
#define tornado_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include "obstacle.hpp"

class Tornado: public Obstacle {
private:
    gameEntityType gEntityType = tornado;
    
public:
    Tornado(b2World& world, float X, float Y);
    void updateVelocity();
//    void playPause();
    gameEntityType getGameEntityType();

};



#endif /* tornado_hpp */
