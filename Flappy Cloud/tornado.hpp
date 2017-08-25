//
//  tornado.hpp
//  FlappyCloud
//
//  Created by Olivier Freyssinet on 25/08/2017.
//  Copyright © 2017 Appdea. All rights reserved.
//

#ifndef tornado_hpp
#define tornado_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include "obstacle.hpp"

class Tornado: public Obstacle {
private:
    
public:
    Tornado(b2World& world, float X, float Y);
    void updateVelocity();
    void playPause();
};



#endif /* tornado_hpp */
