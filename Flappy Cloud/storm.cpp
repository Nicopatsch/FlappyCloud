//
//  storm.cpp
//  FlappyCloud
//
//  Created by Olivier Freyssinet on 25/08/2017.
//  Copyright © 2017 Appdea. All rights reserved.
//

#include "storm.hpp"


Storm::Storm(b2World& world, float X, float Y, float stormVelocityY): Obstacle(world, X, Y,  100.f, 100.f) {
    this->stormVelocityY=stormVelocityY;
    
    texture.loadFromFile(resourcePath() + "storm.png");
    sprite.setOrigin(50.f, 50.f);
    
    body->SetLinearVelocity(b2Vec2(0, randomIntBetween(-stormVelocityY ,stormVelocityY)));
    
    topLimit = randomIntBetween(215, 400);
    bottomLimit = randomIntBetween(35, 215);
    
}

void Storm::updateVelocity() {
    if(playing) {
        /*If between topLimit and bottomLimit, keep the same speed*/
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
    else {
        body->SetLinearVelocity(b2Vec2(0, 0));
    }
}

void Storm::pause() {
    savedSpeed = body->GetLinearVelocity().y;
    body->SetType(b2_staticBody);
    playing = false;
}

void Storm::play() {
    body->SetType(b2_dynamicBody);
    body->SetLinearVelocity(b2Vec2(0, savedSpeed));
    playing = true;
}
