#include "main.h"
#include "MenuMan.h"

int main()
{
    WINDOW *menu_win, *focus_win, *console_win;
    int highlight = 1; // Selected entry

    // ----- Init Window -----

    initscr();
    curs_set(0); // Hide cursor
    start_color();

    // Initialize color pairs for priorities
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);

    clear();
    noecho();
    cbreak(); // Line buffering disabled. pass on everything

    focus_win = newwin(7, 100, 1, 1);
    menu_win = newwin(30, 100, 9, 1);
    console_win = newwin(10, 100, 40, 1);
    keypad(menu_win, TRUE); // Enables arrow keys
    ScreenState current_screen = TASK_SCREEN;

    // ----- Create data -----

    // Init SQL
    sqlite3 *db;
    InitSQL(&db);

    // Init list
    DoublyLinkedList *list = create_list();
    size_t *n_tasks = &list->size;
    EatSQL(list, db);

    // ----- Output to terminal -----

    while (1)
    {
        // Clear the windows
        wclear(focus_win);
        wclear(menu_win);
        wclear(console_win);

        // Outline windows
        wattron(focus_win, COLOR_PAIR(6));
        box(focus_win, 0, 0);
        wattroff(focus_win, COLOR_PAIR(6));

        box(menu_win, 0, 0);

        wattron(console_win, COLOR_PAIR(1));
        box(console_win, 0, 0);
        wattroff(console_win, COLOR_PAIR(1));

        // Refresh windows
        wrefresh(focus_win);
        wrefresh(menu_win);
        wrefresh(console_win);

        // Menu window state machine
        switch (current_screen)
        {
        case TASK_SCREEN:
            current_screen = TaskScreen(menu_win, list, &highlight, n_tasks);
            break;
        case ADD_TASK_SCREEN:
            current_screen = AddTaskScreen(menu_win, list, db);
            break;
        }
    }

    wprintw(console_win, "Exiting...");
    clrtoeol();
    refresh();
    getch();
    endwin();
    return 0;
}
