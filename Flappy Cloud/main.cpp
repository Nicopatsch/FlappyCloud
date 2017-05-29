#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>


// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"
#include "cloud.cpp"
#include "pugixml.hpp"
#include "pugiconfig.hpp"

using namespace std;
using namespace pugi;


void createGround(b2World& world, float X, float Y);
b2Body* createCloud(b2World& world); // Spawns a box at MouseX, MouseY
void cloudJump(b2Body* cloud);
void loadVariables();

static float gravityX;
static float gravityY;

int main(int, char const**)
{
    loadVariables();
    
    /** Prepare the world */
    b2Vec2 gravity(gravityX, gravityY);
    b2World world(gravity);
    createGround(world, 400.f, 500.f);
    b2Body* cloud = createCloud(world);
    
    sf::Texture groundTexture;
    sf::Texture boxTexture;
    sf::Texture cloudTexture;
    groundTexture.loadFromFile(resourcePath() + "ground.png");
    boxTexture.loadFromFile(resourcePath() + "box.png");
    cloudTexture.loadFromFile(resourcePath() + "cloud.png");
    
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    
    
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
                cloudJump(cloud);
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
        
        world.Step(1/60.f, 8, 3);
        
        window.clear(sf::Color::White);
        int bodyCount = 0;
        
        
        
        for (b2Body* bodyIterator = world.GetBodyList(); bodyIterator != 0; bodyIterator = bodyIterator->GetNext())
        {
            if (bodyIterator->GetType() == b2_dynamicBody)
            {
                sf::Sprite sprite;
                sprite.setTexture(cloudTexture);
                sprite.setOrigin(16.f, 16.f);
                sprite.setPosition(SCALE * bodyIterator->GetPosition().x, SCALE * bodyIterator->GetPosition().y);
                sprite.setRotation(bodyIterator->GetAngle() * 180/b2_pi);
                window.draw(sprite);
            }
            else
            {
                sf::Sprite groundSprite;
                groundSprite.setTexture(groundTexture);
                groundSprite.setOrigin(400.f, 8.f);
                groundSprite.setPosition(bodyIterator->GetPosition().x * SCALE, bodyIterator->GetPosition().y * SCALE);
                groundSprite.setRotation(180/b2_pi * bodyIterator->GetAngle());
                window.draw(groundSprite);
            }
        }
        // Update the window
        window.display();
    }
    
    return EXIT_SUCCESS;
}


void createGround(b2World& world, float X, float Y)
{
    b2BodyDef bodyDef;
    bodyDef.position = b2Vec2(X/30.f, Y/30.f);
    bodyDef.type = b2_staticBody;
    b2Body* body = world.CreateBody(&bodyDef);
    
    b2PolygonShape shape;
    shape.SetAsBox((800.f/2)/SCALE, (16.f/2)/SCALE);
    b2FixtureDef fixtureDef;
    fixtureDef.density = 0.f;
    fixtureDef.shape = &shape;
    body->CreateFixture(&fixtureDef);
    
    
}

void cloudJump(b2Body* cloud) {
    cloud->SetLinearVelocity(b2Vec2(0,-5));
}


b2Body* createCloud(b2World& world)
{
    b2BodyDef bodyDef;
    bodyDef.position = b2Vec2(10, 10);
    bodyDef.type = b2_dynamicBody;
    b2Body* body = world.CreateBody(&bodyDef);
    
    b2PolygonShape shape;
    shape.SetAsBox((100.f/2)/SCALE, (67.f/2)/SCALE);
    //    shape.SetAsBox((80.f)/SCALE, (53.5f)/SCALE);
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.f;
    fixtureDef.friction = 0.7f;
    fixtureDef.restitution = 0.1f;
    fixtureDef.shape = &shape;
    body->CreateFixture(&fixtureDef);
    return body;
}

void loadVariables() {
    pugi::xml_document doc;
    
    if (!doc.load_file("/Users/nicopatsch/FlappyCloud/Flappy Cloud/parameters.xml")) cout << "Failed loading file" << endl;
    
    pugi::xml_node gravityNode = doc.child("Parameters").child("Gravity");
    
    cout << "Gravity: " << doc.child("Parameters").child("Gravity").attribute("GravityX").value() << " ; " << doc.child("Parameters").child("Gravity").attribute("GravityY").value() << endl;
    gravityX = stof(gravityNode.attribute("GravityX").value());
    gravityY = stof(gravityNode.attribute("GravityY").value());
}
