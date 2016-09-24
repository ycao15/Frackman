#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
/*
struct Node // coordinate system for maze search
{
    Node() {beenHere = false; dir = Actor::none;}
    
    bool beenHere;
    Actor::Direction dir;
};
*/
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir)
    : GameWorld(assetDir)
    {
        m_score = 0;
        num_actors = 0;
        gameText = "";
        actors.clear();
        barrelsFound = 0;
    }
    
    virtual int init()
    {   //actors.push_back(new Sonar(fracker, this));
        barrelsFound = 0;
        
        B = fmin((getLevel()/2) + 2, 6);
        G = fmax(5-(getLevel()/2), 2);
        L = fmin(2+getLevel(), 20);
        targetProtesters = fmin(15, 2+(getLevel()*1.5));
        T = fmax(25, 200-getLevel());
        
        probabilityOfHardcore = fmin(90, (getLevel() * 10) + 30);
        
        //sonar and water probability
        isThereSonar = false;
        P = (getLevel()*25) + 300;
        compareToP = rand() % P+1;
        
        // make the dirt
        makeDirt();
        fracker = new Frackman(this);
        
        //initialize the Node structure
        /*
        for (int x = 0; x != 64; x++)
        {
            for (int y = 0; y != 64; y++)
            {
                if (y >= 60) node[x][y] = new Node();
                else if (x >=30 && x <= 33 && y > 3) node[x][y] = new Node();
                else
                    node[x][y] = nullptr;
            }
        }
        */
         
        actors.clear();
        
        //initialize counters to 0
        num_actors = 0;
        num_protesters = 0;
        tick = 0;
        pAdded = false;
        
        // create the boulders
        for (int b = 0; b != B; b++)
        {
            bool goodForm = false;
            while (!goodForm)
            {
                int x = rand() % 57;
                int y = rand() % 57;
                if (!goodCoord(x, y)) continue;

                actors.push_back(new Boulder(x, y, fracker, this));
                /*
                for (int i = x; i != x+4; i++)
                    for (int j = y; j != y+4; j++)
                    {
                        if (node[i][j] != nullptr) delete node[i][j];
                        node[i][j] = nullptr;
                    }
                 */
                num_actors++;
                goodForm = true;
            }
        }
        
        //create the gold nuggets
        for (int g = 0; g != G; g++)
        {
            bool goodForm = false;
            while (!goodForm)
            {
                int x = rand() % 57;
                int y = rand() % 57;
                if (!goodCoord(x, y)) continue;
                
                std::string s = "frackman";
                actors.push_back(new Gold(x, y, fracker, this, s));
                num_actors++;
                goodForm = true;
            }
        }
        
        //create the barrels of oil
        for (int l = 0; l != L; l++)
        {
            bool goodForm = false;
            while (!goodForm)
            {
                int x = rand() % 57;
                int y = rand() % 57;
                if (!goodCoord(x, y)) continue;
                
                actors.push_back(new Barrel(x, y, fracker, this));
                num_actors++;
                barrels++;
                goodForm = true;
            }
        }
        
        if (getLives() <= 0)
            return GWSTATUS_PLAYER_DIED;
        
        return GWSTATUS_CONTINUE_GAME;
    }
    
    virtual int move();
    void makeDirt()
    {
        for (int x = 0; x != 64; x++)
            for (int y = 0; y != 60; y++)
            {
                if (x >=30 && x <= 33 && y > 3) d[x][y] = nullptr;
                else
                    d[x][y] = new Dirt(x, y, this);
            }
    }
    Frackman *getFracker() {return fracker;}
    void shovelDirt();
    double getDistance(Actor* f, Actor*a) const;
    void setupBoulder(int startX, int startY);
    bool checkBoulder(int startX, int startY, Actor *a);
    void playDeadSound(const std::string &s);
    void sonarTrue();
    void formatGameText();
    bool goodCoord(const int &x, const int &y);
    void setSonarStatus() {isThereSonar = false;}
    Actor* getNearest(Actor *a);
    //void setupNode();
    /*
    void clearNodeSquare(int i, int j)
    {
        for (int x = i; x != i+4; x++)
            for (int y = j; y != j+4; y++)
                if (node[x][y] != nullptr) delete node[x][y];
    }
     */
    
    void addProtester()
    {
        int probability = rand() % probabilityOfHardcore;
        if (probability != 0)
            actors.push_back(new Protester(fracker, this, IID_PROTESTER));
        if (probability == 0)
            actors.push_back(new Hardcore(fracker, this));
        num_protesters++;
        num_actors++;
        pAdded = true;
        ptick = 0;
    }
    
    void addProtesterGold()
    {
        std::string s = "protester";
        actors.push_back(new Gold(fracker->getX(), fracker->getY(), fracker, this, s));
        num_actors++;
    }
    
    void addSquirt()
    {
        actors.push_back(new Squirt(fracker->getX(), fracker->getY(), fracker, this));
        num_actors++;
    }
    
    bool isMineShaft(const int &x, const int &y) const
    {
        if (x >=27 && x <= 35 && y >= 0) return true; // 30, 33, 3
        return false;
    }
    
    void setAllVisibile()
    {
        std::list<Actor*>::iterator it = actors.begin();
        for (; it != actors.end(); it++)
        {
            (*it)->setVisible(true);
        }
    }
    
    bool canMove(const int &i, const int &j, Actor::Direction dir);
    bool checkEmptySquare(const int &i, const int &j, Actor *a);
    
    void foundBarrel()
    {barrelsFound++;}
    
    bool isThereDirt(const int& x, const int &y) const
    {
        if (x < 0 || x > 63) return false;
        if (y < 0 || y > 59) return false;
        return d[x][y] != nullptr;
    }
    
    virtual void cleanUp()
    {
        for (int x = 0; x != 64; x++)
            for (int y = 0; y != 60; y++)
            {
                if (x >=30 && x <= 33 && y > 3)
                    d[x][y] = nullptr;
                else
                    delete d[x][y];
            }
        /*
        for (int x = 0; x != 64; x++)
            for (int y = 0; y != 64; y++)
            {
                if (node[x][y] != nullptr) delete node[x][y];
                node[x][y] = nullptr;
            }
        */
        delete fracker;
        
        std::list<Actor*>::iterator it = actors.begin();
        for(; it != actors.end(); it++)
            delete (*it);
        actors.clear();
    }
    
    ~StudentWorld()
    {
        for (int x = 0; x != 64; x++)
            for (int y = 0; y != 60; y++)
            {
                if (x >=30 && x <= 33 && y > 3)
                    d[x][y] = nullptr;
                else
                    delete d[x][y];
            }
        
        delete fracker;
        std::list<Actor*>::iterator it = actors.begin();
        for(; it != actors.end(); it++)
            delete (*it);
        /*
        for (int x = 0; x != 64; x++)
            for (int y = 0; y != 64; y++)
            {
                if (node[x][y] != nullptr) delete node[x][y];
                node[x][y] = nullptr;
            }
        */
        actors.clear();
    }
    
private:
    //Node *node[64][64];
    Frackman *fracker;
    Dirt *d[VIEW_WIDTH][64];
    
    
    int B; // number of boulders in the level
    int G; // number of gold nuggets in the level
    int L; // number of oil barrels in the level
    int P; // 1 in P chance that Water/Sonar will spawn
    int targetProtesters; // target number of protesters in the field
    int T; // fewest number of ticks that can pass before another added protester
    int probabilityOfHardcore; // probability of a hardcore protester spawning
    
    int compareToP;
    
    int barrels;
    int barrelsFound;
    int sonar;
    int gold;
    int m_score;
    int currentLevel;
    int num_actors;
    int num_protesters;
    
    int tick;
    int ptick; // ticks passed since last protester was added
    bool pAdded; // true if protester has been added
    
    bool isThereSonar;
    
    std::string gameText;
    std::list<Actor*> actors;
};

#endif // STUDENTWORLD_H_
