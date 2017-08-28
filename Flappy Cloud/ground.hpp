//
//  ground.hpp
//  FlappyCloud


#ifndef ground_hpp
#define ground_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <vector>
#include "iostream"
#include <sstream>
#include <memory>
#include <random>

#include "globals.h"


/*La classe Ground permet de créer des morceaux de sol (un par block)*/
class Ground {
private:
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    sf::Texture groundTexture;
    sf::Sprite sprite;
    float blockLength;
public:
    Ground(b2World& world, float X, float blockLength);
    Ground();
    ~Ground();
    void draw(sf::RenderWindow& window);
};



#endif /* ground_hpp */
