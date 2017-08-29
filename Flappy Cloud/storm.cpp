//
//  storm.cpp
//  FlappyCloud


#include "storm.hpp"


Storm::Storm(b2World& world, float X, float Y, float stormVelocityY): Obstacle(world, X, Y,  100.f, 100.f) {
    this->stormVelocityY=stormVelocityY;
    
    texture.loadFromFile(resourcePath() + "storm.png");
    sprite.setOrigin(50.f, 50.f);
    
    body->SetLinearVelocity(b2Vec2(0, randomIntBetween(-stormVelocityY ,stormVelocityY)));
    
    topLimit = randomIntBetween(225, 400);
    bottomLimit = randomIntBetween(50, 225);
    
}

void Storm::updateVelocity() {
    if(body->GetPosition().y < topLimit/SCALE && body->GetPosition().y > bottomLimit/SCALE) {
        if(body->GetLinearVelocity().y > 0) {
            body->SetLinearVelocity(b2Vec2(0, stormVelocityY));
        }
        else {
            body->SetLinearVelocity(b2Vec2(0, -stormVelocityY));
        }
    }
    else if(body->GetPosition().y > topLimit/SCALE) {
        body->SetLinearVelocity(b2Vec2(0, -stormVelocityY));
    }
    else {
        body->SetLinearVelocity(b2Vec2(0, stormVelocityY));
    }
    if (body->GetLinearVelocity().y > 0) {
        if (body->GetPosition().y > topLimit/SCALE) body->SetLinearVelocity(b2Vec2(0, -stormVelocityY));
    }
    else if (body->GetPosition().y < bottomLimit/SCALE) body->SetLinearVelocity(b2Vec2(0, stormVelocityY));
}

string Storm::getGameEntityType() {
    return "I'm a storm";
}
