#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <sstream>
#include <memory>

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
static float blockLength = 800.f;

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
    bool dead;
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
        
        dead = false;
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
        dead=true;
        body->SetTransform(b2Vec2(body->GetPosition().x, body->GetPosition().y), b2_pi);
        body->SetLinearVelocity(b2Vec2(0,0));
        body->SetType(b2_staticBody);
        cout << "Cloud dead --> Game over!" << endl;
    }
    
    float getScore() {
        return round(body->GetPosition().x/scoreCoeff);
    }
    
    void drawScore(sf::RenderWindow& window) {
        //Re-creating and updating the text in the sfml text object
        streamScoreText.str("Score: ");
        streamScoreText << round(body->GetPosition().x/scoreCoeff) << " points";
        scoreText = streamScoreText.str();
        sfScore.setString(scoreText);
        sfScore.setPosition((body->GetPosition().x)*SCALE-100, 0);
        window.draw(sfScore);
    }
    
    bool isDead() {
        return dead;
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
private:
    b2BodyDef bodyDef;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    sf::Texture texture;
    sf::Sprite sprite;
    obstacleType type;
public:
    Obstacle(b2World& world, float X, float Y, float width, float height, obstacleType type) {
        this->type = type;
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


static int nbBlocks = 0; //Migrer vers block.cpp avec la classe
class Block {
private:
    int N;
    vector<Obstacle> obstacles = vector<Obstacle>();
    Ground ground;
    Ceilling ceilling;
public:
    Block(b2World& world, int N){
        for (int i=0 ; i<1 ; i++) {
            obstacles.push_back(Storm(world, N*blockLength+i*blockLength/1, 100)); //Storms espacés de 1000m
            obstacles.push_back(Tornado(world, N*blockLength+(i+0.5)*blockLength/1, 400));
        }
        this->N = N;
        ground = Ground(world, (N+0.5)*blockLength);
        ceilling = Ceilling(world, (N+0.5)*blockLength);
        nbBlocks+=1;
    }
    
    Block() {
        
    }
    
    ~Block() {
//        ground.~Ground();
//        for (auto it=obstacles.begin(); it<obstacles.end() ; it++) {
//            it->~Obstacle();
//        }
        nbBlocks--;
    }
    
    void draw(sf::RenderWindow& window) {
        ground.draw(window);
        for (auto obs = obstacles.begin() ; obs<obstacles.end(); obs++) {
            obs->draw(window);
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


int main(int, char const**)
{
    loadVariables();
    
    /** Prepare the world */
    b2Vec2 gravity(gravityX, gravityY);
    b2World world(gravity);
    Cloud cloud(world);
    
    int N=0;
    
    vector<unique_ptr<Block>> blockPtrs;
    blockPtrs.push_back(make_unique<Block>(world, N-1));
    blockPtrs.push_back(make_unique<Block>(world, N));
    blockPtrs.push_back(make_unique<Block>(world, N+1));
    
    
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
        world.Step(elapsed.asSeconds(), velocityIterations, positionIterations);
        
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
        }
        
        
        window.clear(sf::Color(119, 185, 246));
        cloud.draw(window);
        cloud.drawScore(window);
        blockPtrs[0]->draw(window);
        blockPtrs[1]->draw(window);
            blockPtrs[2]->draw(window);
        cout << "blockPtrs[1]->getPosition() = " << blockPtrs[1]->getPosition() << ", cloud.getPositionX()*SCALE = " << cloud.getPositionX()*SCALE << endl;
        cout << "nbBlocks = " << nbBlocks << ", nbGrounds = " << nbGrounds << ", nbObstacles = " << nbObstacles << endl;
        if(cloud.getPositionX()*SCALE>blockPtrs[1]->getPosition()) {
            cout << "Passage à droite" <<endl;
                cout << "Length of blockPtrs : " << blockPtrs.size();
                blockPtrs.erase(blockPtrs.begin());
            blockPtrs.push_back(make_unique<Block>(world, N));
            N+=1;
            
            cout << "Block numéro " << N << " construit, " << N-2 << " détruit." << endl;
        }
        
        // Update the window
        view.setCenter(SCALE*cloud.getPositionX(), 300);
        window.setView(view);
        window.display();
    }
    
    return EXIT_SUCCESS;
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
