#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <vector>
#include "iostream"
#include <sstream>
#include <memory>
#include <random>

#include "ResourcePath.hpp"
#include "pugixml.hpp"
#include "pugiconfig.hpp"

#include "globals.h"
#include "basic_functions.h"
#include "cloud.h"
#include "ground.hpp"
#include "ceilling.hpp"
#include "obstacle.hpp"
#include "storm.hpp"
#include "tornado.hpp"
#include "block.hpp"
#include "game.hpp"



using namespace std;
using namespace pugi;


int main(int, char const**)
{
    
    /*Initialize new game*/
    Game game = Game();

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    sf::View view(sf::FloatRect(0, 0, 800, 600));
    window.setFramerateLimit(1/game.timeStep);
    window.setView(view);


    // Clear screen
    window.clear();

    sf::Clock clock;
    // Start the game loop
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        if (game.getPlaying()) {
            game.setStep(elapsed);
            game.updateBlocks();
            game.checkGameOver();
        }

        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                game.newCircle((event.mouseButton.x - 300)/SCALE, (event.mouseButton.y - game.getCloudPosition().second*SCALE)/SCALE);
                
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                if (game.getStarted()){
                    game.jumpCloud();
                }
                }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                cout << "building new game" << endl;
                game.setStarted(false);
                game.~Game();
                new (&game) Game();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                game.playPause();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N) {
                game.newCircle(1,1);
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                game.saveCloudConfiguration();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L) {
                if (!game.getStarted()) {
                    cout << "loading square" << endl;
                    game.loadCloudConfiguration("square");
                }
            }
            
        }


        window.clear(sf::Color(119, 185, 246));
        game.draw(window);
        
        
        // Update the window
        view.setCenter(game.center().first, game.center().second);
        window.setView(view);
        window.display();
    }

    return EXIT_SUCCESS;
}



