#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <sstream>
#include <memory>
#include <random>

#include "ResourcePath.hpp"
#include "cloud.cpp"
#include "pugixml.hpp"
#include "pugiconfig.hpp"

using namespace std;
using namespace pugi;

float distanceBetween(pair<float, float> a, pair<float, float> b);
int randomIntBetween(int inf, int sup);
void loadVariables();

/*Toutes les variables globales qui vont être initialisées
 depuis le fichier data.xml grâce à la fonction loadVariables()*/
static float gravityX, gravityY;
static float velocityY, velocityX;
static float scoreCoeff;
static float timeStep, velocityIterations, positionIterations;
static float blockLength; //Length of a block
static int obstPerBlock; //Number of each type of obstacles per block
static float stormVelocityY; // Storm objects vertical velocity (moving obstacles)
static float epsilon;


static float circleRadius = .5f;


enum obstacleType {storm, tornado};

/*Cloud est la classe de notre nuage, instanciée à chaque nouveau jeu*/
class Cloud {
private:
    b2BodyDef bodyDef;
    b2Body* body;
    sf::Sprite sprite;
    float savedSpeed;
    bool playing;
    bool dead;
    int lives;
    int compteur;
    
    vector<pair<sf::CircleShape, pair<float, float>>> sfCircles; //vecteur dont chaque élement est une paire contenant first-le sf::CircleShape et en second une paire de coordonnées
public:
    Cloud(b2World& world)
    {
        /*Création du body Box2D*/
        bodyDef.position = b2Vec2((-1000)/SCALE, 200/SCALE);
        bodyDef.type = b2_staticBody;
        bodyDef.fixedRotation =true;
        body = world.CreateBody(&bodyDef);

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
        playing = false;
        lives = 1;
    }
    
    Cloud() {
        //Constructeur par défaut (nécessaire de le déclarer, même vide)
    }

    void jump() {
        body->SetLinearVelocity(b2Vec2(velocityX,velocityY));
    }

    void draw(sf::RenderWindow& window) {
        
        for(auto c = sfCircles.begin() ; c < sfCircles.end() ; c++) {
            c->first.setPosition(SCALE * (body->GetPosition().x - circleRadius - c->second.first), SCALE * (body->GetPosition().y - circleRadius + c->second.second));
            window.draw(c->first);
        }
    }

    float getPositionX() {
        return body->GetPosition().x;
    }

    float getPositionY() {
        return body->GetPosition().y;
    }
    
    /*Fonction appelée lors de la mort du nuage. 
     Il s'arrête et se retourne sur le dos*/
    void kill() {
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

    float getScore() {
        /*Le score est proportionnel à la distance parcourue,
         au coefficient scoreCoeff prêt*/
        return round(body->GetPosition().x/scoreCoeff);
    }

    bool isDead() {
        return dead;
    }
    
    bool checkDead() {
        if (body->GetLinearVelocity().x < velocityX && playing) {
            this->kill();
            return true;
        }
        return false;
    }
    
    void play() {
        body->SetType(b2_dynamicBody);
        body->SetLinearVelocity(b2Vec2(velocityX,savedSpeed));
        playing = true;
    }
    
    void pause() {
        playing = false;
        savedSpeed = body->GetLinearVelocity().y;
        body->SetTransform(b2Vec2(body->GetPosition().x, body->GetPosition().y), body->GetAngle());
        body->SetLinearVelocity(b2Vec2(0,0));
        body->SetType(b2_staticBody);
    }
    
    void newCircle(float X, float Y) {
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
    
    void saveCloudConfiguration() {
        pugi::xml_document doc;
        if (!doc.load_file("../../../../../../../../FlappyCloud/Flappy Cloud/data.xml")) cout << "Failed loading file" << endl;
        pugi::xml_node clouds = doc.child("Saved").child("Clouds");
        
        /*Preparing the CloudConfiguration node*/
        clouds.append_child("CloudConfiguration");
        clouds.last_child().append_attribute("type").set_value("custom");
        
        for (auto c=sfCircles.begin() ; c<sfCircles.end() ; c++) {
            clouds.last_child().append_child("Center");
            clouds.last_child().last_child().append_attribute("X").set_value(to_string(c->second.first).c_str());
            clouds.last_child().last_child().append_attribute("Y").set_value(to_string(c->second.second).c_str());
        }
        
        doc.save_file("../../../../../../../../FlappyCloud/Flappy Cloud/data.xml");
    }
    
    
    void loadCloudConfiguration(string name) {
        pugi::xml_document doc;
        if (!doc.load_file("../../../../../../../../FlappyCloud/Flappy Cloud/data.xml")) cout << "Failed loading file" << endl;
        pugi::xml_node clouds = doc.child("Saved").child("Clouds");
        
        pugi::xml_node cloudConfig = clouds.find_child_by_attribute("CloudConfiguration", "name", name.c_str());
        
        /*Deleting previously written circles*/
        sfCircles.clear();
        lives = 0;
        
        pair<float, float> coordinates;
        for (pugi::xml_node center: cloudConfig.children()) {
            this->newCircle(stof(center.attribute("X").value()), stof(center.attribute("Y").value()));
        }
        
        doc.save_file("../../../../../../../../FlappyCloud/Flappy Cloud/data.xml");
    }
    
    
    /*** PROBLEME DE SCALE ***/
    bool checkValidCircle(float X, float Y) {
        pair<float, float> coordinates = pair<float, float>(-X, Y);
        bool valid1 = false;
        bool valid2 = true;
        auto c = sfCircles.begin();
        
        /*Checking that the new circle is not too far from the others*/
        while(!valid1 && c<sfCircles.end()) {
            valid1 = (distanceBetween(c->second, coordinates) < 1.75*circleRadius);
            c++;
        }
        
        /*Checking that the new circle is not too close to the others*/
        c = sfCircles.begin();
        while(valid2 && c<sfCircles.end()) {
            valid2 = (distanceBetween(c->second, coordinates) > 0.75*circleRadius);
            c++;
        }
        
        return (valid1 && valid2);
    }
    
};


/*Variable globale permettant garder un oeil 
 sur le nombre de grounds existant. Elle est incrémentée
 par Ground() et décrémentée par ~Ground().*/
static int nbGrounds = 0;
/*La classe Ground permet de créer des morceaux de sol (un par block)*/
class Ground {
private:
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    sf::Texture groundTexture;
    sf::Sprite sprite;
public:
    Ground(b2World& world, float X) {
        /*Selon la valeur de blockLength (modifiable dans data.xml)
         Les blocks, grounds et ceillings s'adapteront et adapteront leur texture.*/
        bodyDef.position = b2Vec2(X/SCALE, 500.f/SCALE);
        bodyDef.type = b2_staticBody;
        body = world.CreateBody(&bodyDef);
        shape.SetAsBox((blockLength/2)/SCALE, (16.f/2)/SCALE);
        fixtureDef.density = 0.f;
        fixtureDef.friction = 1.f;
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
        groundTexture.loadFromFile(resourcePath() + "ground.png");
//        if(((int)(X/blockLength-0.5))%2 == 0) {
//            groundTexture.loadFromFile(resourcePath() + "ground.png");
//        } else {
//            groundTexture.loadFromFile(resourcePath() + "ground2.png");
//        }
        groundTexture.setRepeated(true);
        
        nbGrounds++;
    }

    Ground() {
        nbGrounds++;
    }

    ~Ground() {
        nbGrounds--;
    }

    void draw(sf::RenderWindow& window) {
        sprite.setTexture(groundTexture);
        sprite.setOrigin(blockLength/2, 8.f);
        sprite.setPosition(body->GetPosition().x * SCALE, body->GetPosition().y * SCALE);
        sprite.setTextureRect(sf::IntRect(0,0,blockLength,128.f));
        sprite.setRotation(180/b2_pi * body->GetAngle());
        window.draw(sprite);
    }
};

/*La classe Ceilling est très similaire à la classe Ground.
 Elle permet de créer les morceaux de plafond nuageux.*/
static int nbCeillings = 0;
class Ceilling {
private:
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    sf::Texture ceillingTexture;
    sf::Sprite sprite;
public:
    Ceilling(b2World& world, float X) {
        bodyDef.position = b2Vec2(X/SCALE, -13.f/SCALE);
        bodyDef.type = b2_staticBody;
        body = world.CreateBody(&bodyDef);
        shape.SetAsBox((blockLength/2)/SCALE, (13.f/2)/SCALE);
        fixtureDef.density = 0.f;
        fixtureDef.friction = 0.f;
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
        ceillingTexture.loadFromFile(resourcePath() + "ceilling.png");
        ceillingTexture.setRepeated(true);
        nbCeillings++;
    }

    Ceilling() {
        nbCeillings++;
    }

    ~Ceilling() {
        nbCeillings--;
    }

    void draw(sf::RenderWindow& window) {
        sprite.setTexture(ceillingTexture);
        sprite.setOrigin(blockLength/2, -13.f);
        sprite.setPosition(body->GetPosition().x * SCALE, body->GetPosition().y * SCALE);
        sprite.setTextureRect(sf::IntRect(0,0,blockLength,13.f));
        sprite.setRotation(180/b2_pi * body->GetAngle());
        window.draw(sprite);
    }
};


/*Les obstacles sont de 2 types (classes dérivées).
 Encore une fois, on garde un compteur du nombre d'Obstacles créés*/
static int nbObstacles = 0;
class Obstacle {
protected: // Protected: objet de classe dérivée peut accéder à ses propres attributs déf dans la classe base
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    sf::Texture texture;
    sf::Sprite sprite;
    float height, width;
    
public:
    Obstacle(b2World& world, float X, float Y, float width, float height) {
        bodyDef.position = b2Vec2(X/SCALE, Y/SCALE);
        bodyDef.type = b2_kinematicBody;
        body = world.CreateBody(&bodyDef);
        shape.SetAsBox((width/2)/SCALE, (height/2)/SCALE);
        fixtureDef.density = 0.f;
        fixtureDef.friction = 1.f;
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
        
        this->height = (height/2)/SCALE;
        this->width=(width/2)/SCALE;
        cout<<"Obstacle "<<this<<" instancié"<<endl;
        nbObstacles++;
    }

    ~Obstacle() {
        cout<<"Obstacle "<<this<<" détruit"<<endl;
        nbObstacles--;
    }
    
    void draw(sf::RenderWindow& window) {
        sprite.setTexture(texture);
        sprite.setPosition(body->GetPosition().x * SCALE, body->GetPosition().y * SCALE);
        sprite.setRotation(180/b2_pi * body->GetAngle());
        window.draw(sprite);
    }
};


/*Les Storms sont un type d'Obstacles. Leur particularité 
 est qu'ils translatent aléatoirement de haut en bas*/
class Storm: public Obstacle {
private:
    /*Variables privées propres à Storm permettant de
    mettre des limites au déplacement de l'obstacle*/
    float topLimit;
    float bottomLimit;
    bool playing;
    float savedSpeed;
    
public:
    Storm(b2World& world, float X, float Y): Obstacle(world, X, Y,  100.f, 100.f) {
        
        texture.loadFromFile(resourcePath() + "storm.png");
        sprite.setOrigin(50.f, 50.f);
        
        body->SetLinearVelocity(b2Vec2(0, randomIntBetween(-stormVelocityY ,stormVelocityY)));

        topLimit = randomIntBetween(215, 400);
        bottomLimit = randomIntBetween(35, 215);

    }
    
    void updateVelocity() {
        if(playing) {
            /*If between topLimit and bottomLimit, keep the same speed*/
            if(body->GetPosition().y < topLimit/SCALE && body->GetPosition().y > bottomLimit/SCALE) {
                if(body->GetLinearVelocity().y > 0) {
                    body->SetLinearVelocity(b2Vec2(0, stormVelocityY));
                }
                else {
                    body->SetLinearVelocity(b2Vec2(0, -stormVelocityY));
                }
            }
            else if(body->GetPosition().y > topLimit/SCALE) {
                body->SetLinearVelocity(b2Vec2(0, -stormVelocityY));
            }
            else {
                body->SetLinearVelocity(b2Vec2(0, stormVelocityY));
            }
            if (body->GetLinearVelocity().y > 0) {
                if (body->GetPosition().y > topLimit/SCALE) body->SetLinearVelocity(b2Vec2(0, -stormVelocityY));
            }
            else if (body->GetPosition().y < bottomLimit/SCALE) body->SetLinearVelocity(b2Vec2(0, stormVelocityY));
        }
        else {
            body->SetLinearVelocity(b2Vec2(0, 0));
        }
    }
    
    void pause() {
        savedSpeed = body->GetLinearVelocity().y;
        body->SetType(b2_staticBody);
        playing = false;
    }
    
    void play() {
        body->SetType(b2_dynamicBody);
        body->SetLinearVelocity(b2Vec2(0, savedSpeed));
        playing = true;
    }

};


class Tornado: public Obstacle {
private:

public:
    Tornado(b2World& world, float X, float Y): Obstacle(world, X, Y,  100.f, 149.f) {
        texture.loadFromFile(resourcePath() + "tornado.png");
        sprite.setOrigin(50.f, 74.5f);
    }
    
    void updateVelocity() {}
    void playPause() {}
};


static int nbBlocks = 0;
class Block {
private:
    int N;
    vector<Storm> storms = vector<Storm>();
    vector<Tornado> tornadoes = vector<Tornado>();
    Ground ground;
    Ceilling ceilling;
public:
    Block(b2World& world, int N){
        if(N!=-1){
            for (int i=0 ; i<obstPerBlock ; i++) {
                storms.push_back(Storm(world, N*blockLength+i*blockLength/obstPerBlock, 100)); //Storms espacés de 1000m
                tornadoes.push_back(Tornado(world, N*blockLength+(i+0.5)*blockLength/obstPerBlock, 400));
            }
        }
        this->N = N;
        ground = Ground(world, (N+0.5)*blockLength);
        ceilling = Ceilling(world, (N+0.5)*blockLength);
        nbBlocks+=1;
        cout<<"Block "<<N<<" instancié"<<endl;
    }

    Block() {
        cout<<"Block "<<N<<" créé"<<endl;
    }

    ~Block() {
        cout<<"Block "<<N<<" détruit"<<endl;
        nbBlocks--;
    }

    void draw(sf::RenderWindow& window) {
        ground.draw(window);

        for (auto sto = storms.begin() ; sto<storms.end(); sto++) {
            sto->updateVelocity();
            sto->draw(window);
        }
        for (auto tor = tornadoes.begin() ; tor<tornadoes.end(); tor++) {
            tor->draw(window);
        }
        ceilling.draw(window);
    }
    

    float getPositionX() {
        return (N+0.5)*blockLength;
    }
    
    int getIndex() {
        return N;
    }

    int getNbBlocks() {
        return nbBlocks;
    }
    
    void play() {
        for (auto sto = storms.begin() ; sto<storms.end(); sto++) {
            sto->play();
        }
    }
    
    void pause() {
        for (auto sto = storms.begin() ; sto<storms.end(); sto++) {
            sto->pause();
        }
    }
};


class Game{
private:
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
        loadVariables();
        
        /** Prepare the world */
        gravity = b2Vec2(gravityX, gravityY);
        world = new b2World(gravity);
        cloud = Cloud(*world);
        blockIndex = 1;
        
        /*Initialisation du premier triplet de blocks*/
        blockPtrs.push_back(make_unique<Block>(*world, -1));
        blockPtrs.push_back(make_unique<Block>(*world, 0));
        blockPtrs.push_back(make_unique<Block>(*world, 1));
        
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
            
            if (!doc.load_file("../../../../../../../../FlappyCloud/Flappy Cloud/data.xml")) cout << "Failed loading file" << endl;
            bestScore = stoi(doc.child("Saved").child("Score").attribute("BestScore").value());
            
            
            //cout<<"bestScore = "<<bestScore<<" et cloud.getPositionX()/scoreCoeff = "<<cloud.getScore()<<endl;
            if (bestScore<cloud.getScore() && cloud.getScore()>0) {
                bestScore = cloud.getScore();
                cout << "New best score" <<endl;
//                doc.child("Score").attribute("BestScore").value() = bestScore;
                doc.child("Saved").child("Score").attribute("BestScore").set_value(to_string(bestScore).c_str());
                cout<<"Done : "<<doc.child("Saved").child("Score").attribute("BestScore").value()<<endl;
                cout<<"to_string(bestScore) = "<<to_string(bestScore)<<endl;
                doc.save_file("../../../../../../../../FlappyCloud/Flappy Cloud/data.xml");
//                doc.child("Score").find_attribute("BestScore").set_value(bestScore);
            }
        }
    }
    
    bool updateBlocks() {
//        cout << "Cloud position*SCALE: " << cloud.getPositionX()*SCALE << "; block[1] position: " << blockPtrs[1]->getPositionX()<<endl;
        if(cloud.getPositionX()*SCALE>blockPtrs[1]->getPositionX()) {
            /*Destruction du premier block puis ajout d'un nouveau en fin de vector blockPtrs*/
            blockPtrs.erase(blockPtrs.begin());
            blockPtrs.push_back(make_unique<Block>(*world, blockIndex+1));
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
        
        loadVariables();
        gravity = b2Vec2(gravityX, gravityY);
        world = new b2World(gravity);
        cloud.~Cloud();
        cloud = Cloud(*world);
        blockIndex = 0;

        /*Initialisation du premier triplet de blocks*/
        blockPtrs.push_back(make_unique<Block>(*world, -1));
        blockPtrs.push_back(make_unique<Block>(*world, 0));
//        blockPtrs.push_back(make_unique<Block>(*world, 1));

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
    window.setFramerateLimit(1/timeStep);
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
            

            /*Pour changer les variables en cours de route*/
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
                loadVariables();
                b2Vec2 gravity(gravityX, gravityY);
                game.SetGravity(gravity);
            }
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

float distanceBetween(pair<float, float> a, pair<float, float> b) {
    return sqrt((a.first - b.first) * (a.first - b.first) + (a.second - b.second) * (a.second - b.second));
}

int randomIntBetween(int inf, int sup) {
    static std::random_device rd;
    static std::default_random_engine engine(rd());
    std::uniform_int_distribution<unsigned> distribution(inf, sup);
    return distribution(engine);
}

void loadVariables() {
    pugi::xml_document doc;

    if (!doc.load_file("../../../../../../../../FlappyCloud/Flappy Cloud/data.xml")) cout << "Failed loading file" << endl;

    pugi::xml_node parameters = doc.child("Parameters");
    pugi::xml_node saved = doc.child("Saved");
    

    gravityX=stof(parameters.child("Gravity").attribute("GravityX").value());
    gravityY=stof(parameters.child("Gravity").attribute("GravityY").value());
    velocityX=stof(parameters.child("Velocity").attribute("VelocityX").value());
    velocityY=stof(parameters.child("Velocity").attribute("VelocityY").value());
    stormVelocityY=stof(parameters.child("StormVelocity").attribute("StormVelocityY").value());

    timeStep=stof(parameters.child("Iterations").attribute("TimeStep").value());
    velocityIterations=stof(parameters.child("Iterations").attribute("VelocityIterations").value());
    positionIterations=stof(parameters.child("Iterations").attribute("PositionIterations").value());
    obstPerBlock=stoi(parameters.child("Blocks").attribute("ObstPerBlock").value());
    blockLength=stof(parameters.child("Blocks").attribute("BlockLength").value());
    
    scoreCoeff=stof(saved.child("Score").attribute("ScoreCoeff").value());
    
    epsilon=stof(parameters.child("Kill").attribute("Epsilon").value());
}
