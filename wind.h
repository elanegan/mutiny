#ifndef WIND
#include <math.h>
#include <time.h>
#define WIND

    typedef struct Wind {
        double direction; // wind direction in radians (0-2pi)
        double speed; // absolute speed of wind
    } Wind;

    Wind* createNewWind() {
        Wind* wind = malloc(sizeof(Wind));
        wind->direction = 35.0;
        wind->speed = 15.0;
        srand(time(NULL));

        return(wind);
    }

    void destroyWind(Wind* localWind) {
        free(localWind);
    }

    double getWindDirection(Wind* localWind) {
        return(localWind->direction);
    }

    double getWindSpeed(Wind* localWind) {
        return(localWind->speed);
    }

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
        
        if (currentSpeed > 20) {
            currentSpeed = 19.8;
        }

        localWind->direction = currentDirection;
        localWind->speed = currentSpeed;
        
    }

#endif