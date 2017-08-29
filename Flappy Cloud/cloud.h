//
//  cloud.h
//  FlappyCloud


#ifndef cloud_h
#define cloud_h


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

using namespace std;
using namespace pugi;

#include "globals.h"
#include "basic_functions.h"

/*Cloud est la classe de notre nuage, instanciée à chaque nouveau jeu*/
class Cloud {
private:
    gameEntityType gEntityType = cloud;
    b2BodyDef bodyDef;
    b2Body* body;
    sf::Sprite sprite;
    bool inCollision; // lorsque body rentre en contact avec un autre body, le contact peut durer plusieurs frames. Ce booléen inCollision sera VRAI lorsqu'il y aura eu contact à la dernière frame afin de permettre d'éviter que le Cloud perde plusieurs vies pour une même collision.
    bool dead;
    int lives;
//    int compteur;
    float velocityY, velocityX;
    float scoreCoeff;
    
    vector<pair<sf::CircleShape, pair<float, float>>> sfCircles; //vecteur dont chaque élement est une paire contenant first-le sf::CircleShape et en second une paire de coordonnées
public:
    Cloud(b2World& world, float velocityX, float velocityY, float scoreCoeff);
    Cloud();
    void jump();
    void draw(sf::RenderWindow& window);
    float getPositionX();
    float getPositionY();
    void damage();
    float getScore();
    bool isDead();
    bool checkCollision();
    void newCircle(float X, float Y);
    void saveCloudConfiguration();
    void loadCloudConfiguration(string name);
    bool checkValidCircle(float X, float Y);

    string getGameEntityType();
};

#endif /* cloud_h */
