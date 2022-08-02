#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <panel.h>

#include "./ship.h"
#include "./wind.h"

#define BUFFER_SIZE 32

#define COLOR_BROWN 8
#define BACKGROUND_PAIR 0
#define MAP_BORDER_PAIR 1
#define SEA_PAIR 2
#define INFO_PAIR 3
#define WHEEL_PAIR 4
#define SHIP_PAIR 5
#define NEGATIVE 6
#define POSITIVE 7
#define MEDIUM 8

WINDOW* createNewWindow(int height, int width, int initY, int initX, int colPair);
void destroyWindow(WINDOW* localWin);
void setupColors();
void displaySprite(WINDOW* localWin, int yPos, int xPos, char fileName[], int colPair);
void displayPlayerInfo(WINDOW* localWin, Ship* localShip);
void displayShip(WINDOW* localWin, Ship* localShip);
void displayTurnStat(WINDOW* wheelWindow, Ship* player);
void displayWindInfo(WINDOW* infoWindow, Wind* localWind);
void resetCamera(WINDOW* seaWindow);

int main(int argc, char* argv[]) {

    WINDOW* windows[3];
    PANEL* panels[3];

    int ch;

    // initialisation
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(250);
    setupColors();

    // set up windows and panels with dynamic sizing
    int scrX, scrY;
    getmaxyx(stdscr, scrY, scrX);

    int spacing = 3;
    double availableX = scrX-(2*spacing);
    double availableY = scrY-(2*spacing);

    windows[0] = createNewWindow(availableY, (2*availableX)/3, spacing, spacing*2, SEA_PAIR);
    windows[1] = createNewWindow((availableY/2)-spacing, (availableX/3)-(3*spacing), spacing, ((2*availableX)/3)+(3*spacing), INFO_PAIR);
    windows[2] = createNewWindow(ceil(availableY/2), (availableX/3)-(3*spacing), (availableY/2)+spacing, ((2*availableX)/3)+(3*spacing), WHEEL_PAIR);

    for (int i = 0; i < 3; i++) {
        panels[i] = new_panel(windows[i]);
    }

    // display steering wheel in centre of wheel screen
    int wheelMaxX, wheelMaxY;
    getmaxyx(windows[2], wheelMaxY, wheelMaxX);

    displaySprite(windows[2], (wheelMaxY/2)-4, (wheelMaxX/2)-8, "./assets/wheel", WHEEL_PAIR);

    // display player ship in centre of the main playing screen
    int seaMaxX, seaMaxY;
    getmaxyx(windows[0], seaMaxY, seaMaxX);
    Ship* playerShip = createNewShip(0, 0, 0, (seaMaxY/2), (seaMaxX/2));
    displayShip(windows[0], playerShip);

    // set screen position on map
    int screenXPos = getDisplayXPosition(playerShip) - (seaMaxX/2);
    int screenYPos = getDisplayYPosition(playerShip) - (seaMaxY/2);

    // display ship off camera
    Ship* otherShip = createNewShip(1, -30, 10, 0, 0);
    updateDisplayPosition(otherShip, screenYPos, screenXPos);

    // create wind
    Wind* wind = createNewWind();

    // updates
    update_panels();
    doupdate();

    // display changing ship sprite
    while (ch != KEY_F(1)) {
        resetCamera(windows[0]);
        updateWind(wind);
        sail(playerShip, wind);
        displayWindInfo(windows[1], wind);
        mvwprintw(windows[1], 10, 2, "X: %.0f; Y: %.0f ", getXPosition(playerShip), getYPosition(playerShip));
        updateSprite(playerShip);

        // update camera position
        screenXPos = -(getXPosition(playerShip) + (seaMaxX/2));
        screenYPos = -(getYPosition(playerShip) + (seaMaxY/2));
        displayShip(windows[0], playerShip);
        updateDisplayPosition(otherShip, screenYPos, screenXPos);
        displayShip(windows[0], otherShip);
        displayPlayerInfo(windows[1], playerShip);
        displayTurnStat(windows[2], playerShip);

        // replace border for camera
        wattron(windows[0],COLOR_PAIR(MAP_BORDER_PAIR));
        wborder(windows[0], '|', '|', '-','-','+','+','+','+');
        wattroff(windows[0],COLOR_PAIR(MAP_BORDER_PAIR));

        update_panels();
        doupdate();

        ch = getch();
        switch(ch) {
            case(KEY_RIGHT):
                incStarboardTurn(playerShip);
                break;
            case(KEY_LEFT):
                incPortTurn(playerShip);
                break;
            case('7'):
                switchSails(playerShip);
                break;
        }
    }
    endwin();

    return 0;
}

WINDOW* createNewWindow(int height, int width, int initY, int initX, int colPair) {
    WINDOW* localWin;

    localWin = newwin(height, width, initY, initX);
    wattron(localWin,COLOR_PAIR(MAP_BORDER_PAIR));
    wborder(localWin, '|', '|', '-','-','+','+','+','+');
    wattroff(localWin,COLOR_PAIR(MAP_BORDER_PAIR));

    wbkgd(localWin, COLOR_PAIR(colPair));
    
    wrefresh(localWin);

    return localWin;
}

void destroyWindow(WINDOW* localWin) {
    wborder(localWin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(localWin);
    delwin(localWin);
}

void setupColors() {
    start_color();

    init_color(COLOR_BROWN, 500, 160, 0);

    init_pair(MAP_BORDER_PAIR,COLOR_YELLOW, COLOR_BLACK);
    init_pair(SEA_PAIR, COLOR_WHITE, COLOR_BLUE);
    init_pair(INFO_PAIR, COLOR_BLUE, COLOR_BLACK);
    init_pair(WHEEL_PAIR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(SHIP_PAIR, COLOR_BROWN, COLOR_BLUE);
    init_pair(NEGATIVE, COLOR_RED, COLOR_BLACK);
    init_pair(MEDIUM, COLOR_YELLOW, COLOR_BLACK);
    init_pair(POSITIVE, COLOR_GREEN, COLOR_BLACK);
}

void displaySprite(WINDOW* localWin, int yPos, int xPos, char fileName[], int colPair) {
    int yPosition = yPos;
    int xPosition = xPos;

    FILE* fileHandle = fopen(fileName, "r");
    if (fileHandle == NULL) {
        printw("Unable to open file.\n");
        return;
    }
    
    char line[BUFFER_SIZE];
    char* result;

    result = fgets(line, BUFFER_SIZE, fileHandle);
    line[strcspn(line, "\n")] = 0;
    while (result != NULL) {
        wattron(localWin, COLOR_PAIR(colPair));
        mvwprintw(localWin, yPosition, xPosition, "%s", result);
        wattroff(localWin, COLOR_PAIR(colPair));
        yPosition++;
        result = fgets(line, BUFFER_SIZE, fileHandle);
        line[strcspn(line, "\n")] = 0;
    }

    fclose(fileHandle);
}

void displayPlayerInfo(WINDOW* localWin, Ship* localShip) {
    int healthBarSize = (int) (getHealth(localShip)/5);

    // start health bar
    mvwprintw(localWin, 2, 2, "Health: [");

    // set colour for health bar
    if (healthBarSize > 14)
        wattron(localWin, COLOR_PAIR(POSITIVE));
    else if (healthBarSize > 7)
        wattron(localWin, COLOR_PAIR(MEDIUM));
    else if (healthBarSize > 0)
        wattron(localWin, COLOR_PAIR(NEGATIVE));

    // display health bar amount
    for (int i = 0; i < healthBarSize; i++) {
        mvwprintw(localWin, 2, (i+11), "#");
    }
    // switch off formatting
    wattroff(localWin, COLOR_PAIR(POSITIVE));
    wattroff(localWin, COLOR_PAIR(MEDIUM));
    wattroff(localWin, COLOR_PAIR(NEGATIVE));
    // end health bar
    mvwprintw(localWin, 2, 31, "]");

    mvwprintw(localWin, 4, 2, "Speed: %.1f Knots", getSpeed(localShip));

    if(getSailStat(localShip)) {
        wattron(localWin, COLOR_PAIR(POSITIVE));
        mvwprintw(localWin, 6, 2, "Sails: OPEN");
        wattroff(localWin, COLOR_PAIR(POSITIVE));
    }
    else {
        wattron(localWin, COLOR_PAIR(NEGATIVE));
        mvwprintw(localWin, 6, 2, "Sails: DOWN");
        wattroff(localWin, COLOR_PAIR(NEGATIVE));
    }
}

void displayShip(WINDOW* seaWindow, Ship* localShip) {
    char sprite[64];
    strcpy(sprite, localShip->spritePath);

    // get relative position to place on screen, and shift for sprite size
    int xPos = getDisplayXPosition(localShip)-8;
    int yPos = getDisplayYPosition(localShip)-4;

    displaySprite(seaWindow, yPos, xPos, sprite, SHIP_PAIR);
}

void displayTurnStat(WINDOW* wheelWindow, Ship* player) {
    int turnLevel = getTurnLevel(player);
    int maxX, maxY;
    getmaxyx(wheelWindow, maxY, maxX);

    // reset
    mvwprintw(wheelWindow, (maxY/2)+5, (maxX/2)-6, " ");
    mvwprintw(wheelWindow, (maxY/2), (maxX/2)-10, " ");
    mvwprintw(wheelWindow, (maxY/2)-5, (maxX/2)-6, " ");
    mvwprintw(wheelWindow, (maxY/2)-6, (maxX/2), " ");
    mvwprintw(wheelWindow, (maxY/2)-5, (maxX/2)+6, " ");
    mvwprintw(wheelWindow, (maxY/2), (maxX/2)+10, " ");
    mvwprintw(wheelWindow, (maxY/2)+5, (maxX/2)+6, " ");
    
    // replace
    wattron(wheelWindow, A_BOLD);
    switch(turnLevel) {
        case(-3):
            mvwprintw(wheelWindow, (maxY/2)+5, (maxX/2)-6, "#");
            break;
        case(-2):
            mvwprintw(wheelWindow, (maxY/2), (maxX/2)-10, "#");
            break;
        case(-1):
            mvwprintw(wheelWindow, (maxY/2)-5, (maxX/2)-6, "#");
            break;
        case(0):
            mvwprintw(wheelWindow, (maxY/2)-6, (maxX/2), "#");
            break;
        case(1):
            mvwprintw(wheelWindow, (maxY/2)-5, (maxX/2)+6, "#");
            break;
        case(2):
            mvwprintw(wheelWindow, (maxY/2), (maxX/2)+10, "#");
            break;
        case(3):
            mvwprintw(wheelWindow, (maxY/2)+5, (maxX/2)+6, "#");
            break;
    }
    wattroff(wheelWindow, A_BOLD);
}

void displayWindInfo(WINDOW* infoWindow, Wind* localWind) {
        double orientation = getWindDirection(localWind);
        char text[16] = "!! ";

        if (orientation < 0 || orientation > 2*M_PI)
            strcpy(text, "!! ");
        else if (orientation < M_PI/4)
            strcpy(text, "N ");
        else if (orientation < M_PI/2)
            strcpy(text, "NE ");
        else if (orientation < (3*M_PI)/4)
            strcpy(text, "E ");
        else if (orientation < M_PI)
            strcpy(text, "SE ");
        else if (orientation < (5*M_PI)/4)
            strcpy(text, "S ");
        else if (orientation < (3*M_PI)/2)
            strcpy(text, "SW ");
        else if (orientation < (7*M_PI)/4)
            strcpy(text, "W ");
        else if (orientation < 2*M_PI)
            strcpy(text, "NW ");
        
    mvwprintw(infoWindow, 8, 2, "Wind: %.1f Knots %s ", getWindSpeed(localWind), text);
}

void resetCamera(WINDOW* seaWindow) {
    // remove everything from screen
    wclear(seaWindow);

    // reset background colours
    wbkgd(seaWindow, COLOR_PAIR(SEA_PAIR));

    wrefresh(seaWindow);
}