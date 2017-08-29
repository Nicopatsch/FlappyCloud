//
//  rainbow.cpp
//  FlappyCloud


#include "rainbow.hpp"


Rainbow::Rainbow(b2World& world, Cloud* cloud, float X, float Y){
    this->X = X;
    this->Y = Y;
    bodyDef.position = b2Vec2(X/SCALE, Y/SCALE);
    bodyDef.type = b2_staticBody;
    body = world.CreateBody(&bodyDef);
    shape.m_radius=radius;
    fixtureDef.shape=&shape;
    fixtureDef.isSensor=true;
    body->CreateFixture(&fixtureDef);
    this->cloud=cloud;

};

Rainbow::Rainbow() {
};

Rainbow::~Rainbow(){
};

void Rainbow::checkCollisionWithCloud(){
    
};

void Rainbow::draw(sf::RenderWindow& window){
    sfCircle.setRadius(SCALE*radius);
    sfCircle.setPosition(X-radius*SCALE, Y-radius*SCALE);
    sfCircle.setFillColor(sf::Color::White);
    window.draw(sfCircle);
};
