//
//  rainbow.cpp
//  FlappyCloud


#include "rainbow.hpp"


Rainbow::Rainbow(b2World& world, float X, float Y){
    bodyDef.position = b2Vec2(X/SCALE, Y/SCALE);
    bodyDef.type = b2_staticBody;
    body = world.CreateBody(&bodyDef);
    shape.m_radius=radius;
    fixtureDef.shape=&shape;
    fixtureDef.isSensor=true;
    body->CreateFixture(&fixtureDef);

};

Rainbow::Rainbow() {
};

Rainbow::~Rainbow(){
};


void Rainbow::draw(sf::RenderWindow& window){
    sfCircle.setRadius(SCALE*radius);
    sfCircle.setPosition( (body->GetPosition().x - radius) * SCALE,(body->GetPosition().y - radius) * SCALE);
    sfCircle.setFillColor(sf::Color::White);
    window.draw(sfCircle);
};
