//
//  contactListener.hpp
//  FlappyCloud
//
//  Created by Olivier Freyssinet on 29/08/2017.
//  Copyright © 2017 Appdea. All rights reserved.
//

#ifndef contactListener_hpp
#define contactListener_hpp

#include <stdio.h>
#include <Box2D/Box2D.h>
#include "game.hpp"


class ContactListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
};




#endif /* contactListener_hpp */
