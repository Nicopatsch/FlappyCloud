//
//  ceiling.cpp
//  FlappyCloud
//
//  Created by Olivier Freyssinet on 25/08/2017.
//  Copyright © 2017 Appdea. All rights reserved.
//

#include "ceiling.hpp"

Ceilling::Ceilling(b2World& world, float X, float blockLength) {
    this->blockLength = blockLength;
    bodyDef.position = b2Vec2(X/SCALE, -13.f/SCALE);
    bodyDef.type = b2_staticBody;
    body = world.CreateBody(&bodyDef);
    shape.SetAsBox((blockLength/2)/SCALE, (13.f/2)/SCALE);
    fixtureDef.density = 0.f;
    fixtureDef.friction = 0.f;
    fixtureDef.shape = &shape;
    body->CreateFixture(&fixtureDef);
    ceillingTexture.loadFromFile(resourcePath() + "ceilling.png");
    ceillingTexture.setRepeated(true);
    extern int nbCeillings;
    nbCeillings++;
}

Ceilling::Ceilling() {
    extern int nbCeillings;
    nbCeillings++;
}

Ceilling::~Ceilling() {
    extern int nbCeillings;
    nbCeillings--;
}

void Ceilling::draw(sf::RenderWindow& window) {
    sprite.setTexture(ceillingTexture);
    sprite.setOrigin(blockLength/2, -13.f);
    sprite.setPosition(body->GetPosition().x * SCALE, body->GetPosition().y * SCALE);
    sprite.setTextureRect(sf::IntRect(0,0,blockLength,13.f));
    sprite.setRotation(180/b2_pi * body->GetAngle());
    window.draw(sprite);
}
