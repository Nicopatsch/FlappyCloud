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

int randomIntBetween(int inf, int sup);
void loadVariables();

static float gravityX, gravityY;
static float velocityY, velocityX;
static float scoreCoeff;
static float timeStep, velocityIterations, positionIterations;
static float blockLength = 800.f;
static float stormVelocityY; // Storm objects vertical velocity (moving obstacles)
static float epsilon;

enum obstacleType {storm, tornado};

class Cloud {
private:
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    sf::Texture cloudTexture;
    sf::Sprite sprite;
    bool dead;
public:
    Cloud(b2World& world)
    {
        cloudTexture.loadFromFile(resourcePath() + "cloud.png");
        bodyDef.position = b2Vec2(300/SCALE, 200/SCALE);
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

        dead = false;
    }
    
    Cloud() {
        
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

    float getPositionY() {
        return body->GetPosition().y;
    }
    
    void kill() {
        dead=true;
        body->SetTransform(b2Vec2(body->GetPosition().x, body->GetPosition().y), b2_pi);
        body->SetLinearVelocity(b2Vec2(0,0));
        body->SetType(b2_staticBody);
        cout << "Cloud dead --> Game over!" << endl;
    }

    float getScore() {
        return round(body->GetPosition().x/scoreCoeff);
    }

    bool isDead() {
        return dead;
    }
    
    float getWidth() {
        return (100.f/2)/SCALE;
    }
    
    float getHeight() {
        return (67.f/2)/SCALE;
    }
    
    bool checkDead() {
        if (body->GetLinearVelocity().x < velocityX) {
            this->kill();
            return true;
        }
        return false;
    }
    
};


static int nbGrounds = 0;
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
        bodyDef.position = b2Vec2(X/SCALE, 500.f/SCALE);
        bodyDef.type = b2_staticBody;
        body = world.CreateBody(&bodyDef);
        shape.SetAsBox((blockLength/2)/SCALE, (16.f/2)/SCALE);
        fixtureDef.density = 0.f;
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
        groundTexture.loadFromFile(resourcePath() + "ground.png");
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
        sprite.setRotation(180/b2_pi * body->GetAngle());
        window.draw(sprite);
    }
};


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
        shape.SetAsBox((blockLength/2)/SCALE, 6.5f/SCALE);
        fixtureDef.density = 0.f;
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
        ceillingTexture.loadFromFile(resourcePath() + "ceilling.png");
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
        sprite.setRotation(180/b2_pi * body->GetAngle());
        window.draw(sprite);
    }
};



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
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
        
        this->height = (height/2)/SCALE;
        this->width=(width/2)/SCALE;
        
        nbObstacles++;
    }

    ~Obstacle() {
        nbObstacles--;
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
    float topLimit;
    float bottomLimit;
    
public:
    Storm(b2World& world, float X, float Y): Obstacle(world, X, Y,  100.f, 100.f) {
        
        texture.loadFromFile(resourcePath() + "storm.png");
        sprite.setOrigin(50.f, 50.f);
        
        body->SetLinearVelocity(b2Vec2(0, randomIntBetween(-stormVelocityY ,stormVelocityY)));

        topLimit = randomIntBetween(215, 400); //400 étant environ à hauteur du sol (à vérifier, estimation…)
        bottomLimit = randomIntBetween(35, 215); //35 hauteur du plafond

    }
    
    void updateVelocity() {
        if (body->GetLinearVelocity().y > 0) {
            if (body->GetPosition().y > topLimit/SCALE) body->SetLinearVelocity(b2Vec2(0, -stormVelocityY));
        }
        else if (body->GetPosition().y < bottomLimit/SCALE) body->SetLinearVelocity(b2Vec2(0, stormVelocityY));
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

};


static int nbBlocks = 0; //Migrer vers block.cpp avec la classe
class Block {
private:
    int N;
    vector<Storm> storms = vector<Storm>();
    vector<Tornado> tornadoes = vector<Tornado>();
    Ground ground;
    Ceilling ceilling;
public:
    Block(b2World& world, int N){
        for (int i=0 ; i<1 ; i++) {
            storms.push_back(Storm(world, N*blockLength+i*blockLength/1, 100)); //Storms espacés de 1000m
            tornadoes.push_back(Tornado(world, N*blockLength+(i+0.5)*blockLength/1, 400));
        }
        this->N = N;
        ground = Ground(world, (N+0.5)*blockLength);
        ceilling = Ceilling(world, (N+0.5)*blockLength);
        nbBlocks+=1;
    }

    Block() {

    }

    ~Block() {

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
    

    float getPosition() {
        return (N+0.5)*blockLength;
    }

    int getNbBlocks() {
        return nbBlocks;
    }
};


class Game{
private:
    Cloud cloud;
    b2Vec2 gravity;
    b2World* world;
    int nbOfBlocks;
    vector<unique_ptr<Block>> blockPtrs; //Pointeurs vers les 3 blocks que l'on suit. A chaque création d'un block, on supprime le premier de la liste et on rajoute le nouveau à la fin
    stringstream streamScoreText;
    sf::Text sfScore;
    stringstream streamGameOverText;
    sf::Text sfGameOver;
    sf::Font font;
    
    
public:
    Game() {
        loadVariables();
        
        /** Prepare the world */
        gravity = b2Vec2(gravityX, gravityY);
        world = new b2World(gravity);
        cloud = Cloud(*world);
        nbOfBlocks = 0;
        
        /*Initialisation du premier triplet de blocks*/
        blockPtrs.push_back(make_unique<Block>(*world, nbOfBlocks-1));
        blockPtrs.push_back(make_unique<Block>(*world, nbOfBlocks));
        blockPtrs.push_back(make_unique<Block>(*world, nbOfBlocks+1));
        
        //Prepare the sfml score text
        if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
            return EXIT_FAILURE;
        }
        sfScore = sf::Text(streamScoreText.str(), font, 50);
        sfScore.setFillColor(sf::Color::Black);
        sfGameOver = sf::Text("Game Over!", font, 50);
        sfGameOver.setFillColor(sf::Color::Black);
    }
    
    void setStep(sf::Time elapsed) {
        world->Step(elapsed.asSeconds(), velocityIterations, positionIterations);
    }
    
    void jumpCloud() {
        cloud.jump();
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
        if (!cloud.isDead()) {
            streamScoreText.str("Score: ");
            streamScoreText << round(cloud.getPositionX()/scoreCoeff) << " points";
            sfScore.setString(streamScoreText.str());
            sfScore.setPosition((cloud.getPositionX())*SCALE-100, 0);
            window.draw(sfScore);
        } else {
            /*Transformation du texte de score "Game Over"*/
            streamGameOverText.str("");
            streamGameOverText.clear();
            streamGameOverText.str("Game Over! Score: ");
            streamGameOverText << round(cloud.getPositionX()/scoreCoeff) << " points";
            sfGameOver.setString(streamGameOverText.str());
            sfGameOver.setPosition((cloud.getPositionX())*SCALE-100, 200);
            window.draw(sfGameOver);
        }

    }
    
    void checkGameOver() {
        if (cloud.checkDead()) {
            
        }
    }
    
    void updateBlocks() {
        if(cloud.getPositionX()*SCALE>blockPtrs[1]->getPosition()) {
            /*Destruction du premier block puis ajout d'un nouveau en fin de vector blockPtrs*/
            blockPtrs.erase(blockPtrs.begin());
            blockPtrs.push_back(make_unique<Block>(*world, nbOfBlocks));
            nbOfBlocks+=1;
        }
    }
    
    pair<float, float> center() {
        return pair<float, float>(SCALE*cloud.getPositionX(), 300);
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

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                game.jumpCloud();
            }

            /*Pour changer les variables en cours de route*/
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
                loadVariables();
                b2Vec2 gravity(gravityX, gravityY);
                game.SetGravity(gravity);
            }
        }


        window.clear(sf::Color(119, 185, 246));
        
        game.draw(window);
        
        game.checkGameOver();
        
        game.updateBlocks();
        
        // Update the window
        view.setCenter(game.center().first, game.center().second);
        window.setView(view);
        window.display();
    }

    return EXIT_SUCCESS;
}

int randomIntBetween(int inf, int sup) {
    static std::random_device rd;
    static std::default_random_engine engine(rd());
    std::uniform_int_distribution<unsigned> distribution(inf, sup);
    return distribution(engine);
}

void loadVariables() {
    pugi::xml_document doc;

    if (!doc.load_file("../../../../../../../../FlappyCloud/Flappy Cloud/parameters.xml")) cout << "Failed loading file" << endl;

    pugi::xml_node parameters = doc.child("Parameters");

    gravityX=stof(parameters.child("Gravity").attribute("GravityX").value());
    gravityY=stof(parameters.child("Gravity").attribute("GravityY").value());
    velocityX=stof(parameters.child("Velocity").attribute("VelocityX").value());
    velocityY=stof(parameters.child("Velocity").attribute("VelocityY").value());
    stormVelocityY=stof(parameters.child("StormVelocity").attribute("StormVelocityY").value());

    scoreCoeff=stof(parameters.child("Score").attribute("ScoreCoeff").value());
    timeStep=stof(parameters.child("Iterations").attribute("TimeStep").value());
    velocityIterations=stof(parameters.child("Iterations").attribute("VelocityIterations").value());
    positionIterations=stof(parameters.child("Iterations").attribute("PositionIterations").value());
    
    epsilon=stof(parameters.child("Kill").attribute("Epsilon").value());
}
