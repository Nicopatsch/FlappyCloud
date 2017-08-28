//
//  obstacle.hpp
//  FlappyCloud

#ifndef obstacle_hpp
#define obstacle_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <vector>
#include "iostream"
#include <sstream>
#include <memory>
#include <random>

#include "globals.h"


/*Les obstacles sont de 2 types (classes dérivées)*/
class Obstacle {
protected: // Protected: tout objet de classe dérivée peut accéder à ses propres attributs déf dans la classe de base
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    sf::Texture texture;
    sf::Sprite sprite;
    float height, width;
    
public:
    Obstacle(b2World& world, float X, float Y, float width, float height);
    
    ~Obstacle();
    
    void draw(sf::RenderWindow& window);
};

#endif /* obstacle_hpp */
