//
//  cloud.cpp
//  Flappy Cloud
//
//  Created by Nicolas Patschkowski on 26/05/2017.
//  Copyright © 2017 Appdea. All rights reserved.
//

#include "cloud.hpp"
#include <Box2D/Box2D.h>
#include <iostream>


using namespace std;

static const float SCALE = 30.f;

class Cloud {
private:
    double position;
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
public:
    Cloud(b2World world) {
        cout << "done1" << endl;
        bodyDef.position = b2Vec2(0, 0);
        bodyDef.type = b2_dynamicBody;
        body = world.CreateBody(&bodyDef);
        cout << "done2" << endl;
        shape.SetAsBox((32.f/2)/SCALE, (32.f/2)/SCALE);
        fixtureDef.density = 1.f;
        fixtureDef.friction = 0.7f;
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
        cout << "done3" << endl;
    }
    
    /*void cloudUp() {
     body->ApplyLinearImpulse(b2Vec2(0,50), body->GetWorldCenter());
     }*/
    
};
