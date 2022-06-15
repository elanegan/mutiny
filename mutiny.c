#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <panel.h>

#define BUFFER_SIZE 32
#define BACKGROUND_PAIR 0
#define MAP_BORDER_PAIR 1
#define SEA_PAIR 2
#define INFO_PAIR 3
#define WHEEL_PAIR 4

WINDOW* createNewWindow(int height, int width, int initY, int initX, int colPair);
void destroyWindow(WINDOW* localWin);
void setupColors();
void displaySprite(WINDOW* localWin, int yPos, int xPos, char* fileName);

int main(int argc, char* argv[]) {
    WINDOW* mainWindow;
    WINDOW* infoWindow;
    WINDOW* wheelWindow;

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
/*
    %--.../    ,
    \    /`-. /
     \  /@   /`.
      `/    @   )
      ' `-./   ,'
          / ```
*/
    /* mvwprintw(windows[0], 10, boatx, "%%--.../    ,");
    mvwprintw(windows[0], 11, boatx, "\\    /`-. /");
    mvwprintw(windows[0], 12, boatx, " \\  /@   /`.");
    mvwprintw(windows[0], 13, boatx, "  `/    @   )");
    mvwprintw(windows[0], 14, boatx, "  ' `-./   ,'");
    mvwprintw(windows[0], 15, boatx, "      / ```"); */

    displaySprite(windows[0], 10, 75, "./assets/ship");

    // updates
    update_panels();
    doupdate();

    while (ch != KEY_F(1))
        ch = getch();
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

    init_pair(MAP_BORDER_PAIR,COLOR_YELLOW, COLOR_BLACK);
    init_pair(SEA_PAIR, COLOR_WHITE, COLOR_BLUE);
    init_pair(INFO_PAIR, COLOR_BLUE, COLOR_BLACK);
    init_pair(WHEEL_PAIR, COLOR_YELLOW, COLOR_BLACK);
}

void displaySprite(WINDOW* localWin, int yPos, int xPos, char* fileName) {
    FILE* fileHandle = fopen("./assets/ship", "r");
    if (fileHandle == NULL) {
        printw("Unable to open file.\n");
        return;
    }
    
    char line[BUFFER_SIZE];
    char* result;

    int yPosition = yPos;
    result = fgets(line, BUFFER_SIZE, fileHandle);
    line[strcspn(line, "\n")] = 0;
    while (result != NULL) {
        mvwprintw(localWin, yPosition, xPos, "%s", result);
        yPosition++;
        result = fgets(line, BUFFER_SIZE, fileHandle);
        line[strcspn(line, "\n")] = 0;
    }

    fclose(fileHandle);
}