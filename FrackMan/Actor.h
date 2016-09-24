#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld *w)
    : GraphObject(imageID, startX, startY, dir, size, depth)
    {setVisible(true); m_health = 100; world = w;}
    
    int getHealth() const {return m_health;}
    void setHealth(int h) {m_health = h;}
    virtual void playDeadSound() const = 0;
    virtual std::string identify() const = 0;
    virtual void annoy(const int &n) {}
    virtual void bribe() {}
    virtual bool isLeaving() {return true;}
    
    
    virtual void doSomething() = 0;
    
    StudentWorld* getWorld() const {return world;}
    
    virtual ~Actor() {}
    
private:
    int m_health;
    StudentWorld *world;
};

class Dirt : public Actor
{
public:
     Dirt (int startX, int startY, StudentWorld *w)
    : Actor(IID_DIRT, startX, startY, right, 0.25, 3, w) {}
    
    void doSomething() {}
    std::string identify() const {return "";}
    void playDeadSound() const {}
    ~Dirt() {}
};

class Frackman : public Actor
{
public:
    Frackman(StudentWorld* world)
    : Actor(IID_PLAYER, 30, 60, right, 1.0, 0, world)
    {m_gold = 0; m_sonar = 1; m_squirts = 5;}
    
    void doSomething();
    void incGold() {m_gold++;}
    void incSonar() {m_sonar+=2;}
    int getGold() {return m_gold;}
    int getSonar() {return m_sonar;}
    int getSquirts() {return m_squirts;}
    void addSquirts(int n) {m_squirts += n;}
    void playDeadSound() const {}
    std::string identify() const {return "frackman";}
    
    ~Frackman() {}
    
private:
    int m_gold;
    int m_sonar;
    int m_squirts;
};

class Barrel: public Actor
{
public:
    Barrel(int startX, int startY, Frackman *f, StudentWorld *w)
    : Actor(IID_BARREL, startX, startY, right, 1.0, 2, w)
    {
        setVisible(false);
        visibility = false;
        fracker = f;
    }
    
    void doSomething();
    void playDeadSound() const;
    std::string identify() const {return "barrel";}
    
    ~Barrel(){}
    
private:
    bool visibility;
    Frackman *fracker;
};

class Boulder: public Actor
{
public:
    Boulder(int startX, int startY, Frackman *f, StudentWorld *w)
    : Actor(IID_BOULDER, startX, startY, down, 1.0, 1, w)
    {
        setVisible(true);
        setHealth(100);
        state = "stable";
        count = 0;
        fracker = f;
        soundCount = 0;
        setupCount = 0;
    }
    
    void doSomething();
    void playDeadSound() const {}
    std::string getState() {return state;}
    std::string identify() const {return "boulder";}
    
    ~Boulder(){}
private:
    std::string state;
    int count;
    int soundCount;
    int setupCount;
    Frackman *fracker;
};

class Squirt: public Actor
{
public:
    Squirt(int startX, int startY, Frackman *f, StudentWorld *w)
    : Actor(IID_WATER_SPURT,startX, startY, f->getDirection(), 1.0, 1, w)
    {
        setVisible(true);
        setHealth(100);
        fracker = f;
        fullTravelDistance = 8;
        curTravelDistance = 0;
    }
    
    void doSomething();
    void playDeadSound() const;
    std::string identify() const {return "squirt";}
    
    ~Squirt() {}
    
private:
    int curTravelDistance;
    int fullTravelDistance;
    Frackman *fracker;
};

class Gold: public Actor
{
public:
    Gold(int startX, int startY, Frackman *f, StudentWorld *w, std::string &s)
    : Actor(IID_GOLD, startX, startY, right, 1.0, 2, w)
    {
        fracker = f;
        pickupStatus = s;
        tick = 0;
        if (pickupStatus == "frackman")
        {
            setVisible(false);
            visibility = false;
        }
        
        if (pickupStatus == "protester")
        {
            setVisible(true);
            visibility = true;
        }
    }
    
    void doSomething();
    void playDeadSound() const;
    std::string identify() const {return "gold";}
    void setState() {pickupStatus = "protester";}
    
    ~Gold(){}
private:
    Frackman *fracker;
    int tick;
    bool visibility;
    std::string pickupStatus;
};

class Sonar: public Actor
{
public:
    Sonar(Frackman *f, StudentWorld *w)
    : Actor(IID_SONAR, 0, 60, right, 1.0, 2, w)
    {
        tSetCount = 0;
        currentT = 0;
        fracker = f;
        state = "frackman";
    }
    
    void doSomething();
    std::string identify() const {return "sonar";}
    void playDeadSound() const {}
    
    ~Sonar() {}
    
private:
    int tSetCount;
    int maxT;
    int currentT;
    std::string state;
    Frackman *fracker;
};

class Protester: public Actor
{
public:
    Protester(Frackman *f, StudentWorld *w, const int &IID)
    : Actor(IID, 60,60, left, 1.0, 0, w)
    {
        fracker = f;
        state = "moving";
        restingTicks = 0;
        movingTicks = 0;
        initCount = 0;
        hitPoints = 5;
        
        shoutLast15 = -16;
        turnLast200 = -200;
        annoyedTicks = 0;
        
        annoyed = false;
        justAnnoyed = false; // bool for checking whether to wait in annoyed state
        previouslyLeaving = false;
        justGaveUp = false;
        numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
    }
    
    void doSomething();
    std::string identify() const {return "protester";}
    void playDeadSound() const;
    void setJustAnnoyed() {justAnnoyed = true;}
    bool isFacingFracker();
    bool lineOfSight();
    bool accessToFracker();
    virtual bool isLeaving() {return previouslyLeaving;}
    Actor::Direction directionToFracker();
    bool number7TurnLast(Direction dir);
    virtual void bribe();
    
    virtual void annoy(const int &n);
    
    void to_Direction(const int &i, Direction &d)
    {
        if (i == 0) d = up;
        else if (i == 1) d = down;
        else if (i == 2) d = left;
        else if (i == 3) d = right;
    }
    
    virtual ~Protester() {}
    
private:
    Frackman *fracker;
    std::string state;
    
    int restingTicks;
    int movingTicks;
    int N; // how many ticks pass before a protester can do something
    int nAnnoyed; // how many ticks the protester waits when it gets annoyed
    int annoyedTicks;
    
    int initCount;
    int shoutLast15;
    int turnLast200;
    
    int hitPoints;
    bool annoyed;
    bool justAnnoyed;
    bool previouslyLeaving;
    bool justGaveUp;
    int numSquaresToMoveInCurrentDirection;
};

class Water: public Actor
{
public:
    Water(int startX, int startY, Frackman *f, StudentWorld *w)
    : Actor(IID_WATER_POOL, startX, startY, right, 1.0, 2, w)
    {
        setVisible(true);
        fracker = f;
        initCount = 0;
        tick = 0;
        state = "waiting";
    }
    
    void doSomething();
    void playDeadSound() const;
    std::string identify() const {return "water";}
    
    ~Water() {}
private:
    Frackman *fracker;
    int initCount;
    int T;
    int tick;
    std::string state;
};

class Hardcore : public Protester
{
public:
    Hardcore(Frackman *f, StudentWorld *w)
    : Protester(f, w, IID_HARD_CORE_PROTESTER)
    {setHealth(200);}
    
    void doSomething() {Protester::doSomething();}
    std::string identify() const {return "hardcore";}
    void playDeadSound() const {Protester::playDeadSound();}
    
    ~Hardcore() {}
private:
};

#endif // ACTOR_H_
