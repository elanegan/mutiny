#ifndef SHIP
#include <math.h>
#include <string.h>
#include "./wind.h"
#define SHIP
//define compass directions
#define SHIP_NORTH 0
#define SHIP_NORTH_WEST 1
#define SHIP_WEST 2
#define SHIP_SOUTH_WEST 3
#define SHIP_SOUTH 4
#define SHIP_SOUTH_EAST 5
#define SHIP_EAST 6
#define SHIP_NORTH_EAST 7

    typedef struct Ship {
        int id; // unique identifier. player is 0.
        double health; //health, from 0-100.
        double xPosition; // x position on the game map
        double yPosition; // y position on the game map
        int displayXPosition; // x position on the display screen
        int displayYPosition; // y position on the display screen
        double direction; // angle of direction of travel, 0-2pi or degrees?
        double xVelocity; // x velocity of the ship across the map
        double yVelocity; //y velocity of the ship across the map
        int turnLevel; // the turn level that the player controls
        int sailsOpen; // whether the sails are open
        double firingSpeed; // how fast the ship fires
        char spritePath[64]; // the path to the set of sprites to be used
    } Ship;

    Ship* createNewShip(int identifier, double mapY, double mapX, int screenY, int screenX) {
        Ship* ship = malloc(sizeof(Ship));
        ship->id = identifier;
        ship->health = 100.0;
        ship->xPosition = mapX;
        ship->yPosition = mapY;
        ship->displayXPosition = screenX;
        ship->displayYPosition = screenY;
        ship->direction = 0.0;
        ship->xVelocity = 0.0;
        ship->yVelocity = 0.0;
        ship->turnLevel = 0;
        ship->firingSpeed = 1.0;
        ship->sailsOpen = 1;
        strcpy(ship->spritePath, "./assets/ship/sailOpen/ship0");

        return(ship);
    }

    void destroyShip(Ship* localShip) {
        free(localShip);
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

    void setDisplayXPosition(Ship* localShip, int newXPosition) {
        localShip->displayXPosition = newXPosition;
    }

    int getDisplayXPosition(Ship* localShip) {
        return(localShip->displayXPosition);
    }

    void setDisplayYPosition(Ship* localShip, int newYPosition) {
        localShip->displayYPosition = newYPosition;
    }

    int getDisplayYPosition(Ship* localShip) {
        return(localShip->displayYPosition);
    }

    void updateDisplayPosition(Ship* localShip, int screenYPos, int screenXPos) {
        int newX = getXPosition(localShip) - screenXPos;
        int newY = getYPosition(localShip) - screenYPos;
        setDisplayXPosition(localShip, newX)
    }

    void setDirection(Ship* localShip, double newDirection) {
        while (newDirection < 0) {
            newDirection += (2*M_PI);
        }
        while (newDirection > 2*M_PI) {
            newDirection -= (2*M_PI);
        }

        localShip->direction = newDirection;
    }

    double getDirection(Ship* localShip) {
        return(localShip->direction);
    }

    void setXVelocity(Ship* localShip, double newVelocity) {
        localShip->xVelocity = newVelocity;
    }

    double getXVelocity(Ship* localShip) {
        return(localShip->xVelocity);
    }

    void setYVelocity(Ship* localShip, double newVelocity) {
        localShip->yVelocity = newVelocity;
    }

    double getYVelocity(Ship* localShip) {
        return(localShip->yVelocity);
    }

    double getSpeed(Ship* localShip) {
        double xSpeed = fabs(getXVelocity(localShip));
        double ySpeed = fabs(getYVelocity(localShip));
        double speed = sqrt((xSpeed*xSpeed) + (ySpeed*ySpeed));

        return speed;
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
        strcpy(localShip->spritePath, newPath);
    }

    void setSpriteCompass(Ship* localShip, int orientation) {
        if (orientation < 0 || orientation > 7)
            return;
        
        char path[64];
        char openClose[16] = {"sailDown/"};
        if(getSailStat(localShip))
            strcpy(openClose, "sailOpen/");

        snprintf(path, 64, "./assets/ship/%sship%d", openClose, orientation);
        setSpritePath(localShip, path);
    }

    char* getSpritePath(Ship* localShip) {
        return localShip->spritePath;
    }

    void updateSprite(Ship* localShip) {
        double orientation = getDirection(localShip);

        if (orientation < 0 || orientation > 2*M_PI)
            return;
        else if (orientation < M_PI/4)
            setSpriteCompass(localShip, SHIP_NORTH);
        else if (orientation < M_PI/2)
            setSpriteCompass(localShip, SHIP_NORTH_EAST);
        else if (orientation < (3*M_PI)/4)
            setSpriteCompass(localShip, SHIP_EAST);
        else if (orientation < M_PI)
            setSpriteCompass(localShip, SHIP_SOUTH_EAST);
        else if (orientation < (5*M_PI)/4)
            setSpriteCompass(localShip, SHIP_SOUTH);
        else if (orientation < (3*M_PI)/2)
            setSpriteCompass(localShip, SHIP_SOUTH_WEST);
        else if (orientation < (7*M_PI)/4)
            setSpriteCompass(localShip, SHIP_WEST);
        else if (orientation < 2*M_PI)
            setSpriteCompass(localShip, SHIP_NORTH_WEST);
    }

    void sail(Ship* localShip, Wind* localWind) {
        double shipSpeed = getSpeed(localShip);
        int turnLevel = getTurnLevel(localShip);
        double windSpeed = getWindSpeed(localWind);
        double shipAngle = getDirection(localShip);
        double windAngle = getWindDirection(localWind);
        double xPos = getXPosition(localShip);
        double yPos = getYPosition(localShip);

        if (getSailStat(localShip) && shipSpeed < windSpeed) {
            shipSpeed += cos(windAngle-shipAngle);

            //double approachAngle = (3*M_PI)/4 - (windAngle-shipAngle);
            //shipAngle += (shipAngle-approachAngle)*approachAngle;
        }

        shipSpeed *= 0.985; // slow down with time, water resistance
        shipAngle += (shipSpeed/40)*(M_PI/12)*turnLevel;

        xPos += shipSpeed*sin(shipAngle);
        yPos += shipSpeed*cos(shipAngle);
        
        setXPosition(localShip, xPos);
        setYPosition(localShip, yPos);
        setXVelocity(localShip, shipSpeed*sin(shipAngle));
        setYVelocity(localShip, shipSpeed*cos(shipAngle));
        setDirection(localShip, shipAngle);
    }

#endif