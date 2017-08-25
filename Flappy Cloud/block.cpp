//
//  block.cpp
//  FlappyCloud
//
//  Created by Olivier Freyssinet on 25/08/2017.
//  Copyright © 2017 Appdea. All rights reserved.
//

#include "block.hpp"


Block::Block(b2World& world, int N, float stormVY, int obstPerBlock, float blockLength){
    if(N!=-1){
        for (int i=0 ; i<obstPerBlock ; i++) {
            storms.push_back(Storm(world, N*blockLength+i*blockLength/obstPerBlock, 100, stormVY)); //Storms espacés de 1000m
            tornadoes.push_back(Tornado(world, N*blockLength+(i+0.5)*blockLength/obstPerBlock, 400));
        }
    }
    this->blockLength = blockLength;
    this->N = N;
    ground = Ground(world, (N+0.5)*blockLength, blockLength);
    ceilling = Ceilling(world, (N+0.5)*blockLength, blockLength);
    extern int nbBlocks;
    nbBlocks+=1;
    cout<<"Block "<<N<<" instancié"<<endl;
}

Block::Block() {
    cout<<"Block "<<N<<" créé"<<endl;
}

Block::~Block() {
    cout<<"Block "<<N<<" détruit"<<endl;
    extern int nbBlocks;
    nbBlocks--;
}

void Block::draw(sf::RenderWindow& window) {
    ground.draw(window);
    
    for (auto sto = storms.begin() ; sto<storms.end(); sto++) {
        sto->updateVelocity();
        sto->draw(window);
    }
    for (auto tor = tornadoes.begin() ; tor<tornadoes.end(); tor++) {
        tor->draw(window);
    }
    ceilling.draw(window);
}


float Block::getPositionX() {
    return (N+0.5)*blockLength;
}

int Block::getIndex() {
    return N;
}

int Block::getNbBlocks() {
    extern int nbBlocks;
    return nbBlocks;
}

void Block::play() {
    for (auto sto = storms.begin() ; sto<storms.end(); sto++) {
        sto->play();
    }
}

void Block::pause() {
    for (auto sto = storms.begin() ; sto<storms.end(); sto++) {
        sto->pause();
    }
}

