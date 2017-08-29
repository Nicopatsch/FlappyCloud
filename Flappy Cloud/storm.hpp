//
//  storm.hpp
//  FlappyCloud


#ifndef storm_hpp
#define storm_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <vector>
#include "iostream"
#include <sstream>
#include <memory>
#include <random>

#include "globals.h"
#include "obstacle.hpp"
#include "basic_functions.h"

/*Les Storms sont un type d'Obstacles. Leur particularité
 est qu'ils translatent aléatoirement de haut en bas*/
class Storm: public Obstacle {
private:
    gameEntityType gEntityType = storm;
    /*Variables privées propres à Storm permettant de
     mettre des limites au déplacement de l'obstacle*/
    float topLimit;
    float bottomLimit;
    float savedSpeed;
    float stormVelocityY;
    
public:
    Storm(b2World& world, float X, float Y, float stormVelocityY);
    
    void updateVelocity();
    
    string getGameEntityType();

};



#endif /* storm_hpp */
