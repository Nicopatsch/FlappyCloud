//
//  game.cpp
//  FlappyCloud


#include "game.hpp"



Game::Game() {
    /**** Load game parameters ****/
    pugi::xml_document doc;
    
    if (!doc.load_file("data.xml")) cout << "(Loading game parameters) Failed loading file from path '" << (resourcePath()+"data.xml").c_str() << "'" << endl;
    
    pugi::xml_node parameters = doc.child("Parameters");
    pugi::xml_node saved = doc.child("Saved");
    
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
    ContactListener contactListener;
    //world->SetContactListener(&contactListener);
    cloud = Cloud(*world, velocityX, velocityY, scoreCoeff);
    blockIndex = 0;
    
    /*Initialisation du premier triplet de blocks*/
    blockPtrs.push_back(make_unique<Block>(*world, &cloud, -1, stormVelocityY, obstPerBlock, blockLength));
    blockPtrs.push_back(make_unique<Block>(*world, &cloud, 0, stormVelocityY, obstPerBlock,blockLength));
    
    //Prepare the sfml score text
    if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
        return EXIT_FAILURE;
    }
    sfScore = sf::Text(score.str(), font, 50);
    sfScore.setFillColor(sf::Color::White);
    sfGameOver = sf::Text("Game Over!", font, 50);
    sfGameOver.setFillColor(sf::Color::White);
    sfPause = sf::Text("Pause", font, 50);
    sfPause.setFillColor(sf::Color::White);
    
    sfLives = sf::Text(to_string(cloud.getLives()), font, 50);
    sfLives.setFillColor(sf::Color::White);
    
    heartTextures = vector<sf::Texture>();
    for(int i=0 ; i<5 ; i++) {
        heartTextures.push_back(sf::Texture());
    }
    heartTextures[0].loadFromFile(resourcePath() + "heart0.png");
    heartTextures[1].loadFromFile(resourcePath() + "heart1.png");
    heartTextures[2].loadFromFile(resourcePath() + "heart2.png");
    heartTextures[3].loadFromFile(resourcePath() + "heart3.png");
    heartTextures[4].loadFromFile(resourcePath() + "heart4.png");
    
    cout << "New game built" << endl;
    
    playing = false;
}


void Game::setStep(sf::Time elapsed) {
    world->Step(elapsed.asSeconds(), velocityIterations, positionIterations);
}

void Game::jumpCloud() {
    if(playing && !cloud.isDead()) {
        cloud.jump();
    }
}

void Game::SetGravity(b2Vec2 gravity) {
    world->SetGravity(gravity);
}

void Game::draw(sf::RenderWindow& window) {
    cloud.draw(window);
    blockPtrs[0]->draw(window);
    blockPtrs[1]->draw(window);
    
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
    
    string livesText = to_string(cloud.getLives());
    sfLives.setString(livesText);
    sfLives.setPosition((cloud.getPositionX())*SCALE-200, 40);
    heartSprite.setTexture(heartTextures[(cloud.getLives()!=0) * (ceil(((cloud.getLives())-1)/3)+1)]);
    heartSprite.setOrigin(0,0);
    heartSprite.setPosition(cloud.getPositionX() * SCALE-280, 45);
    heartSprite.setScale(.5, .5);

    window.draw(heartSprite);
    window.draw(sfLives);
}

void Game::checkGameOver() {
    if (!cloud.checkCollision()) {
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

bool Game::updateBlocks() {
    //        cout << "Cloud position*SCALE: " << cloud.getPositionX()*SCALE << "; block[1] position: " << blockPtrs[1]->getPositionX()<<endl;
    if(cloud.getPositionX()*SCALE>blockPtrs[1]->getPositionX()) {
        /*Destruction du premier block puis ajout d'un nouveau en fin de vector blockPtrs*/
        blockPtrs.erase(blockPtrs.begin());
        blockPtrs.push_back(make_unique<Block>(*world, &cloud, blockIndex+1, stormVelocityY, obstPerBlock, blockLength));
        blockIndex+=1;
    }
    return blockIndex%2==0;//returns true if blockIndex is even
}

pair<float, float> Game::center() {
    return pair<float, float>(SCALE*cloud.getPositionX()+100, 300);
}

////// PAS UTILISÉ
void Game::restart() {
    blockPtrs.erase(blockPtrs.begin());
    blockPtrs.erase(blockPtrs.begin());
    
    gravity = b2Vec2(gravityX, gravityY);
    world = new b2World(gravity);
    cloud.~Cloud();
    cloud = Cloud(*world, velocityX, velocityY, scoreCoeff);
    blockIndex = 0;
    
    /*Initialisation du premier triplet de blocks*/
    blockPtrs.push_back(make_unique<Block>(*world, &cloud, -1, stormVelocityY, obstPerBlock, blockLength));
    blockPtrs.push_back(make_unique<Block>(*world, &cloud, 0, stormVelocityY, obstPerBlock, blockLength));
    started = false;

}

void Game::playPause() {
    if (playing) {
        playing = false;
    } else {
        playing = true;
    }
    started = true;
}

void Game::newCircle(float X, float Y) {
    if(cloud.checkValidCircle(X,Y) && cloud.getLives() < 10) {
        cloud.newCircle(X,Y);
    }
}

void Game::saveCloudConfiguration() {
    cloud.saveCloudConfiguration();
}

void Game::loadCloudConfiguration(string name) {
    cloud.loadCloudConfiguration(name);
}

pair<float, float> Game::getCloudPosition() {
    pair<float, float> position = pair<float, float>(cloud.getPositionX(), cloud.getPositionY());
    return position;
}

bool Game::getPlaying() {
    return playing;
}

bool Game::getStarted() {
    return started;
}

bool Game::setStarted(bool b) {
    started=b;
}

float Game::getTimeStep() {
    return timeStep;
}



