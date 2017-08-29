//
//  contactListener.cpp
//  FlappyCloud

#include "contactListener.hpp"

void ContactListener::BeginContact(b2Contact* contact) {
    cout << "ContactListener::BeginContact" << endl;
//    //check if fixture A was a ball
//    void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
//    if ( bodyUserData ) {
//        Cloud* cloud = static_cast<Cloud*>( bodyUserData );
//        if (cloud) {
//            cout << "fixture A was a cloud" << endl;
//        }
//    }
//    
//    //check if fixture B was a ball
//    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
//    if ( bodyUserData ) {
//        Cloud* cloud = static_cast<Cloud*>( bodyUserData );
//        if (cloud) {
//            cout << "fixture B was a cloud" << endl;
//        }
//    }
}

void ContactListener::EndContact(b2Contact* contact) {
}
