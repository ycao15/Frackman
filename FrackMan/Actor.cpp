#include "Actor.h"
#include "StudentWorld.h"
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

void Frackman::doSomething()
{
    int ch;
    int check = getDirection();
    if (getWorld()->getKey(ch) == true)
    {
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                setDirection(left);
                if (check == left & getX() > 0 && !getWorld()->checkBoulder(getX()-1, getY(), this))
                    moveTo(getX()-1, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if (check == right && getX() < 60 && !getWorld()->checkBoulder(getX()+1, getY(), this))
                    moveTo(getX()+1, getY());
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                if (check == up && getY() < 60 && !getWorld()->checkBoulder(getX(), getY()+1, this))
                    moveTo(getX(), getY()+1);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if (check == down && getY() > 0 && !getWorld()->checkBoulder(getX(), getY()-1, this))
                    moveTo(getX(), getY()-1);
                break;
            case KEY_PRESS_ESCAPE:
                getWorld()->decLives();
                break;
            case KEY_PRESS_TAB:
                if (getGold() >= 1)
                {
                    getWorld()->addProtesterGold();
                    m_gold--;
                }
                break;
            case KEY_PRESS_SPACE:
                if (getSquirts() >= 1)
                {
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    getWorld()->addSquirt();
                    m_squirts--;
                }
                break;
            case 'Z':
            case 'z':
            {
                if (m_sonar >= 1)
                {
                    getWorld()->sonarTrue();
                    m_sonar--;
                    getWorld()->playDeadSound("deadSonar");
                }
                break;
            }
                 //test/cheat key
            case 'L':
                getWorld()->setAllVisibile();
                break;
                
        }
    }
}

//////////////////////////

void Barrel::doSomething()
{
    if (getHealth() <= 0) return;
    
    if (!visibility && getWorld()->getDistance(this, fracker) == 4)
    {setVisible(true); visibility = true; return;}
    
    if (getWorld()->getDistance(this, fracker) <= 3)
    {
        getWorld()->increaseScore(1000);
        getWorld()->foundBarrel();
        setHealth(0);
    }
}

void Barrel::playDeadSound() const
{getWorld()->StudentWorld::playDeadSound("barrel");}

//////////////////////////////

void Boulder::doSomething()
{
    if (getHealth() <= 0) return;
    
    if (setupCount == 0)
    {
        getWorld()->setupBoulder(getX(), getY());
        setupCount++;
    }
    
    if (state == "stable")
    {
        for (int i = 0; i != 4; i++)
            if (getWorld()->isThereDirt(getX()+i, getY()-1)) return;
        state = "waiting";
    }
    
    if (state == "waiting" && count <= 30)
    {
        if (count == 30)
        {
            state = "falling";
            //getWorld()->clearNodeSquare(getX(), getY());
        }
        count++;
    }
    // falling from here-on out
    if (state == "falling")
    {
        if (soundCount == 0)
        {
            soundCount++;
            getWorld()->playDeadSound("boulder");
        }
        for (int i = 0; i != 4; i++)
            if (getWorld()->isThereDirt(getX()+i, getY()-1)
                || getY() == 0)
            {
                setHealth(0);
                return;
            }
        
        moveTo(getX(), getY()-1);
        
        // bonk the fracker
        if (getWorld()->getDistance(this, fracker) <= 3 && fracker->getY() <= getY())
            getWorld()->decLives();
        
        //bonk the protester
        Actor *p = getWorld()->getNearest(this);
        if (p == nullptr || p->isLeaving()) return;
        if (p != nullptr && p->getWorld()->getDistance(this, p) <= 3
            && p->getY() <= getY())
        {
            p->annoy(0); //annoy the protester
            getWorld()->increaseScore(500);
        }
        
        
    }
}

////////////////////////////

void Squirt::doSomething()
{
    //if (getHealth() <= 0) return;
    
    if (curTravelDistance == 0)
        setDirection(fracker->getDirection());
    
    //annoy the nearest protester
    Actor *nearestProtester = getWorld()->getNearest(this);
    if (nearestProtester != nullptr)
    {
        if (getWorld()->getDistance(this, nearestProtester) <= 3)
        {
            if (!nearestProtester->isLeaving())
            {
                if (nearestProtester->identify() == "protester")
                    nearestProtester->annoy(nearestProtester->getHealth()-40);
                if (nearestProtester->identify() == "hardcore")
                    nearestProtester->annoy(nearestProtester->getHealth()-20);
                setHealth(0);
                return;
            }
        }
    }
    
    //2
    if (curTravelDistance == fullTravelDistance)
        setHealth(0);
    
    
    if (getWorld()->canMove(getX(), getY(), getDirection())
        && !getWorld()->checkBoulder(getX(), getY(), this))
    {
        switch (getDirection())
        {
            case up:
                moveTo(getX(), getY()+1);
                curTravelDistance++;
                return;
            case down:
                moveTo(getX(), getY()-1);
                curTravelDistance++;
                return;
            case left:
                moveTo(getX()-1, getY());
                curTravelDistance++;
                return;
            case right:
                moveTo(getX()+1, getY());
                curTravelDistance++;
                return;
            default:
                break;
        }
    }
    else
        setHealth(0);
}

void Squirt::playDeadSound() const
{
    getWorld()->StudentWorld::playDeadSound("squirt");
}

////////////////////////////

void Gold::doSomething()
{
    if (getHealth() <= 0) return;
    if (tick == 100)
    {
        pickupStatus = "none";
        setHealth(0);
    }
    
    if (!visibility && getWorld()->getDistance(this, fracker) == 4)
    {
        setVisible(true);
        return;
    }
    
    if (pickupStatus == "protester")
    {
        tick++;
        if (!visibility) setVisible(true);
    }
    
    if (getWorld()->getDistance(this, fracker) <= 3 && pickupStatus == "frackman")
    {
        getWorld()->increaseScore(10);
        fracker->incGold();
        setHealth(0);
        return;
    }
    
    // gets picked up by protester
    Actor *nearestProtester = getWorld()->getNearest(this);
    if (nearestProtester != nullptr && pickupStatus == "protester")
    {
        if (getWorld()->getDistance(this, nearestProtester) <= 3)
        {
            nearestProtester->bribe();
            if (nearestProtester->identify() == "protester")
                getWorld()->increaseScore(25);
            if (nearestProtester->identify() == "hardcore")
                getWorld()->increaseScore(50);
            setHealth(0);
        }
    }
}

void Gold::playDeadSound() const
{
    if (pickupStatus == "frackman")
        getWorld()->StudentWorld::playDeadSound("gold");
    if (pickupStatus == "protester")
        getWorld()->StudentWorld::playDeadSound("protesterGotGold");
}

/////////////////////////////

void Sonar::doSomething()
{
    if (getHealth() <= 0) return;
    
    if (tSetCount == 0)
    {
        maxT = fmax(100, 300-(10*getWorld()->getLevel()));
        tSetCount++;
    }
    
    if (currentT == maxT)
    {setHealth(0); getWorld()->setSonarStatus();}
    
    if (getWorld()->getDistance(this, fracker) <= 3)
    {
        getWorld()->increaseScore(75);
        fracker->incSonar();
        getWorld()->StudentWorld::playDeadSound("sonar");
        getWorld()->setSonarStatus();
        setHealth(0);
    }
    
    currentT++;
}

//////////////////////////

bool Protester::isFacingFracker()
{
    
    if (getDirection() == directionToFracker()
        /*&& getWorld()->getDistance(this, fracker) <= 4*/) return true;
    //check the getDistance() part
    
    /*
    double distance = getWorld()->getDistance(this, fracker);
    if (distance > 4) return false;
    
    int frackerX = fracker->getX();
    int frackerY = fracker->getY();
    int xDistance = frackerX - getX();
    int yDistance = frackerY - getY();
    
    switch (getDirection())
    {
        case up:
        {
            if (yDistance > 0 || yDistance < -4) return false;
            if (getX() > frackerX + 4 || getX() < frackerX - 4) return false;
            return true;
        }
        case down:
        {
            if (yDistance > 4 || yDistance < 0) return false;
            if (getX() > frackerX + 4 || getX() < frackerX - 4) return false;
            return true;
        }
        case left:
        {
            if (xDistance > 4 || xDistance < 0) return false;
            if (getY() > frackerY + 4 || getY() < frackerY - 4) return false;
            return true;
        }
        case right:
        {
            if (xDistance > 0 || xDistance < -4) return false;
            if (getY() > frackerY + 4 || getY() < frackerY - 4) return false;
            return true;
        }
        default:
            break;
    }
    */
    return false;
}

void Protester::doSomething()
{
    if (getHealth() <= 0) return;
    if (initCount == 0)
    {
        N = fmax(0, 3-(getWorld()->getLevel()/4));
        nAnnoyed = fmax(50, 100-(getWorld()->getLevel()*10));
        initCount++;
    }
    ///////////////////////////////////////////
    
    if (restingTicks == 0) state = "resting";
    
    if (annoyedTicks == 0 && justAnnoyed)
    {
        state = "annoyed";
        justAnnoyed = false;
    }
    
    //change to moving state if the resting ticks have been used up
    if (restingTicks == N)
    {
        if (previouslyLeaving) state = "leaving";
        else if (!previouslyLeaving)
            state = "moving";
        restingTicks = 0;
    }
    
    if (annoyedTicks == nAnnoyed)
    {
        if (previouslyLeaving) state = "leaving";
        if (restingTicks < N) state = "resting";
        else state = "moving";
        annoyedTicks = 0;
    }
    
    //annoyed state
    if (state == "annoyed")
    {
        annoyedTicks++;
        return;
    }
    
    //resting state
    if (state == "resting")
    {
        restingTicks++;
        return;
    }
    
    ///////////////////////////////////////////
    if (previouslyLeaving) state = "leaving";
    //3 leaving state, TODO:
    if (state == "leaving")
    {
        previouslyLeaving = true;
        if (getX() == 59 && getY() == 60)
        {
            justGaveUp = false;
            setHealth(0);
            return;
        }
        
        bool checkUp = false;
        bool checkRight = false;
        
        if (getWorld()->canMove(getX(), getY(), up) && getY() < 60)
        {
            checkUp = true;
        }
        
        if (getWorld()->canMove(getX(), getY(), right) && getX() < 60)
        {
            checkRight = true;
        }
        
        if (!checkUp && checkRight && getY() != 60)
        {
            setDirection(left);
            moveTo(getX()-1, getY());
            return;
        }
        
        bool check;
        if (getWorld()->canMove(getX(), getY(), up) && getY() < 60)
        {
            check = true;
            setDirection(up);
            moveTo(getX(), getY()+1);
        }
        
        if (!check && getWorld()->canMove(getX(), getY(), right) && getX() < 60)
        {
            setDirection(right);
            moveTo(getX()+1, getY());
        }
        
        //TODO: maze queue towards exit
        return;
    }
    
    // update the moving ticks
    movingTicks++;
    ///////// EVERYTHING AFTER IS IN THE MOVING STATE
    
    //4 CHECK
    if (getWorld()->getDistance(this, fracker) <= 4.00)
    {
        if (isFacingFracker())
        {
            if (movingTicks - shoutLast15 > 15)
            {
                getWorld()->playSound(SOUND_PROTESTER_YELL);
                shoutLast15 = movingTicks;
                
                fracker->setHealth(fracker->getHealth()-20);
                return;
            }
        }
    }

    //5 DONE
    if (lineOfSight() && getWorld()->getDistance(this, fracker) > 4)
    {
        if (accessToFracker())
        {
            Direction d = directionToFracker();
            if (d != none)
                setDirection(d);
            switch (d)
            {
                case up:
                    moveTo(getX(), getY() +1);
                    break;
                case down:
                    moveTo(getX(), getY()-1);
                    break;
                case left:
                    moveTo(getX()-1, getY());
                    break;
                case right:
                    moveTo(getX()+1, getY());
                    break;
                default:
                    break;
            }
            
            numSquaresToMoveInCurrentDirection = 0;
            return;
        }
    }
    
    
    //6 DONE
    bool continueToStep8 = false;
    numSquaresToMoveInCurrentDirection--;
    Direction dir = none;
    if (numSquaresToMoveInCurrentDirection <= 0)
    {
        //after loop, dir will be a valid direction to take
        bool goodForm = false;
        while (!goodForm)
        {
            int newDirection = rand() % 4;
            to_Direction(newDirection, dir);
            
            switch (dir)
            {
                case up:
                    if (!getWorld()->canMove(getX(), getY(), up) ||
                        getWorld()->checkBoulder(getX(), getY(), this))
                        continue;
                    goodForm = true;
                    break;
                case down:
                    if(!getWorld()->canMove(getX(), getY(), down) ||
                       getWorld()->checkBoulder(getX(), getY(), this))
                        continue;
                    goodForm = true;
                    break;
                case left:
                    if (!getWorld()->canMove(getX(), getY(), left) ||
                        getWorld()->checkBoulder(getX(), getY(), this))
                        continue;
                    goodForm = true;
                    break;
                case right:
                    
                    if (!getWorld()->canMove(getX(), getY(), right) ||
                        getWorld()->checkBoulder(getX(), getY(), this))
                        continue;
                    goodForm = true;
                    break;
                default: break;
            }
            
        }
        
        setDirection(dir);
        // squares to move
        numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
        continueToStep8 = true;
    }
    
    
    //7 DONE
    if (!continueToStep8 && number7TurnLast(getDirection())) // dir
    {
        numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
        turnLast200 = movingTicks;
    }
    
    
    //8 and 9 DONE
    switch(getDirection())
    {
        case up:
        {
            if ((getWorld()->canMove(getX(), getY(), up) &&
                !getWorld()->checkBoulder(getX(), getY(), this)) && getY() < 61)
                moveTo(getX(), getY()+1);
            else
                numSquaresToMoveInCurrentDirection = 0;
            break;
        }
        case down:
        {
            if ((getWorld()->canMove(getX(), getY(), down) &&
                 !getWorld()->checkBoulder(getX(), getY(), this)) && getY()-1 > 0)
                moveTo(getX(), getY()-1);
            else
                numSquaresToMoveInCurrentDirection = 0;
            break;
        }
        case left:
        {
            if ((getWorld()->canMove(getX(), getY(), left) &&
                 !getWorld()->checkBoulder(getX(), getY(), this)) && getX()-1 > 0)
                moveTo(getX()-1, getY());
            else
                numSquaresToMoveInCurrentDirection = 0;
            break;
        }
        case right:
        {
            if ((getWorld()->canMove(getX(), getY(), right) &&
                 !getWorld()->checkBoulder(getX(), getY(), this)) && getX() < 61)
                moveTo(getX()+1, getY());
            else
                numSquaresToMoveInCurrentDirection = 0;
            break;
        }
        default:
            break;
    }
}

void Protester::bribe()
{
    getWorld()->StudentWorld::playDeadSound("protesterFoundGold");
    if (identify() == "protester")
    {
        state = "leaving";
        previouslyLeaving = true;
    }
    
    if (identify() == "harcore")
        setJustAnnoyed();
}

void Protester::playDeadSound() const
{
    if (annoyed)
        getWorld()->StudentWorld::playDeadSound("protesterAnnoyed");
    if (justGaveUp)
        getWorld()->StudentWorld::playDeadSound("protesterGiveUp");
}

bool Protester::lineOfSight()
{
    int frackerX = fracker->getX();
    int frackerY = fracker->getY();
    
    if (getX() < frackerX + 4 && getX() > frackerX -4)
        return true;
    if (getY() < frackerY + 4 && getY() > frackerY - 4)
        return true;
    
    return false;
}

bool Protester::accessToFracker()
{
    switch (directionToFracker())
    {
        case up:
        {
            for (int x = 0; x != 4; x++)
                for (int y = getY(); y <= fracker->getY(); y++)
                {
                    if (getWorld()->isThereDirt(getX() + x,y)) return false;
                    if (getWorld()->checkBoulder(getX() + x, y, this)) return false;
                }
            return true;
        }
        case down:
        {
            for (int x = 0; x != 4; x++)
                for (int y = fracker->getY(); y <= getY(); y++)
                {
                    if (getWorld()->isThereDirt(getX()+x, y)) return false;
                    if (getWorld()->checkBoulder(getX()+x, y, this)) return false;
                }
            return true;
        }
        case left:
        {
            for (int y = 0; y != 4; y++)
                for (int x = fracker->getX(); x <= getX(); x++)
                {
                    if (getWorld()->isThereDirt(x, getY() + y)) return false; // getX()
                    if (getWorld()->checkBoulder(x, getY()+y, this)) return false; // getX()
                }
            return true;
        }
        case right:
        {
            for (int y = 0; y != 4; y++)
                for (int x = getX(); x <= fracker->getX(); x++)
                {
                    if (getWorld()->isThereDirt(x, getY() + y)) return false; // getX()
                    if (getWorld()->checkBoulder(x, getY()+y, this)) return false; // getX()
                }
            return true;
        }
        default:
            return false;
    }
    return false;
}

//CHECK
Actor::Direction Protester::directionToFracker()
{
    int xDistance = fracker->getX() - getX();
    int yDistance = fracker->getY() - getY();
    
    //check if it's up or down
    if (xDistance <= 4 && xDistance >= -4)
    {
        if (yDistance < 0) return down;
        if (yDistance > 0) return up;
        if (xDistance < 0) return left;
        if (xDistance > 0) return right;
        
    }
    
    //check if it's left or right
    if (yDistance <= 4 && yDistance >= -4)
    {
        if (xDistance < 0) return left;
        if (xDistance > 0) return right;
        if (yDistance < 0) return down;
        if (yDistance > 0) return up;
    }
    
    return none;
}

bool Protester::number7TurnLast(Direction dir)
{
    if (movingTicks - turnLast200 <= 200) return false;
    bool firstViable = false;
    bool secondViable = false;
    switch (dir)
    {
        default: return false;
        case up:
        case down:
        {
            if (getWorld()->canMove(getX(), getY(), left))
                firstViable = true;
            if (getWorld()->canMove(getX(), getY(), right))
                secondViable = true;
            if (firstViable & secondViable)
            {
                int direction = rand() % 2;
                if (direction == 0)
                    setDirection(left);
                else setDirection(right);
                return true;
            }
            
            ////// only one direction viable
            if (firstViable)
            {setDirection(left); return true;}
            if (secondViable)
            {setDirection(right); return true;}
            
            return false; // check
        }
            break;
            
        case left:
        case right:
        {
            if (getWorld()->canMove(getX(), getY(), up))
                firstViable = true;
            
            if (getWorld()->canMove(getX(), getY(), down)) // check second case
                secondViable = true;

            if (firstViable & secondViable)
            {
                int direction = rand() % 2;
                if (direction == 0)
                    setDirection(up);
                else setDirection(down);
                return true;
            }
            
            ///// only one direction viable
            if (firstViable)
            {setDirection(up); return true;}
            if (secondViable)
            {setDirection(down); return true;}
                
            return false;
        }
            break;
            
    }
    return false;
}

void Protester::annoy(const int &n)
{
    if (isLeaving()) return;
    justAnnoyed = true;
    if (n <= 0)
    {
        restingTicks = N;
        state = "leaving";
        previouslyLeaving = true;
        justGaveUp = true;
        playDeadSound();
        if (identify() == "protester")
            getWorld()->increaseScore(100);
        if (identify() == "hardcore")
            getWorld()->increaseScore(250);
        return;
    }
    
        annoyed = true;
        playDeadSound();
        annoyed = false;
        setHealth(n);
}


//////////////////////////////

void Water::doSomething()
{
    if (getHealth() <= 0) return;
    
    if (initCount == 0)
    {
        //max number of ticks the water pool can exist
        T = fmax(100, 300-(10*getWorld()->getLevel()));
        initCount++;
    }
    
    tick++; // increase the ticker
    if (tick == T)
    {
        state = "none";
        setHealth(0);
    }
    
    
    if (getWorld()->getDistance(this, fracker) <= 3)
    {
        getWorld()->increaseScore(100);
        fracker->addSquirts(5);
        state = "frackman";
        setHealth(0);
    }
}

void Water::playDeadSound() const
{
    if (state == "frackman")
        getWorld()->playDeadSound("water");
}