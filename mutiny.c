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

WINDOW* createNewWindow(int height, int width, int initY, int initX, int colPair);
void destroyWindow(WINDOW* localWin);
void setupColors();
void displaySprite(WINDOW* localWin, Ship* localShip, int colPair);

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

    // updates
    update_panels();
    doupdate();

    Ship* newship = createNewShip(0, 25, 75);
    printw("newship: %s", newship->spritePath);

    // display changing ship sprite
    while (ch != KEY_F(1)) {
        ch = getch();
        switch(ch) {
            case(KEY_UP):
                setSpriteCompass(newship, SHIP_NORTH);
                displaySprite(windows[0], newship, SHIP_PAIR);
                break;
            case(KEY_RIGHT):
                setSpriteCompass(newship, SHIP_EAST);
                displaySprite(windows[0], newship, SHIP_PAIR);
                break;
            case(KEY_DOWN):
                setSpriteCompass(newship, SHIP_SOUTH);
                displaySprite(windows[0], newship, SHIP_PAIR);
                break;
            case(KEY_LEFT):
                setSpriteCompass(newship, SHIP_WEST);
                displaySprite(windows[0], newship, SHIP_PAIR);
                break;
        }
        update_panels();
        doupdate();
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
}

void displaySprite(WINDOW* localWin, Ship* localShip, int colPair) {
    char fileName[16];
    printw("%s", localShip->spritePath);
    strcpy(fileName, localShip->spritePath);
    printw("fileName: %s ", fileName);
    int xPosition = getXPosition(localShip);
    int yPosition = getYPosition(localShip);

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