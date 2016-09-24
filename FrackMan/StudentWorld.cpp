#include "StudentWorld.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

double StudentWorld::getDistance(Actor *h, Actor *other) const
{
    int xRad;
    int yRad;
    double distance;
    
    xRad = h->getX() - other->getX();
    yRad = h->getY() - other->getY();
    
    if (xRad < 0) xRad *= -1;
    if (yRad < 0) yRad *= -1;
    
    distance = sqrt((xRad * xRad) + (yRad * yRad));
    return distance;
}

int StudentWorld::move()
{
    formatGameText();
    int currentLives = getLives();
    
    ///////// DO SOMETHING //////////
    if (fracker->getHealth() > 0) // tell Frackman to move
        fracker->doSomething();
    else if (fracker->getHealth() <= 0)
    {
        playSound(SOUND_PLAYER_GIVE_UP);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    //shovel away the dirt
    shovelDirt();
    
    //do something
    list<Actor*>::iterator it = actors.begin();
    for (; it != actors.end(); it++)
    {
        (*it)->doSomething();
        
        if ((*it)->getHealth() <= 0)
        {
            (*it)->playDeadSound();
            if ((*it)->identify() == "protester" || (*it)->identify() == "hardcore")
                num_protesters--;
            delete (*it);
            actors.erase(it);
            num_actors--;
        }
        
        if (getLives() < currentLives) return GWSTATUS_PLAYER_DIED;
        if (barrelsFound == L)
        {
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    // check if we should add a sonar kit or water
    int checkP = rand() % P+1;
    if (checkP == compareToP)
    {
        
        //sonar
        if (rand() % 5 +1 == 3 && !isThereSonar)
        {
            actors.push_back(new Sonar(fracker,this));
            isThereSonar = true;
            num_actors++;
        }
        
        //water pool
        else
        {
            bool goodForm = false;
            while (!goodForm)
            {
                int x = rand() % 57;
                int y = rand() % 57;
                //if (!goodCoord(x, y)) continue;
                if (!checkEmptySquare(x, y, nullptr)) continue;
                
                actors.push_back(new Water(x, y, fracker, this));
                num_actors++;
                goodForm = true;
            }
        }
    }
    
    //check if we should add a protester
    if (num_protesters == 0 && tick == 1)
        addProtester();
    if (ptick >= T && num_protesters < targetProtesters)
        addProtester();
    
    if (pAdded) ptick++;
    tick++; // increment the tick counter
    
    ///////////////////////////////////////
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::shovelDirt()
{
    if (fracker->getY() <60 && fracker->getY() >=0 && fracker->getX() < 61 && fracker->getX() >= 0)
    {
        /*
         for (int i = 0; i != 4; i++)
         for (int k = 0; k != 4; k++)
         {
         if (d[fracker->getX()+i][fracker->getY()+k] != nullptr)
         {
         delete d[fracker->getX()+i][fracker->getY()+k];
         d[fracker->getX()+i][fracker->getY()+k] = nullptr;
         playSound(SOUND_DIG);
         }
         }
         */
        
        switch(fracker->getDirection())
        {
            case Actor::up:
            {
                /*
                for (int i = 0; i != 4; i++)
                {
                    if (d[fracker->getX()+i][fracker->getY()+3] != nullptr)
                        playSound(SOUND_DIG);
                    delete d[fracker->getX()+i][fracker->getY()+3];
                    d[fracker->getX()+i][fracker->getY()+3] = nullptr;
                    node[fracker->getX()+i][fracker->getY()+3] = new Node();
                }
                */
                if (d[fracker->getX()][fracker->getY()+3] != nullptr
                    || d[fracker->getX()+1][fracker->getY()+3] != nullptr
                    || d[fracker->getX()+2][fracker->getY()+3] != nullptr
                    || d[fracker->getX()+3][fracker->getY()+3] != nullptr)
                    playSound(SOUND_DIG);
                delete d[fracker->getX()][fracker->getY()+3];
                delete d[fracker->getX()+1][fracker->getY()+3];
                delete d[fracker->getX()+2][fracker->getY()+3];
                delete d[fracker->getX()+3][fracker->getY()+3];
                d[fracker->getX()][fracker->getY()+3] = nullptr;
                d[fracker->getX()+1][fracker->getY()+3] = nullptr;
                d[fracker->getX()+2][fracker->getY()+3] = nullptr;
                d[fracker->getX()+3][fracker->getY()+3] = nullptr;
                
                //node[fracker->getX()][fracker->getY()+3] = new Node();
                //node[fracker->getX()+1][fracker->getY()+3] = new Node();
                //node[fracker->getX()+2][fracker->getY()+3] = new Node();
                //node[fracker->getX()+3][fracker->getY()+3] = new Node();
                break;
            }
            case Actor::down:
            {
                /*
                for (int i = 0; i != 4; i++)
                {
                    if (d[fracker->getX()+i][fracker->getY()] != nullptr)
                        playSound(SOUND_DIG);
                    delete d[fracker->getX()+i][fracker->getY()];
                    d[fracker->getX()+i][fracker->getY()] = nullptr;
                    node[fracker->getX()+i][fracker->getY()] = new Node();
                }
                */
                if (d[fracker->getX()][fracker->getY()] != nullptr
                    || d[fracker->getX()+1][fracker->getY()] != nullptr
                    || d[fracker->getX()+2][fracker->getY()] != nullptr
                    || d[fracker->getX()+3][fracker->getY()] != nullptr)
                    playSound(SOUND_DIG);
                delete d[fracker->getX()][fracker->getY()];
                delete d[fracker->getX()+1][fracker->getY()];
                delete d[fracker->getX()+2][fracker->getY()];
                delete d[fracker->getX()+3][fracker->getY()];
                d[fracker->getX()][fracker->getY()] = nullptr;
                d[fracker->getX()+1][fracker->getY()] = nullptr;
                d[fracker->getX()+2][fracker->getY()] = nullptr;
                d[fracker->getX()+3][fracker->getY()] = nullptr;
                
                //node[fracker->getX()][fracker->getY()] = new Node();
                //node[fracker->getX()+1][fracker->getY()] = new Node();
                //node[fracker->getX()+2][fracker->getY()] = new Node();
                //node[fracker->getX()+3][fracker->getY()] = new Node();
                break;
            }
            case Actor::left:
            {
                if (d[fracker->getX()][fracker->getY()] != nullptr
                    || d[fracker->getX()][fracker->getY()+1] != nullptr
                    || d[fracker->getX()][fracker->getY()+2] != nullptr
                    || d[fracker->getX()][fracker->getY()+3] != nullptr)
                    playSound(SOUND_DIG);
                delete d[fracker->getX()][fracker->getY()];
                delete d[fracker->getX()][fracker->getY()+1];
                delete d[fracker->getX()][fracker->getY()+2];
                delete d[fracker->getX()][fracker->getY()+3];
                d[fracker->getX()][fracker->getY()] = nullptr;
                d[fracker->getX()][fracker->getY()+1] = nullptr;
                d[fracker->getX()][fracker->getY()+2] = nullptr;
                d[fracker->getX()][fracker->getY()+3] = nullptr;
                
                //node[fracker->getX()][fracker->getY()] = new Node();
                //node[fracker->getX()][fracker->getY()+1] = new Node();
                //node[fracker->getX()][fracker->getY()+2] = new Node();
                //node[fracker->getX()][fracker->getY()+3] = new Node();
                break;
            }
            case Actor::right:
            {
                if (d[fracker->getX()+3][fracker->getY()] != nullptr
                    || d[fracker->getX()+3][fracker->getY()+1] != nullptr
                    || d[fracker->getX()+3][fracker->getY()+2] != nullptr
                    || d[fracker->getX()+3][fracker->getY()+3] != nullptr)
                    playSound(SOUND_DIG);
                delete d[fracker->getX()+3][fracker->getY()];
                delete d[fracker->getX()+3][fracker->getY()+1];
                delete d[fracker->getX()+3][fracker->getY()+2];
                delete d[fracker->getX()+3][fracker->getY()+3];
                d[fracker->getX()+3][fracker->getY()] = nullptr;
                d[fracker->getX()+3][fracker->getY()+1] = nullptr;
                d[fracker->getX()+3][fracker->getY()+2] = nullptr;
                d[fracker->getX()+3][fracker->getY()+3] = nullptr;
                
                //node[fracker->getX()+3][fracker->getY()] = new Node();
                //node[fracker->getX()+3][fracker->getY()+1] = new Node();
                //node[fracker->getX()+3][fracker->getY()+2] = new Node();
                //node[fracker->getX()+3][fracker->getY()+3] = new Node();
                break;
            }
            default: break;
        }
        //
    }
}

void StudentWorld::setupBoulder(int startX, int startY)
{
    delete d[startX][startY];
    delete d[startX+1][startY];
    delete d[startX+2][startY];
    delete d[startX+3][startY];
    
    delete d[startX][startY+1];
    delete d[startX+1][startY+1];
    delete d[startX+2][startY+1];
    delete d[startX+3][startY+1];
    
    delete d[startX][startY+2];
    delete d[startX+1][startY+2];
    delete d[startX+2][startY+2];
    delete d[startX+3][startY+2];
    
    delete d[startX][startY+3];
    delete d[startX+1][startY+3];
    delete d[startX+2][startY+3];
    delete d[startX+3][startY+3];
    //////////////////////////////
    d[startX][startY] = nullptr;
    d[startX + 1][startY] = nullptr;
    d[startX+2][startY] = nullptr;
    d[startX+3][startY] = nullptr;
    
    d[startX][startY+1] = nullptr;
    d[startX+1][startY+1] = nullptr;
    d[startX+2][startY+1] = nullptr;
    d[startX+3][startY+1] = nullptr;
    
    d[startX][startY+2] = nullptr;
    d[startX+1][startY+2] = nullptr;
    d[startX+2][startY+2] = nullptr;
    d[startX+3][startY+2] = nullptr;
    
    d[startX][startY+3] = nullptr;
    d[startX+1][startY+3] = nullptr;
    d[startX+2][startY+3] = nullptr;
    d[startX+3][startY+3] = nullptr;
}

bool StudentWorld::checkBoulder(int startX, int startY, Actor *a)
{
    //if (a == nullptr) return false;
    Actor *boulder = nullptr;
    list<Actor*>::iterator it = actors.begin();
    for (; it != actors.end(); it++)
    {
        if (a != nullptr && (*it)->identify() == "boulder" && getDistance(a, (*it)) <= 8.00)
        {
            boulder = (*it);
            break;
        }
    }

    if (boulder == nullptr) return false;
    
    int bX = boulder->getX();
    int bY = boulder->getY();
    int x = startX - bX;
    int y = startY - bY;
    double distance = sqrt((x*x) + (y*y));
    if (distance <= 3.8) return true;
    
    /*
    if (boulder == nullptr) return false;
    if (dir == 0) // approaching from left
    {
        if (boulder->getX()+3 == startX)
        {
            for (int k = startY; k <= startY + 3; k++)
            {
                for (int i = boulder->getY(); i <= boulder->getY()+3; i++)
                    if (i == k) return true;
            }
        }
    }
    
    else if (dir == 1) // approaching from right
    {
        if (boulder->getX()-3 == startX)
        {
            for (int k = startY; k <= startY + 3; k++)
            {
                for (int i = boulder->getY(); i <= boulder->getY()+3; i++)
                    if (i == k) return true;
            }
        }
    }
    
    else if (dir == 2) // approaching from below
    {
        if (boulder->getY()-3 == startY)
        {
            for (int k = startX; k <= startX + 3; k++)
            {
                for (int i = boulder->getX(); i <= boulder->getX()+3; i++)
                    if (i == k) return true;
            }
        }
    }
    
    else if (dir == 3) // approaching from above
    {
        if (boulder->getY()+3 == startY)
        {
            for (int k = startX; k <= startX + 3; k++)
            {
                for (int i = boulder->getX(); i <= boulder->getX()+3; i++)
                    if (i == k) return true;
            }
        }
    }
    */
    return false;
}

void StudentWorld::playDeadSound(const std::string &s)
{
    if (s == "gold" || s == "sonar" || s == "water")
        playSound(SOUND_GOT_GOODIE);
    if (s == "boulder") playSound(SOUND_FALLING_ROCK);
    else if (s == "barrel") playSound(SOUND_FOUND_OIL);
    else if (s == "deadSonar") playSound(SOUND_SONAR);
    else if (s == "protesterFoundGold") playSound(SOUND_PROTESTER_FOUND_GOLD);
    else if (s == "protesterGiveUp") playSound(SOUND_PROTESTER_GIVE_UP);
    else if (s == "protesterAnnoyed") playSound(SOUND_PROTESTER_ANNOYED);
}

void StudentWorld::sonarTrue()
{
    list<Actor*>::iterator it = actors.begin();
    for (; it != actors.end(); it++)
    {
        if (getDistance(fracker, *it) <= 12.00)
            (*it)->setVisible(true);
    }
}

void StudentWorld::formatGameText()
{
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int health = fracker->getHealth();
    int gold = fracker->getGold();
    int water = fracker->getSquirts();
    int sonar = fracker->getSonar();
    int barrelsLeft = L - barrelsFound;
    
    //set the score string
    string newScore = to_string(score);
    int scoreSize = (int) newScore.length();
    //string temp = newScore;
    newScore = "";
    for (int i = 0; i != 8-scoreSize; i++)
        newScore += "0";
    newScore += to_string(score);
    
    //set the level string
    string newLevel = "";
    if (level < 10)
        newLevel += " ";
    newLevel += to_string(level);
    
    //set the health string
    string newHealth = to_string(health) + "%";
    
    //set the water string
    string newWater = "";
    if (water < 10) newWater += " ";
    newWater += to_string(water);
    
    //set the gold string
    string newGold = "";
    if (gold < 10) newGold += " ";
    newGold += to_string(gold);
    
    //set the sonar string
    string newSonar = "";
    if (sonar < 10) newSonar += " ";
    newSonar += to_string(sonar);
    
    //set the oil left string
    string oilLeft = "";
    if (barrelsLeft < 10) oilLeft += " ";
    oilLeft += to_string(barrelsLeft);
    
    string final = "Scr: " + newScore + "  " + "Lvl: " + newLevel + "  "
    + "Lives: " + to_string(lives) + "  " + "Hlth: " + newHealth + "  " + "Wtr: "
    + newWater + "  " + "Gld: " + newGold + "  " + "Sonar: " + newSonar
    + "  " + "Oil Left: " + oilLeft;
    setGameStatText(final);
}

bool StudentWorld::goodCoord(const int &x, const int &y)
{
    if (isMineShaft(x, y)) return false;
    
    // get a good set of valid coordinates
    int check = 0;
    list<Actor*>::iterator it = actors.begin();
    for (; it != actors.end(); it++)
    {
        int xRad = (*it)->getX() - x;
        int yRad = (*it)->getY() - y;
        
        if (xRad < 0) xRad *= -1;
        if (yRad < 0) yRad *= -1;
        
        if (sqrt((xRad * xRad) + (yRad * yRad)) <= 6.00) return false;
        //if (isMineShaft(xRad, yRad)) return false;
        check++;
    }
    
    // if the coordinates generated are not 6 units away from all other actors
    if (check != num_actors) return false;
    return true;
}

Actor* StudentWorld::getNearest(Actor *a)
{
    double distance = 1000;
    Actor *f = nullptr;
    list<Actor*>::iterator it = actors.begin();
    for (; it != actors.end(); it++)
    {
        if ((*it)->identify() == "protester" || (*it)->identify() == "hardcore")
        {
            double tempDistance = getDistance(a, (*it));
            if (tempDistance < distance)
                f = (*it);
            distance = tempDistance;
        }
    }
    
    return f;
}

bool StudentWorld::canMove(const int &i, const int &j, Actor::Direction dir)
{
    switch (dir)
    {
        case Actor::up:
        {
            if (j == 60) return false;
            for (int x = i; x != i+4; x++)
            {
                if (isThereDirt(x, j+4)) return false;
                if (checkBoulder(x, j+4, nullptr)) return false; // check
            }
            return true;
        }
        case Actor::down:
        {
            if (j == 0) return false;
            for (int x = i; x != i+4; x++)
            {
                if (isThereDirt(x, j-1)) return false;
                if (checkBoulder(x, j-1, nullptr)) return false;
            }
            return true;
        }
        case Actor::left:
        {
            if (i == 0) return false;
            for (int y = j; y != j+4; y++)
            {
                if (isThereDirt(i-1, j)) return false;
                if (checkBoulder(i-1, j, nullptr)) return false;
            }
            return true;
        }
        case Actor::right:
        {
            if (i == 60) return false;
            for (int y = j; y != j+4; y++)
            {
                if (isThereDirt(i+4, j)) return false;
                if (checkBoulder(i+4, j, nullptr)) return false;
            }
            return true;
        }
        default:
            break;
    }
    
    return false;
}

bool StudentWorld::checkEmptySquare(const int &i, const int &j, Actor *a)
{
    if (i + 4>= 60 || j +4 >= 60) return false;
    
        for (int x = i; x != i+4; x++)
            for (int y = j; y != j+4; y++)
                if (isThereDirt(x, y)) return false;
    //if (a != nullptr)
    //{
        if (checkBoulder(i, j, a)) return false;
        if (checkBoulder(i, j, a)) return false;
        if (checkBoulder(i, j, a)) return false;
        if (checkBoulder(i, j, a)) return false;
    //}
    
    return true;
}