//
//  rainbow.cpp
//  FlappyCloud
//
//  Created by Olivier Freyssinet on 29/08/2017.
//  Copyright © 2017 Appdea. All rights reserved.
//

#include "rainbow.hpp"
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <vector>
#include "iostream"
#include <sstream>
#include <memory>
#include <random>

#include "cloud.h"

class Rainbow {
private:
    b2BodyDef bodyDef;
    b2Body* body;
    b2CircleShape shape;
    b2FixtureDef fixtureDef;
    float radius;
    Cloud* cloud;
public:
    Rainbow(b2World& world, Cloud& cloud, float X, float Y);
    ~Rainbow();
    void checkCollisionWithCloud();
    
};
