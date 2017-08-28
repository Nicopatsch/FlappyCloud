//
//  cloud.cpp
//  FlappyCloud


#include "cloud.h"


static float circleRadius = .5;

Cloud::Cloud(b2World& world, float velocityX, float velocityY, float scoreCoeff)
{
    this->velocityX = velocityX;
    this->velocityY = velocityY;
    this->scoreCoeff = scoreCoeff;
    
    /*Création du body Box2D*/
    bodyDef.position = b2Vec2((-1000)/SCALE, 200/SCALE);
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation =true;
    body = world.CreateBody(&bodyDef);
    body->SetLinearVelocity(b2Vec2(velocityX, 0));
    
    /*Création du premier cercle Box2D*/
    b2CircleShape circleShape;
    circleShape.m_p.Set(0, 0); //position, relative to body position
    circleShape.m_radius = circleRadius;
    body->CreateFixture(&circleShape, 1);
    
    /*Création du premier cercle SFML*/
    sf::CircleShape sfCircleShape;
    sfCircleShape.setRadius(SCALE*circleRadius);
    pair<float, float> coordinates = pair<float, float>(0, 0);
    pair<sf::CircleShape, pair<float, float>> circle = pair<sf::CircleShape, pair<float, float>>(sfCircleShape, coordinates);
    sfCircles.push_back(circle);
    
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

/*Fonction appelée lors de la mort du nuage.
 Il s'arrête et se retourne sur le dos*/
void Cloud::kill() {
    if(lives==1) {
        dead=true;
        //            cout <<"cloud is dead" << endl;
        body->SetTransform(b2Vec2(body->GetPosition().x, body->GetPosition().y), 0);
        body->SetLinearVelocity(b2Vec2(0,0));
        body->SetType(b2_staticBody);
    } else if(lives > 1) {
        body->SetTransform(b2Vec2(body->GetPosition().x+10., 200./SCALE), 0);
        body->SetLinearVelocity(b2Vec2(velocityX,0));
        sfCircles.pop_back();
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

bool Cloud::checkDead() {
    if (body->GetLinearVelocity().x < velocityX) {
        this->kill();
        return true;
    }
    return false;
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
    
    lives++;
    
}

void Cloud::saveCloudConfiguration() {
    pugi::xml_document doc;
    if (!doc.load_file("saved.xml")) cout << "Failed loading file" << endl;
    pugi::xml_node clouds = doc.child("Saved").child("Clouds");
    
    /*Preparing the CloudConfiguration node*/
    clouds.append_child("CloudConfiguration");
    clouds.last_child().append_attribute("type").set_value("custom");
    
    for (auto c=sfCircles.begin() ; c<sfCircles.end() ; c++) {
        clouds.last_child().append_child("Center");
        clouds.last_child().last_child().append_attribute("X").set_value(to_string(c->second.first).c_str());
        clouds.last_child().last_child().append_attribute("Y").set_value(to_string(c->second.second).c_str());
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
    }
    
    doc.save_file("saved.xml");
}


/*** PROBLEME DE SCALE ***/
bool Cloud::checkValidCircle(float X, float Y) {
    pair<float, float> coordinates = pair<float, float>(-X, Y);
    bool valid1 = false;
    bool valid2 = true;
    auto c = sfCircles.begin();
    
    /*Checking that the new circle is not too far from the others*/
    while(!valid1 && c<sfCircles.end()) {
        valid1 = (distanceBetween(c->second, coordinates) < 1.75*circleRadius);
        c++;
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
    return (!valid1 && valid2);
    //return false;
}
