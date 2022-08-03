/// @file wind.h
#ifndef WIND
#include <math.h>
#include <time.h>
#define WIND

    #define SPEED 1.5 ///< Wind speed typical maximum

    /// Data type for wind
    typedef struct Wind {
        double direction; // wind direction in radians (0-2pi)
        double speed; // absolute speed of wind
    } Wind;

    /**
     * Function to create new wind.
     * @param[out] wind A pointer to a new wind type.
     */
    Wind* createNewWind() {
        Wind* wind = malloc(sizeof(Wind));
        wind->direction = 0;
        wind->speed = SPEED;
        srand(time(NULL));

        return(wind);
    }

    /**
     * Destroys a wind object.
     * @param[in] localWind The wind to destroy.
     */
    void destroyWind(Wind* localWind) {
        free(localWind);
    }

    /**
     * Gets the direction of the wind.
     * @param[in] localWind The wind to act on.
     * @param[out] direction The direction of the wind in radians.
     */
    double getWindDirection(Wind* localWind) {
        return(localWind->direction);
    }

    /**
     * Gets the absolute speed of the wind.
     * @param[in] localWind The wind to act on.
     * @param[out] speed The absolute speed of the wind.
     */
    double getWindSpeed(Wind* localWind) {
        return(localWind->speed);
    }

    /**
     * Update function to vary direction and speed of wind.
     * @param[in] localWind The wind to act on.
     */
    void updateWind(Wind* localWind) {
        double currentSpeed = getWindSpeed(localWind);
        double currentDirection = getWindDirection(localWind);
        int changeSpeed = rand() % 20;
        int changeDirection = rand() % 30;
        //printf("%d %d, ", changeSpeed, changeDirection);

        switch(changeSpeed) {
            case(1):
                currentSpeed += 0.01;
                currentSpeed *= 1.47;
                break;
            case(4):
                currentSpeed *= 0.77;
                break;
        }
        switch(changeDirection) {
            case(3):
                currentDirection += M_PI/8;
                break;
            case(7):
                currentDirection *= 1.23;
                break;
            case(1):
                currentDirection -= M_PI/16;
        }

        if (currentDirection > (2*M_PI) || currentDirection < 0.0)
            currentDirection = 0.0;
        
        if (currentSpeed > 1.4*SPEED) {
            currentSpeed = 1.3*SPEED;
        }

        localWind->direction = currentDirection;
        localWind->speed = currentSpeed;
        
    }

#endif