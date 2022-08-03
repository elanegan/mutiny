/// @file mutiny.c
// Include libraries
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <panel.h>
// Include mutiny libraries
#include "./ship.h"
#include "./wind.h"
// Define file constants
#define BUFFER_SIZE 32
// Define colour constants
#define COLOR_BROWN 8
// Define ncurses text colour pair constants
#define BACKGROUND_PAIR 0
#define MAP_BORDER_PAIR 1
#define SEA_PAIR 2
#define INFO_PAIR 3
#define WHEEL_PAIR 4
#define SHIP_PAIR 5
#define NEGATIVE 6
#define POSITIVE 7
#define MEDIUM 8

// Define functions to be used globally
WINDOW* createNewWindow(int height, int width, int initY, int initX, int colPair);
void destroyWindow(WINDOW* localWin);
void setupColors();
void displaySprite(WINDOW* localWin, int yPos, int xPos, char fileName[], int colPair);
void displayPlayerInfo(WINDOW* localWin, Ship* localShip);
void displayShip(WINDOW* localWin, Ship* localShip);
void displayTurnStat(WINDOW* wheelWindow, Ship* player);
void displayWindInfo(WINDOW* infoWindow, Wind* localWind);
void resetCamera(WINDOW* seaWindow);

/** 
 * Main entry point for the program.
 */
int main(int argc, char* argv[]) {
    WINDOW* windows[3];
    PANEL* panels[3];

    int ch;

    // Initialise ncurses window
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(250);
    setupColors();

    // set up windows and panels with dynamic sizing
    int scrX, scrY;
    int spacing = 3;
    getmaxyx(stdscr, scrY, scrX);
    double availableX = scrX-(2*spacing);
    double availableY = scrY-(2*spacing);

    windows[0] = createNewWindow(availableY, (2*availableX)/3, spacing, spacing*2, SEA_PAIR);
    windows[1] = createNewWindow((availableY/2)-spacing, (availableX/3)-(3*spacing), spacing, ((2*availableX)/3)+(3*spacing), INFO_PAIR);
    windows[2] = createNewWindow(ceil(availableY/2), (availableX/3)-(3*spacing), (availableY/2)+spacing, ((2*availableX)/3)+(3*spacing), WHEEL_PAIR);

    // Create ncurses panels for all windows
    for (int i = 0; i < 3; i++) {
        panels[i] = new_panel(windows[i]);
    }

    // display steering wheel in centre of wheel window
    int wheelMaxX, wheelMaxY;
    getmaxyx(windows[2], wheelMaxY, wheelMaxX);
    displaySprite(windows[2], (wheelMaxY/2)-4, (wheelMaxX/2)-8, "./assets/wheel", WHEEL_PAIR);

    // display player ship in centre of the camera window
    int seaMaxX, seaMaxY;
    getmaxyx(windows[0], seaMaxY, seaMaxX);
    Ship* playerShip = createNewShip(0, 0, 0, (seaMaxY/2), (seaMaxX/2));
    displayShip(windows[0], playerShip);

    // set camera position on map
    int screenXPos = getDisplayXPosition(playerShip) - (seaMaxX/2);
    int screenYPos = getDisplayYPosition(playerShip) - (seaMaxY/2);

    // create wind
    Wind* wind = createNewWind();

    // update all windows
    update_panels();
    doupdate();

    // game loop
    while (ch != KEY_F(1)) {
        // Clear the screen for displaying sprites
        resetCamera(windows[0]);

        // Update and display wind info
        updateWind(wind);
        displayWindInfo(windows[1], wind);

        // Move player and display sprite and (debug) position info 
        sail(playerShip, wind);
        mvwprintw(windows[1], 10, 2, "X: %.0f; Y: %.0f ", getXPosition(playerShip), getYPosition(playerShip));
        updateSprite(playerShip);

        // Update camera position
        screenXPos = -(getXPosition(playerShip) + (seaMaxX/2));
        screenYPos = -(getYPosition(playerShip) + (seaMaxY/2));
        displayShip(windows[0], playerShip);
        displayPlayerInfo(windows[1], playerShip);
        displayTurnStat(windows[2], playerShip);

        // Replace border for camera
        wattron(windows[0],COLOR_PAIR(MAP_BORDER_PAIR));
        wborder(windows[0], '|', '|', '-','-','+','+','+','+');
        wattroff(windows[0],COLOR_PAIR(MAP_BORDER_PAIR));

        // Update all windows with new info
        update_panels();
        doupdate();

        // Get input from player for controls
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

/**
 * Creates and colours a new ncurses window with a border.
 * @param[in] height The height of the window.
 * @param[in] width The width of the window.
 * @param[in] initY The Y position on the main screen to start the window.
 * @param[in] initX The X position on the main screen to start the window.
 * @param[in] colPair The colour pair to use for the background of the window.
 * @param[out] localWin A pointer to an ncurses window.
 */
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

/**
 * Destroys an ncurses window.
 * @param[in] localWin The window to be destroyed.
 */
void destroyWindow(WINDOW* localWin) {
    wborder(localWin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(localWin);
    delwin(localWin);
}

/**
 * Initialises colours and colour pairs.
 */
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

/**
 * Displays a sprite from a text file on a window.
 * @param[in] localWin The window to display the sprite on.
 * @param[in] yPos The Y position to display the sprite on the window.
 * @param[in] xPos The X position to display the sprite on the window.
 * @param[in] fileName The path to the sprite file.
 * @param[in] colPair The colour pair to use for the sprite.
 */
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

/**
 * Displays information about the player ship.
 * @param[in] localWin The window to display the information on.
 * @param[in] localShip The ship to display information about.
 */
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

/**
 * Displays a ship on the window.
 * @param[in] seaWindow The window to display the ship on.
 * @param[in] localship The ship to display on the window.
 */
void displayShip(WINDOW* seaWindow, Ship* localShip) {
    char sprite[64];
    strcpy(sprite, localShip->spritePath);

    // get relative position to place on screen, and shift for sprite size
    int xPos = getDisplayXPosition(localShip)-8;
    int yPos = getDisplayYPosition(localShip)-4;

    displaySprite(seaWindow, yPos, xPos, sprite, SHIP_PAIR);
}

/**
 * Displays the turn status for the player.
 * @param[in] wheelWindow The window to display the turn status on.
 * @param[in] player The player ship.
 */
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

/**
 * Displays information about the wind.
 * @param[in] infoWindow The window to display the information on.
 * @param[in] localwind The wind to display information about.
 */
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

/**
 * Resets the camera window for sprite display.
 * @param[in] seaWindow The camera window.
 */
void resetCamera(WINDOW* seaWindow) {
    // remove everything from screen
    wclear(seaWindow);

    // reset background colours
    wbkgd(seaWindow, COLOR_PAIR(SEA_PAIR));

    wrefresh(seaWindow);
}