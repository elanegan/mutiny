#ifndef SHIP
#include <math.h>
#define SHIP
    typedef struct Ship {
        int id; // unique identifier. player is 0.
        double health; //health, from 0-100.
        double xPosition; // x position in the game
        double yPosition; // y position in the game
        double direction; // angle of direction of travel, 0-2pi or degrees?
        double velocity; // speed of the ship
        int turnLevel; // the turn level that the player controls
        int sailsOpen; // whether the sails are open
        double firingSpeed; // how fast the ship fires
        char* spritePath; // the path to the set of sprites to be used
    } Ship;

    Ship* createNewShip(int identifier, double initX, double initY) {
        Ship* ship = malloc(sizeof(Ship));
        ship->id = identifier;
        ship->health = 100.0;
        ship->xPosition = initX;
        ship->yPosition = initY;
        ship->direction = 0.0;
        ship->velocity = 0.0;
        ship->turnLevel = 0;
        ship->firingSpeed = 1.0;
        ship->sailsOpen = 1;
        ship->spritePath = "./assets/ship0";
    }

    void setHealth(Ship* localShip, double newHealth) {
        localShip->health = newHealth;
    }

    double getHealth(Ship* localShip) {
        return(localShip->health);
    }

    void setXPosition(Ship* localShip, double newXPosition) {
        localShip->xPosition = newXPosition;
    }

    double getXPosition(Ship* localShip) {
        return(localShip->xPosition);
    }

    void setYPosition(Ship* localShip, double newYPosition) {
        localShip->yPosition = newYPosition;
    }

    double getYPosition(Ship* localShip) {
        return(localShip->yPosition);
    }

    void setDirection(Ship* localShip, double newDirection) {
        if (newDirection < 0 || newDirection > 2*M_PI)
            return;
        localShip->direction = newDirection;
    }

    double getDirection(Ship* localShip) {
        return(localShip->direction);
    }

    void setVelocity(Ship* localShip, double newVelocity) {
        localShip->velocity = newVelocity;
    }

    double getVelocity(Ship* localShip) {
        return(localShip->velocity);
    }

    // Port = left turn
    void incPortTurn(Ship* localShip) {
        if(localShip->turnLevel == -3)
            return;
        localShip->turnLevel--;
    }

    // Starboard = right turn
    void incStarboardTurn(Ship* localShip) {
        if(localShip->turnLevel == 3)
            return;
        localShip->turnLevel++;
    }

    int getTurnLevel(Ship* localShip) {
        return(localShip->turnLevel);
    }

    void setFiringSpeed(Ship* localShip, double newFiringSpeed) {
        localShip->firingSpeed = newFiringSpeed;
    }

    double getFiringSpeed(Ship* localShip) {
        return(localShip->firingSpeed);
    }

    void switchSails(Ship* localShip) {
        localShip->sailsOpen = !(localShip->sailsOpen);
    }

    int getSailStat(Ship* localShip) {
        return(localShip->sailsOpen);
    }

    void setSpritePath(Ship* localShip, char* newPath) {
        localShip->spritePath = newPath;
    }

    char* getSpritePath(Ship* localShip) {
        return(localShip->spritePath);
    }

#endif