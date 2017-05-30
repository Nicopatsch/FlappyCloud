#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <sstream>


// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"
#include "cloud.cpp"
#include "pugixml.hpp"
#include "pugiconfig.hpp"

using namespace std;
using namespace pugi;

void loadVariables();

static float gravityX, gravityY;
static float velocityY, velocityX;
static float scoreCoeff;
static float timeStep, velocityIterations, positionIterations;

enum obstacleType {storm, tornado};

class Cloud {
private:
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    sf::Texture cloudTexture;
    sf::Sprite sprite;
    string scoreText;
    stringstream streamScoreText;
    sf::Font font;
    sf::Text sfScore;
public:
    Cloud(b2World& world)
    {
        cloudTexture.loadFromFile(resourcePath() + "cloud.png");
        bodyDef.position = b2Vec2(10, 10);
        bodyDef.type = b2_dynamicBody;
        body = world.CreateBody(&bodyDef);
        shape.SetAsBox((100.f/2)/SCALE, (67.f/2)/SCALE);
        fixtureDef.density = 1.f;
        fixtureDef.friction = 0.f;
        fixtureDef.restitution = 0.1f;
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
        body->SetLinearVelocity(b2Vec2(velocityX, 0));
        body->SetFixedRotation(true);
        
        //Prepare the sfml score text
        if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
            return EXIT_FAILURE;
        }
        sfScore = sf::Text(scoreText, font, 50);
        sfScore.setFillColor(sf::Color::Black);
    }
    
    void jump() {
        body->SetLinearVelocity(b2Vec2(velocityX,velocityY));
    }
    
    void draw(sf::RenderWindow& window) {
        sprite.setTexture(cloudTexture);
        sprite.setOrigin(16.f, 16.f);
        sprite.setPosition(SCALE * body->GetPosition().x, SCALE * body->GetPosition().y);
        sprite.setRotation(body->GetAngle() * 180/b2_pi);
        window.draw(sprite);
    }
    
    float getPositionX() {
        return body->GetPosition().x;
    }
    
    void kill() {
        body->SetTransform(b2Vec2(body->GetPosition().x, body->GetPosition().y), b2_pi);
        body->SetLinearVelocity(b2Vec2(0,0));
        body->SetType(b2_staticBody);
        cout << "Cloud dead --> Game over!" <<endl;
    }
    
    float getScore() {
        return round(body->GetPosition().x/scoreCoeff);
    }
    
    void drawScore(sf::RenderWindow& window) {
        //Re-creating and updating the text in the sfml text object
        streamScoreText << "Score: " << round(body->GetPosition().x/scoreCoeff) << " points";
        scoreText = streamScoreText.str();
        sfScore.setString(scoreText);
        window.draw(sfScore);
    }

};



class Ground {
private:
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    sf::Texture groundTexture;
    sf::Sprite sprite;
public:
    Ground(b2World& world, float X, float Y) {
        bodyDef.position = b2Vec2(X/SCALE, Y/SCALE);
        bodyDef.type = b2_staticBody;
        body = world.CreateBody(&bodyDef);
        shape.SetAsBox((8000.f/2)/SCALE, (16.f/2)/SCALE);
        fixtureDef.density = 0.f;
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
        groundTexture.loadFromFile(resourcePath() + "ground.png");
    }
    
    void draw(sf::RenderWindow& window) {
        sprite.setTexture(groundTexture);
        sprite.setOrigin(400.f, 8.f);
        sprite.setPosition(body->GetPosition().x * SCALE, body->GetPosition().y * SCALE);
        sprite.setRotation(180/b2_pi * body->GetAngle());
        window.draw(sprite);
    }
};


class Obstacle {
private:
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    sf::Texture texture;
    sf::Sprite sprite;
public:
    Obstacle(b2World& world, float X, float Y, float width, float height, obstacleType type) {
        bodyDef.position = b2Vec2(X/SCALE, Y/SCALE);
        bodyDef.type = b2_staticBody;
        body = world.CreateBody(&bodyDef);
        shape.SetAsBox((width/2)/SCALE, (height/2)/SCALE);
        fixtureDef.density = 0.f;
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
        if(type == obstacleType::storm) {
            texture.loadFromFile(resourcePath() + "storm.png");
            sprite.setOrigin(50.f, 50.f);
        } else if(type == obstacleType::tornado) {
            texture.loadFromFile(resourcePath() + "tornado.png");
            sprite.setOrigin(50.f, 74.5f);
        }
    }
    
    void draw(sf::RenderWindow& window) {
        sprite.setTexture(texture);
        sprite.setPosition(body->GetPosition().x * SCALE, body->GetPosition().y * SCALE);
        sprite.setRotation(180/b2_pi * body->GetAngle());
        window.draw(sprite);
        
    }
};


class Storm: public Obstacle {
private:
    
public:
    Storm(b2World& world, float X, float Y): Obstacle(world, X, Y,  100.f, 100.f, obstacleType::storm) {
    }
};


class Tornado: public Obstacle {
private:
    
public:
    Tornado(b2World& world, float X, float Y): Obstacle(world, X, Y,  100.f, 149.f, obstacleType::tornado) {
    }
};


int main(int, char const**)
{
    loadVariables();
    
    /** Prepare the world */
    b2Vec2 gravity(gravityX, gravityY);
    b2World world(gravity);
    Ground ground = Ground(world, 400.f, 500.f);
    Cloud cloud = Cloud(world);
    vector<Obstacle> obstacles = vector<Obstacle>();
    for (int i=0 ; i<100 ; i++) {
        obstacles.push_back(Storm(world, i*1000, 100));
        obstacles.push_back(Tornado(world, (i+0.5)*1000, 400));
    }
    
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    sf::View view(sf::FloatRect(0, 0, 800, 600));
    window.setView(view);
    
    
    // Clear screen
    window.clear();
    
    
    
    

    
    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                cloud.jump();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::K) {
                cloud.kill();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
                loadVariables();
                b2Vec2 gravity(gravityX, gravityY);
                world.SetGravity(gravity);
            }
            
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                int mouseX = sf::Mouse::getPosition(window).x;
                int mouseY = sf::Mouse::getPosition(window).y;
                
            }
        }
        
        world.Step(timeStep, velocityIterations, positionIterations);
        
        window.clear(sf::Color::White);
        int bodyCount = 0;
        cloud.draw(window);
        ground.draw(window);
        cloud.drawScore(window);
        for (auto obs = obstacles.begin() ; obs<obstacles.end(); obs++) {
            obs->draw(window);
        }
        
        
        
        // Update the window
        view.setCenter(SCALE*cloud.getPositionX(), 300);
        window.setView(view);
        window.display();
    }
    
    return EXIT_SUCCESS;
}


void createObstacle(b2World& world, float X, float Y)
{
    b2BodyDef bodyDef;
    bodyDef.position = b2Vec2(850, 100);
    bodyDef.position = b2Vec2(X/SCALE, Y/SCALE);
    bodyDef.type = b2_staticBody;
    b2Body* body = world.CreateBody(&bodyDef);
    
    b2PolygonShape shape;
    shape.SetAsBox((16.f/2)/SCALE, (16.f/2)/SCALE);
    b2FixtureDef fixtureDef;
    fixtureDef.density = 0.f;
    fixtureDef.shape = &shape;
    body->CreateFixture(&fixtureDef);
}



void loadVariables() {
    pugi::xml_document doc;
    
    if (!doc.load_file("../../../../../../../../FlappyCloud/Flappy Cloud/parameters.xml")) cout << "Failed loading file" << endl;
    
    pugi::xml_node parameters = doc.child("Parameters");
    
    gravityX=stof(parameters.child("Gravity").attribute("GravityX").value());
    gravityY=stof(parameters.child("Gravity").attribute("GravityY").value());
    velocityX=stof(parameters.child("Velocity").attribute("VelocityX").value());
    velocityY=stof(parameters.child("Velocity").attribute("VelocityY").value());
    scoreCoeff=stof(parameters.child("Score").attribute("ScoreCoeff").value());
    timeStep=stof(parameters.child("Iterations").attribute("TimeStep").value());
    velocityIterations=stof(parameters.child("Iterations").attribute("VelocityIterations").value());
    positionIterations=stof(parameters.child("Iterations").attribute("PositionIterations").value());
}
