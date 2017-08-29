//
//  obstacle.cpp
//  FlappyCloud


#include "obstacle.hpp"

/*Les obstacles sont de 2 types (classes dérivées)*/

Obstacle::Obstacle(b2World& world, float X, float Y, float width, float height) {
    bodyDef.position = b2Vec2(X/SCALE, Y/SCALE);
    bodyDef.type = b2_kinematicBody;
    bodyDef.userData = this;
    body = world.CreateBody(&bodyDef);
    shape.SetAsBox((width/2)/SCALE, (height/2)/SCALE);
//    fixtureDef.density = 0.f;
    fixtureDef.restitution = 2.0f;
    fixtureDef.friction = 0.f;
    fixtureDef.shape = &shape;
    body->CreateFixture(&fixtureDef);
    
    this->height = (height/2)/SCALE;
    this->width=(width/2)/SCALE;
//    cout<<"Obstacle "<<this<<" instancié"<<endl;
    extern int nbObstacles;
    nbObstacles++;
}

Obstacle::~Obstacle() {
//    cout<<"Obstacle "<<this<<" détruit"<<endl;
    extern int nbObstacles;
    nbObstacles--;
}

void Obstacle::draw(sf::RenderWindow& window) {
    sprite.setTexture(texture);
    sprite.setPosition(body->GetPosition().x * SCALE, body->GetPosition().y * SCALE);
    sprite.setRotation(180/b2_pi * body->GetAngle());
    window.draw(sprite);
}
