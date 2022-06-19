#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <panel.h>

#include "./ship.h"

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
    setupColors();

    // set up windows and panels
    windows[0] = createNewWindow(50, 150, 4, 4, SEA_PAIR);
    windows[1] = createNewWindow(25, 50, 4, 156, INFO_PAIR);
    windows[2] = createNewWindow(25, 50, 29, 156, WHEEL_PAIR);

    for (int i = 0; i < 3; i++) {
        panels[i] = new_panel(windows[i]);
    }

    // display steering wheel
    displaySprite(windows[2], 16, 8, "./assets/wheel", WHEEL_PAIR);

    Ship* playerShip = createNewShip(0, 70, 18);

    displayShip(windows[0], playerShip);

    // updates
    update_panels();
    doupdate();

    // display changing ship sprite
    while (ch != KEY_F(1)) {
        updateSprite(playerShip);
        displayShip(windows[0], playerShip);
        displayPlayerInfo(windows[1], playerShip);
        displayTurnStat(windows[2], playerShip);
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
    int yPosition = xPos;
    int xPosition = yPos;

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
    int xPos = (int)getXPosition(localShip);
    int yPos = (int)getYPosition(localShip);

    displaySprite(seaWindow, yPos, xPos, sprite, SHIP_PAIR);
}

void displayTurnStat(WINDOW* wheelWindow, Ship* player) {
    int turnLevel = getTurnLevel(player);

    // reset
    mvwprintw(wheelWindow, 17, 17, " ");
    mvwprintw(wheelWindow, 12, 14, " ");
    mvwprintw(wheelWindow, 7, 17, " ");
    mvwprintw(wheelWindow, 6, 24, " ");
    mvwprintw(wheelWindow, 7, 31, " ");
    mvwprintw(wheelWindow, 12, 34, " ");
    mvwprintw(wheelWindow, 17, 31, " ");
    
    // replace
    wattron(wheelWindow, A_BOLD);
    switch(turnLevel) {
        case(-3):
            mvwprintw(wheelWindow, 17, 17, "#");
            break;
        case(-2):
            mvwprintw(wheelWindow, 12, 14, "#");
            break;
        case(-1):
            mvwprintw(wheelWindow, 7, 17, "#");
            break;
        case(0):
            mvwprintw(wheelWindow, 6, 24, "#");
            break;
        case(1):
            mvwprintw(wheelWindow, 7, 31, "#");
            break;
        case(2):
            mvwprintw(wheelWindow, 12, 34, "#");
            break;
        case(3):
            mvwprintw(wheelWindow, 17, 31, "#");
            break;
    }
    wattroff(wheelWindow, A_BOLD);
}