//
//  game.hpp
//  FlappyCloud


#ifndef game_hpp
#define game_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <vector>
#include "iostream"
#include <sstream>
#include <memory>
#include <random>

#include "ResourcePath.hpp"
#include "pugixml.hpp"
#include "pugiconfig.hpp"

#include "globals.h"
#include "basic_functions.h"
#include "cloud.h"
#include "ground.hpp"
#include "ceilling.hpp"
#include "obstacle.hpp"
#include "storm.hpp"
#include "tornado.hpp"
#include "block.hpp"

class Game{
private:
    //parametres de jeu
    float gravityX, gravityY;
    float velocityY, velocityX;
    float scoreCoeff;
    float velocityIterations, positionIterations;
    float blockLength; //Length of a block
    int obstPerBlock; //Number of each type of obstacles per block
    float stormVelocityY; // Storm objects vertical velocity (moving obstacles)
    float epsilon;
    
    float timeStep;
    
    Cloud cloud;
    b2Vec2 gravity;
    b2World* world;
    int blockIndex;
    int bestScore;
    bool playing;
    bool started;
    vector<unique_ptr<Block>> blockPtrs; //Pointeurs vers les 2 blocks que l'on suit. A chaque création d'un block, on supprime le premier de la liste et on rajoute le nouveau à la fin
    stringstream score;
    sf::Text sfScore;
    sf::Text sfGameOver;
    sf::Text sfPause;
    sf::Font font;
    
    
public:
    Game();
    
    void setStep(sf::Time elapsed);
    
    void jumpCloud();
    
    void SetGravity(b2Vec2 gravity);
    
    void draw(sf::RenderWindow& window);
    
    void checkGameOver();
    
    bool updateBlocks();
    
    pair<float, float> center();
    
    void restart();
    
    void playPause();
    
    void newCircle(float X, float Y);
    
    void saveCloudConfiguration();
    
    void loadCloudConfiguration(string name);
    
    bool getPlaying();
    
    bool getStarted();
    
    bool setStarted(bool b);
    
    pair<float, float> getCloudPosition();
    
    float getTimeStep();

};

#endif /* game_hpp */
