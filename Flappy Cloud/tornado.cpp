//
//  tornado.cpp
//  FlappyCloud


#include "tornado.hpp"


Tornado::Tornado(b2World& world, float X, float Y): Obstacle(world, X, Y,  100.f, 149.f) {
    texture.loadFromFile(resourcePath() + "tornado.png");
    sprite.setOrigin(50.f, 74.5f);
}

void Tornado::updateVelocity() {}
//void Tornado::playPause() {}

