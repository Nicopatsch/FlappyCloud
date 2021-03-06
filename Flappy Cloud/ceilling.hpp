//
//  ceilling.hpp
//  FlappyCloud


#ifndef ceilling_hpp
#define ceilling_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <vector>
#include "iostream"
#include <sstream>
#include <memory>
#include <random>

#include "globals.h"

/*La classe Ceilling est très similaire à la classe Ground.
 Elle permet de créer les morceaux de plafond nuageux.*/
class Ceilling {
private:
    gameEntityType gEntityType = ceilling;
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    sf::Texture ceillingTexture;
    sf::Sprite sprite;
    float blockLength;
public:
    Ceilling(b2World& world, float X, float blockLength);
    Ceilling();
    ~Ceilling();
    void draw(sf::RenderWindow& window);
    
    gameEntityType getGameEntityType();

};



#endif /* ceilling_hpp */
