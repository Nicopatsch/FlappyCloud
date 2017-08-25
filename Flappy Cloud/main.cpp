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


using namespace std;
using namespace pugi;


class Game{
private:
    //parametres de jeu
    float gravityX, gravityY;
    float velocityY, velocityX;
    float scoreCoeff;
    float velocityIterations, positionIterations;
    float blockLength; //Length of a block
    int obstPerBlock; //Number of each type of obstacles per block
    float stormVelocityY; // Storm objects vertical velocity (moving obstacles)
    float epsilon;

    
    
    Cloud cloud;
    b2Vec2 gravity;
    b2World* world;
    int blockIndex;
    int bestScore;
    bool playing;
    vector<unique_ptr<Block>> blockPtrs; //Pointeurs vers les 2 blocks que l'on suit. A chaque création d'un block, on supprime le premier de la liste et on rajoute le nouveau à la fin
    stringstream score;
    sf::Text sfScore;
    sf::Text sfGameOver;
    sf::Font font;
    
    
public:
    Game() {
        /**** Load game parameters ****/
        pugi::xml_document doc;
        
        if (!doc.load_file("data.xml")) cout << "(Loading game parameters) Failed loading file from path '" << (resourcePath()+"data.xml").c_str() << "'" << endl;
        
        pugi::xml_node parameters = doc.child("Parameters");
        pugi::xml_node saved = doc.child("Saved");
        
        cout << parameters.child("Gravity").attribute("GravityY").value() << endl;
        
        /* paramètres pour Game */
        gravityX=stof(parameters.child("Gravity").attribute("GravityX").value());
        gravityY=stof(parameters.child("Gravity").attribute("GravityY").value());
        velocityIterations=stof(parameters.child("Iterations").attribute("VelocityIterations").value());
        positionIterations=stof(parameters.child("Iterations").attribute("PositionIterations").value());
        
        /* paramètres pour Cloud */
        velocityX=stof(parameters.child("Velocity").attribute("VelocityX").value());
        velocityY=stof(parameters.child("Velocity").attribute("VelocityY").value());
        scoreCoeff=stof(saved.child("Score").attribute("ScoreCoeff").value());

        /* paramètres pour Storm */
        stormVelocityY=stof(parameters.child("StormVelocity").attribute("StormVelocityY").value());
        
        /* paramètres pour le main */
        timeStep=stof(parameters.child("Iterations").attribute("TimeStep").value());
        
        /* paramètres pour Block */
        obstPerBlock=stoi(parameters.child("Blocks").attribute("ObstPerBlock").value());
        
        /* paramètre blockLength: utilisé par Ceiling, Block et Ground */
        blockLength=stof(parameters.child("Blocks").attribute("BlockLength").value());
        
        /** Prepare the world */
        gravity = b2Vec2(gravityX, gravityY);
        world = new b2World(gravity);
        cloud = Cloud(*world, velocityX, velocityY, scoreCoeff);
        blockIndex = 1;
        
        /*Initialisation du premier triplet de blocks*/
        blockPtrs.push_back(make_unique<Block>(*world, -1, stormVelocityY, obstPerBlock, blockLength));
        blockPtrs.push_back(make_unique<Block>(*world, 0, stormVelocityY, obstPerBlock,blockLength));
        blockPtrs.push_back(make_unique<Block>(*world, 1, stormVelocityY, obstPerBlock, blockLength));
        
        //Prepare the sfml score text
        if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
            return EXIT_FAILURE;
        }
        sfScore = sf::Text(score.str(), font, 50);
        sfScore.setFillColor(sf::Color::White);
        sfGameOver = sf::Text("Game Over!", font, 50);
        sfGameOver.setFillColor(sf::Color::White);
        
        cout << "New game built" << endl;
        
        playing = false;
    }
    
    float timeStep;
    
    void setStep(sf::Time elapsed) {
        world->Step(elapsed.asSeconds(), velocityIterations, positionIterations);
    }
    
    void jumpCloud() {
        if(playing) {
            cloud.jump();
        }
    }
    
    void SetGravity(b2Vec2 gravity) {
        world->SetGravity(gravity);
    }
    
    void draw(sf::RenderWindow& window) {
        cloud.draw(window);
        blockPtrs[0]->draw(window);
        blockPtrs[1]->draw(window);
        blockPtrs[2]->draw(window);
        
        /*Draw the score (only if not dead)*/
        score.str("");
        score.clear();
        score << cloud.getScore();
        if (!cloud.isDead()) {
            if(cloud.getPositionX()>0) {
                string scoreText = "Score: " + score.str() + " points";
                sfScore.setString(scoreText);
                sfScore.setPosition((cloud.getPositionX())*SCALE-100, 20);
                window.draw(sfScore);
            }
        } else {
            string gameOverText = "Game Over! \nScore: " + score.str() +"\nBest Score: "+to_string(bestScore)+"\nRestart: R";
            sfGameOver.setString(gameOverText);
            sfGameOver.setPosition((cloud.getPositionX())*SCALE-100, 200);
            window.draw(sfGameOver);
        }

    }
    
    void checkGameOver() {
        if (!cloud.checkDead()) {
            pugi::xml_document doc;
            
            if (!doc.load_file("data.xml")) cout << "Failed loading file" << endl;
            bestScore = stoi(doc.child("Saved").child("Score").attribute("BestScore").value());
            
            
            //cout<<"bestScore = "<<bestScore<<" et cloud.getPositionX()/scoreCoeff = "<<cloud.getScore()<<endl;
            if (bestScore<cloud.getScore() && cloud.getScore()>0) {
                bestScore = cloud.getScore();
                cout << "New best score" <<endl;
//                doc.child("Score").attribute("BestScore").value() = bestScore;
                doc.child("Saved").child("Score").attribute("BestScore").set_value(to_string(bestScore).c_str());
                cout<<"Done : "<<doc.child("Saved").child("Score").attribute("BestScore").value()<<endl;
                cout<<"to_string(bestScore) = "<<to_string(bestScore)<<endl;
                doc.save_file("data.xml");
//                doc.child("Score").find_attribute("BestScore").set_value(bestScore);
            }
        }
    }
    
    bool updateBlocks() {
//        cout << "Cloud position*SCALE: " << cloud.getPositionX()*SCALE << "; block[1] position: " << blockPtrs[1]->getPositionX()<<endl;
        if(cloud.getPositionX()*SCALE>blockPtrs[1]->getPositionX()) {
            /*Destruction du premier block puis ajout d'un nouveau en fin de vector blockPtrs*/
            blockPtrs.erase(blockPtrs.begin());
            blockPtrs.push_back(make_unique<Block>(*world, blockIndex+1, stormVelocityY, obstPerBlock, blockLength));
            blockIndex+=1;
        }
        return blockIndex%2==0;//returns true if blockIndex is eaven
    }
    
    pair<float, float> center() {
        return pair<float, float>(SCALE*cloud.getPositionX()+100, 300);
    }
    
    void restart() {
        blockPtrs.erase(blockPtrs.begin());
        blockPtrs.erase(blockPtrs.begin());
//        blockPtrs.erase(blockPtrs.begin());
        
//        loadVariables();
        gravity = b2Vec2(gravityX, gravityY);
        world = new b2World(gravity);
        cloud.~Cloud();
        cloud = Cloud(*world, velocityX, velocityY, scoreCoeff);
        blockIndex = 0;

        /*Initialisation du premier triplet de blocks*/
        blockPtrs.push_back(make_unique<Block>(*world, -1, stormVelocityY, obstPerBlock, blockLength));
        blockPtrs.push_back(make_unique<Block>(*world, 0, stormVelocityY, obstPerBlock, blockLength));
//        blockPtrs.push_back(make_unique<Block>(*world, 1, stormVelocityY, obstPerBlock, blockLength));

    }
    
    void playPause() {
        if (playing) {
            cloud.pause();
            blockPtrs[0]->pause();
            blockPtrs[1]->pause();
            blockPtrs[2]->pause();
            playing = false;
        } else {
            cloud.play();
            blockPtrs[0]->play();
            blockPtrs[1]->play();
            blockPtrs[2]->play();
            playing = true;
        }
    }
    
    void newCircle(float X, float Y) {
        if(cloud.checkValidCircle(X,Y)) {
            cloud.newCircle(X,Y);
        }
    }
    
    void saveCloudConfiguration() {
        cloud.saveCloudConfiguration();
    }
    
    void loadCloudConfiguration(string name) {
        cloud.loadCloudConfiguration(name);
    }
    
    pair<float, float> getCloudPosition() {
        pair<float, float> position = pair<float, float>(cloud.getPositionX(), cloud.getPositionY());
        return position;
    }
};

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
        game.setStep(elapsed);

        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                cout << sf::Mouse::getPosition().x << " ; " << sf::Mouse::getPosition().y << endl;
                cout << game.getCloudPosition().first << " ; " << game.getCloudPosition().second << endl;
                cout << window.getPosition().x << " ; " <<  window.getPosition().y << endl;
                cout << "mouse clicked" << endl;
                
                game.newCircle((sf::Mouse::getPosition().x - 620)/SCALE, (sf::Mouse::getPosition().y - 92 - game.getCloudPosition().second*SCALE)/SCALE);
                
            }
            
            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                game.jumpCloud();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                cout << "building new game" << endl;
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
                game.loadCloudConfiguration("square");
            }
            

//            /*Pour changer les variables en cours de route*/
//            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
//                loadVariables();
//                b2Vec2 gravity(gravityX, gravityY);
//                game.SetGravity(gravity);
//            }
        }


        window.clear(sf::Color(119, 185, 246));
//        if (game.updateBlocks()) {
//            window.clear(sf::Color(119, 185, 246));
//        } else {
//            window.clear(sf::Color::Blue);
//        }
        game.updateBlocks();
        game.checkGameOver();
        game.draw(window);
        
        
        // Update the window
        view.setCenter(game.center().first, game.center().second);
        window.setView(view);
        window.display();
    }

    return EXIT_SUCCESS;
}
