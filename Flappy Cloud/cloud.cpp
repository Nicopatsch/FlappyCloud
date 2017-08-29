//
//  cloud.cpp
//  FlappyCloud


#include "cloud.h"
#include "storm.hpp"


static float circleRadius = .5;

Cloud::Cloud(b2World& world, float velocityX, float velocityY, float scoreCoeff)
{
    this->velocityX = velocityX;
    this->velocityY = velocityY;
    this->scoreCoeff = scoreCoeff;
    
    /*Création du body Box2D*/
    bodyDef.position = b2Vec2((-1000)/SCALE, 200/SCALE);
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    bodyDef.userData = this;
    body = world.CreateBody(&bodyDef);
    body->SetLinearVelocity(b2Vec2(velocityX, 0));
    
    newCircle(0, 0);
    circles.push_back(pair<int, int>(0,0));
    
    dead = false;
    lives = 1;
}

Cloud::Cloud() {
    //Constructeur par défaut (nécessaire de le déclarer, même vide)
}

void Cloud::jump() {
    body->SetLinearVelocity(b2Vec2(velocityX,velocityY));
}

void Cloud::draw(sf::RenderWindow& window) {
    
    //Limiting the velocity to velocityX and velocityY
    if(abs(body->GetLinearVelocity().x) > velocityX) {
        body->SetLinearVelocity(b2Vec2(velocityX * (1-2*(body->GetLinearVelocity().x<0)),body->GetLinearVelocity().y));
    }
    if(abs(body->GetLinearVelocity().y) > 2 * abs(velocityY)) {
        body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, 2 * abs(velocityY) * (1-2*(body->GetLinearVelocity().y<0))));
    }
    
    for(auto c = sfCircles.begin() ; c < sfCircles.end() ; c++) {
        c->first.setPosition(SCALE * (body->GetPosition().x - circleRadius - c->second.first), SCALE * (body->GetPosition().y - circleRadius + c->second.second));
        window.draw(c->first);
    }
}

float Cloud::getPositionX() {
    return body->GetPosition().x;
}

float Cloud::getPositionY() {
    return body->GetPosition().y;
}

/*Enlève une vie ou tue le nuage*/
void Cloud::damage() {
    if(lives==1) {
        dead=true;
        body->SetTransform(b2Vec2(body->GetPosition().x, body->GetPosition().y), 0);
        body->SetLinearVelocity(b2Vec2(0,0));
        body->SetType(b2_staticBody);
        lives = 0;
    } else if(lives > 1) {
        //        body->SetTransform(b2Vec2(body->GetPosition().x+10., 200./SCALE), 0);
        //        body->SetLinearVelocity(b2Vec2(velocityX,0));
        sfCircles.pop_back();
        b2Fixture fixtureList = *body->GetFixtureList();
        while(fixtureList.GetNext()) {
            fixtureList = *fixtureList.GetNext();
            cout << &fixtureList << endl;
        }
        fixtureList.~b2Fixture();
        lives--;
    }
}

float Cloud::getScore() {
    /*Le score est proportionnel à la distance parcourue,
     au coefficient scoreCoeff prêt*/
    return round(body->GetPosition().x/scoreCoeff);
}

bool Cloud::isDead() {
    return dead;
}

bool Cloud::checkCollision() {
    for (b2ContactEdge* edge = body->GetContactList(); edge; edge = edge->next) {
        
        if (!inCollision){
            this->damage();
        }
        inCollision = true;
        return true;
    }
    inCollision = false;
    return false;

//    if (body->GetLinearVelocity().x < velocityX) {
//        this->kill();
//        return true;
//    }
//    return false;
}


void Cloud::newCircle(float X, float Y) {
    /*Création du cercle Box2D*/
    b2CircleShape circleShape;
    circleShape.m_p.Set(X, Y); //position, relative to body position
    circleShape.m_radius = circleRadius;
    body->CreateFixture(&circleShape, 1);
    /*Création du cercle SFML*/
    pair<float, float> coordinates = pair<float, float>(-X, Y);
    sf::CircleShape sfCircleShape;
    sfCircleShape.setRadius(SCALE*circleRadius);
    pair<sf::CircleShape, pair<float, float>> circle = pair<sf::CircleShape, pair<float, float>>(sfCircleShape, coordinates);
    sfCircles.push_back(circle);
    
    circles.push_back(pair<int, int>(X,Y));
    
    lives++;
    
}

void Cloud::saveCloudConfiguration() {
    pugi::xml_document doc;
    if (!doc.load_file("saved.xml")) cout << "Failed loading file" << endl;
    pugi::xml_node clouds = doc.child("Saved").child("Clouds");
    
    /*Preparing the CloudConfiguration node*/
    clouds.append_child("CloudConfiguration");
    clouds.last_child().append_attribute("type").set_value("custom");
    
    for (auto c=circles.begin() ; c<circles.end() ; c++) {
        clouds.last_child().append_child("Center");
        clouds.last_child().last_child().append_attribute("X").set_value(to_string(c->first).c_str());
        clouds.last_child().last_child().append_attribute("Y").set_value(to_string(c->second).c_str());
    }
    
    doc.save_file("saved.xml");
}


void Cloud::loadCloudConfiguration(string name) {
    pugi::xml_document doc;
    if (!doc.load_file("saved.xml")) cout << "Failed loading file" << endl;
    pugi::xml_node clouds = doc.child("Saved").child("Clouds");
    
    pugi::xml_node cloudConfig = clouds.find_child_by_attribute("CloudConfiguration", "name", name.c_str());
    
    /*Deleting previously written circles*/
    sfCircles.clear();
    lives = 0;
    
    pair<float, float> coordinates;
    for (pugi::xml_node center: cloudConfig.children()) {
        this->newCircle(stof(center.attribute("X").value()), stof(center.attribute("Y").value()));
        circles.push_back(pair<int, int>(stof(center.attribute("X").value()),stof(center.attribute("Y").value())));
    }
    
    doc.save_file("saved.xml");
}


bool Cloud::checkValidCircle(float X, float Y) {
    pair<float, float> coordinates = pair<float, float>(-X, Y);
    bool valid1 = false;
    bool valid2 = true;
    auto c = sfCircles.begin();
    
    /*Checking that the new circle is not too far from the others*/
    while(!valid1 && c<sfCircles.end()) {
        valid1 = (distanceBetween(c->second, coordinates) < 1.75*circleRadius);
        c++;
        cout << "circle checked:" << c->second.first << "," << c->second.second <<endl;
        cout << distanceBetween(c->second, coordinates) << endl;
    }
    
    /*Checking that the new circle is not too close to the others*/
    c = sfCircles.begin();
    while(valid2 && c<sfCircles.end()) {
        valid2 = (distanceBetween(c->second, coordinates) > 0.75*circleRadius);
        c++;
    }
    cout << "new circle not too far: " << valid1 << endl;
    cout << "new circle not too close: " << valid2 << endl;
    return (valid1 && valid2);
}

string Cloud::getGameEntityType() {
//    return gEntityType;
    return "I'm a Cloud";

}

int Cloud::getLives() {
    return this->lives;
}

bool Cloud::addLife() {
    if (lives<10) {
        if (circles[lives].first!=0 || circles[lives].second!=0) {
            /*Création du cercle Box2D*/
            b2CircleShape circleShape;
            circleShape.m_p.Set(circles[lives].first, circles[lives].second); //position, relative to body position
            circleShape.m_radius = circleRadius;
            body->CreateFixture(&circleShape, 1);
            /*Création du cercle SFML*/
            pair<float, float> coordinates = pair<float, float>(-circles[lives].first, circles[lives].second);
            sf::CircleShape sfCircleShape;
            sfCircleShape.setRadius(SCALE*circleRadius);
            pair<sf::CircleShape, pair<float, float>> circle = pair<sf::CircleShape, pair<float, float>>(sfCircleShape, coordinates);
            sfCircles.push_back(circle);

        }
    }
}

