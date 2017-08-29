//
//  ground.cpp
//  FlappyCloud


#include "ground.hpp"

Ground::Ground(b2World& world, float X, float blockLength) {
    /*Selon la valeur de blockLength (modifiable dans data.xml)
     Les blocks, grounds et ceillings s'adapteront et adapteront leur texture.*/
    this->blockLength = blockLength;
    bodyDef.position = b2Vec2(X/SCALE, 500.f/SCALE);
    bodyDef.userData = this;
    bodyDef.type = b2_staticBody;
    body = world.CreateBody(&bodyDef);
    shape.SetAsBox((blockLength/2)/SCALE, (16.f/2)/SCALE);
    fixtureDef.density = 0.f;
    fixtureDef.friction = 1.f;
    fixtureDef.restitution = 1;
    fixtureDef.shape = &shape;
    body->CreateFixture(&fixtureDef);
    groundTexture.loadFromFile(resourcePath() + "ground.png");
    //        if(((int)(X/blockLength-0.5))%2 == 0) {
    //            groundTexture.loadFromFile(resourcePath() + "ground.png");
    //        } else {
    //            groundTexture.loadFromFile(resourcePath() + "ground2.png");
    //        }
    groundTexture.setRepeated(true);
    extern int nbGrounds;
    
    nbGrounds++;
}

Ground::Ground() {
    extern int nbGrounds;
    cout << "nbGrounds: " << nbGrounds << endl;
    nbGrounds++;
}

Ground::~Ground() {
    extern int nbGrounds;
    
    nbGrounds--;
}

void Ground::draw(sf::RenderWindow& window) {
    sprite.setTexture(groundTexture);
    sprite.setOrigin(blockLength/2, 8.f);
    sprite.setPosition(body->GetPosition().x * SCALE, body->GetPosition().y * SCALE);
    sprite.setTextureRect(sf::IntRect(0,0,blockLength,128.f));
    sprite.setRotation(180/b2_pi * body->GetAngle());
    window.draw(sprite);
}

gameEntityType Ground::getGameEntityType() {
    return gEntityType;
}
