/// @file ship.h
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

    /// Data type for a ship
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

    /** 
     * Function to create a new ship.
     * @param[in] identifier A unique identifier for the ship.
     * @param[in] mapY The initial Y position for the ship on the map.
     * @param[in] mapX The initial X position for the ship on the map.
     * @param[in] screenY The initial Y position for the ship on the camera window.
     * @param[in] screenX The initial X position for the ship on the camera window.
     * @param[out] ship A pointer to the new ship.
     */
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

    /**
     * Destroys a ship object.
     * @param[in] localShip The ship to be destroyed.
     */
    void destroyShip(Ship* localShip) {
        free(localShip);
    }

    /**
     * Sets the health of a ship.
     * @param[in] localShip The ship to act on.
     * @param[in] newHealth The new health level to set for the ship.
     */
    void setHealth(Ship* localShip, double newHealth) {
        localShip->health = newHealth;
    }

    /**
     * Gets the health of a ship.
     * @param[in] localShip The ship to act on.
     * @param[out] health The health level of the ship.
     */
    double getHealth(Ship* localShip) {
        return(localShip->health);
    }

    /**
     * Sets a ship's X position on the map.
     * @param[in] localShip The ship to act on.
     * @param[in] newXPosition The new X position to set for the ship.
     */
    void setXPosition(Ship* localShip, double newXPosition) {
        localShip->xPosition = newXPosition;
    }

    /**
     * Gets a ship's X position on the map.
     * @param[in] localShip The ship to act on.
     * @param[out] xPosition The X position of the ship.
     */
    double getXPosition(Ship* localShip) {
        return(localShip->xPosition);
    }

    /**
     * Sets a ship's Y position on the map.
     * @param[in] localShip The ship to act on.
     * @param[in] newYPosition The new Y position to set for the ship.
     */
    void setYPosition(Ship* localShip, double newYPosition) {
        localShip->yPosition = newYPosition;
    }

    /**
     * Gets a ship's Y position on the map.
     * @param[in] localShip The ship to act on.
     * @param[out] yPosition The Y position of the ship.
     */
    double getYPosition(Ship* localShip) {
        return(localShip->yPosition);
    }

    /**
     * Sets a ship's X position on the camera window.
     * @param[in] localShip The ship to act on.
     * @param[in] newXPosition The new X position to set on the camera window.
     */
    void setDisplayXPosition(Ship* localShip, int newXPosition) {
        localShip->displayXPosition = newXPosition;
    }

    /**
     * Gets a ship's X position on the camera window.
     * @param[in] localShip The ship to act on.
     * @param[out] displayXPosition The X position of the ship on the camera window.
     */
    int getDisplayXPosition(Ship* localShip) {
        return(localShip->displayXPosition);
    }

    /**
     * Sets a ship's Y position on the camera window.
     * @param[in] localShip The ship to act on.
     * @param[in] newYPosition The new Y position to set on the camera window.
     */
    void setDisplayYPosition(Ship* localShip, int newYPosition) {
        localShip->displayYPosition = newYPosition;
    }

    /**
     * Gets a ship's Y position on the camera window.
     * @param[in] localShip The ship to act on.
     * @param[out] displayYPosition The Y position of the ship on the camera window.
     */
    int getDisplayYPosition(Ship* localShip) {
        return(localShip->displayYPosition);
    }

    /**
     * Sets the display position of a ship based on where the camera window has visibility.
     * @param[in] localShip The ship to act on.
     * @param[in] screenYPos The Y position of the camera on the map.
     * @param[in] screenXPos The X position of the camera on the map.
     */
    void updateDisplayPosition(Ship* localShip, int screenYPos, int screenXPos) {
        int newX = getXPosition(localShip) - screenXPos;
        int newY = getYPosition(localShip) - screenYPos;
        setDisplayXPosition(localShip, newX);
        setDisplayYPosition(localShip, newY);
    }

    /**
     * Sets a ship's direction of travel.
     * Ranges between 0 and 2Pi radians.
     * @param[in] localShip The ship to act on.
     * @param[in] newDirection The new direction for the ship.
     */
    void setDirection(Ship* localShip, double newDirection) {
        while (newDirection < 0) {
            newDirection += (2*M_PI);
        }
        while (newDirection > 2*M_PI) {
            newDirection -= (2*M_PI);
        }

        localShip->direction = newDirection;
    }

    /**
     * Gets a ship's direction of travel.
     * Ranges between 0 and 2Pi radians.
     * @param[in] localShip The ship to act on.
     * @param[out] direction The direction of travel for the ship, in radians.
     */
    double getDirection(Ship* localShip) {
        return(localShip->direction);
    }

    /**
     * Sets a ship's X velocity on the map.
     * @param[in] localShip The ship to act on.
     * @param[in] newVelocity The new X velocity to set for the ship.
     */
    void setXVelocity(Ship* localShip, double newVelocity) {
        localShip->xVelocity = newVelocity;
    }

    /**
     * Gets a ship's X velocity on the map.
     * @param[in] localShip The ship to act on.
     * @param[out] xVelocity The X velocity of the ship.
     */
    double getXVelocity(Ship* localShip) {
        return(localShip->xVelocity);
    }

    /**
     * Sets a ship's Y velocity on the map.
     * @param[in] localShip The ship to act on.
     * @param[in] newVelocity The new Y velocity to set for the ship.
     */
    void setYVelocity(Ship* localShip, double newVelocity) {
        localShip->yVelocity = newVelocity;
    }

    /**
     * Gets a ship's Y velocity on the map.
     * @param[in] localShip The ship to act on.
     * @param[out] yVelocity The Y velocity of the ship.
     */
    double getYVelocity(Ship* localShip) {
        return(localShip->yVelocity);
    }

    /**
     * Gets the absolute speed of a ship.
     * @param[in] localShip The ship to act on.
     * @param[out] speed The absolute speed of the ship.
     */
    double getSpeed(Ship* localShip) {
        double xSpeed = fabs(getXVelocity(localShip));
        double ySpeed = fabs(getYVelocity(localShip));
        double speed = sqrt((xSpeed*xSpeed) + (ySpeed*ySpeed));

        return speed;
    }

    // Port = anticlockwise turn
    /**
     * Increases the Port side turn of a ship.
     * This corresponds to an anticlockwise turn.
     * @param[in] localShip The ship to act on.
     */
    void incPortTurn(Ship* localShip) {
        if(localShip->turnLevel == -3)
            return;
        localShip->turnLevel--;
    }

    // Starboard = clockwise turn
    /**
     * Increases the Starboard side turn of a ship.
     * This corresponds to a clockwise turn.
     * @param[in] localShip The ship to act on.
     */
    void incStarboardTurn(Ship* localShip) {
        if(localShip->turnLevel == 3)
            return;
        localShip->turnLevel++;
    }

    /**
     * Gets the turn level of a ship.
     * Ranges from -3 to 3.
     * @param[in] localShip The ship to act on.
     * @param[out] turnLevel The turn level of the ship.
     */
    int getTurnLevel(Ship* localShip) {
        return(localShip->turnLevel);
    }

    /**
     * Sets a ship's firing rate.
     * @param[in] localShip The ship to act on.
     * @param[in] newFiringSpeed The new rate at which to fire.
     */
    void setFiringSpeed(Ship* localShip, double newFiringSpeed) {
        localShip->firingSpeed = newFiringSpeed;
    }

    /**
     * Gets a ship's firing rate.
     * @param[in] localShip The ship to act on.
     * @param[out] firingSpeed The rate of fire for the ship.
     */
    double getFiringSpeed(Ship* localShip) {
        return(localShip->firingSpeed);
    }

    /**
     * Toggles a ship's sails.
     * @param[in] localShip The ship to act on.
     */
    void switchSails(Ship* localShip) {
        localShip->sailsOpen = !(localShip->sailsOpen);
    }

    /**
     * Gets a ship's sail status.
     * @param[in] localShip The ship to act on.
     * @param[out] sailsOpen Whether the ship's sails are open.
     */
    int getSailStat(Ship* localShip) {
        return(localShip->sailsOpen);
    }

    /**
     * Sets a ship's sprite file path.
     * @param[in] localShip The ship to act on.
     * @param[in] newPath The new sprite file path to set.
     */
    void setSpritePath(Ship* localShip, char* newPath) {
        strcpy(localShip->spritePath, newPath);
    }

    /**
     * Sets a ship's sprite based on its direction of travel.
     * @param[in] localShip The ship to act on.
     * @param[in] orientation The orientation level of the ship.
     */
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

    /**
     * Gets a ship's sprite file path.
     * @param[in] localShip The ship to act on.
     * @param[out] spritePath The sprite file path of the ship.
     */
    char* getSpritePath(Ship* localShip) {
        return localShip->spritePath;
    }

    /**
     * Updates a ship's sprite based on its direction of travel.
     * @param[in] localShip The ship to act on.
     */
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

    /**
     * Simulate the sailing movement of a ship in the wind.
     * @param localShip The ship to act on.
     * @param localWind The wind acting on the ship.
     */
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

        shipSpeed *= 0.95; // slow down with time, water resistance
        shipAngle += (shipSpeed/8)*(M_PI/12)*turnLevel;

        xPos += shipSpeed*sin(shipAngle);
        yPos += shipSpeed*cos(shipAngle);
        
        setXPosition(localShip, xPos);
        setYPosition(localShip, yPos);
        setXVelocity(localShip, shipSpeed*sin(shipAngle));
        setYVelocity(localShip, shipSpeed*cos(shipAngle));
        setDirection(localShip, shipAngle);
    }

#endif