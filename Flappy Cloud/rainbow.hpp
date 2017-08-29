//
//  rainbow.hpp
//  FlappyCloud


#ifndef rainbow_hpp
#define rainbow_hpp

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
    sf::CircleShape sfCircle;
    float radius = .5;
    
public:
    Rainbow(b2World& world, float X, float Y);
    Rainbow();
    ~Rainbow();
    void draw(sf::RenderWindow& window);
    
};
#endif /* rainbow_hpp */
