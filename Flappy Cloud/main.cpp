
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
//#include "cloud.cpp"


// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"
#include "cloud.cpp"



void createGround(b2World& world, float X, float Y);
void createBox(b2World& world, int mouseX, int mouseY); // Spawns a box at MouseX, MouseY


int main(int, char const**)
{
    
    /** Prepare the world */
    b2Vec2 gravity(0.f, 9.8f);
    b2World world(gravity);
    createGround(world, 400.f, 500.f);
    //Cloud cloud0 = Cloud(world);
    
    sf::Texture groundTexture;
    sf::Texture boxTexture;
    groundTexture.loadFromFile(resourcePath() + "ground.png");
    boxTexture.loadFromFile(resourcePath() + "box.png");
    
    
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
            
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                int mouseX = sf::Mouse::getPosition(window).x;
                int mouseY = sf::Mouse::getPosition(window).y;
                //Cloud cloud(world); -->en comm car ne marche pas...
                createBox(world, mouseX, mouseY);
                
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
                sprite.setTexture(boxTexture);
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


void createBox(b2World& world, int mouseX, int mouseY)
{
    b2BodyDef bodyDef;
    bodyDef.position = b2Vec2(mouseX/SCALE, mouseY/SCALE);
    bodyDef.type = b2_dynamicBody;
    b2Body* body = world.CreateBody(&bodyDef);
    
    b2PolygonShape shape;
    shape.SetAsBox((32.f/2)/SCALE, (32.f/2)/SCALE);
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.f;
    fixtureDef.friction = 0.7f;
    fixtureDef.shape = &shape;
    body->CreateFixture(&fixtureDef);
}

