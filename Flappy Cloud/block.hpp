//
//  block.hpp
//  FlappyCloud


#ifndef block_hpp
#define block_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <vector>
#include "iostream"
#include <sstream>
#include <memory>
#include <random>

#include "storm.hpp"
#include "tornado.hpp"
#include "ground.hpp"
#include "ceilling.hpp"


class Block {
private:
    int N;
    vector<Storm> storms = vector<Storm>();
    vector<Tornado> tornadoes = vector<Tornado>();
    Ground ground;
    Ceilling ceilling;
    int obstPerBlock;
    float blockLength;
public:
    Block(b2World& world, int N, float stormVY, int obstPerBlock, float blockLength);
    
    Block();
    
    ~Block();
    
    void draw(sf::RenderWindow& window);
    
    
    float getPositionX();
    
    int getIndex();
    
    int getNbBlocks();
    
};



#endif /* block_hpp */
