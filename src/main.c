#include "main.h"
#include "MenuMan.h"

int main()
{
    WINDOW *menu_win, *focus_win;

    // ----- Init Window -----

    initscr();
    curs_set(0); // Hide cursor
    start_color();

    // Initialize color pairs for priorities
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);

    clear();
    noecho();
    cbreak(); // Line buffering disabled. pass on everything

    focus_win = newwin(8, 100, 1, 1);
    menu_win = newwin(30, 100, 9, 1);

    console_win = newwin(12, 100, 39, 1);
    console_enabled = true; // Enables console logging
    wattron(console_win, COLOR_PAIR(1));
    // box(console_win, 0, 0);
    wattroff(console_win, COLOR_PAIR(1));

    keypad(menu_win, TRUE); // Enables arrow keys
    ScreenState current_screen = TASK_SCREEN;

    // ----- Create data -----

    // Init SQL
    sqlite3 *db;
    InitSQL(&db);

    LOG_INFO("Finished creating database");

    // Init list
    DoublyLinkedList *list = create_list();
    size_t *n_tasks = &list->size;
    LOG_INFO("Finished creating DLL");

    EatSQL(list, db);
    LOG_INFO("Grabbed %d tasks from SQL database!", *n_tasks);

    Node *highlight = list->head;   // Currently selected node

    // ----- Output to terminal -----
    while (1)
    {
        // Clear the windows
        wclear(focus_win);
        wclear(menu_win);

        // Outline windows
        wattron(focus_win, COLOR_PAIR(6));
        box(focus_win, 0, 0);
        wattroff(focus_win, COLOR_PAIR(6));

        box(menu_win, 0, 0);

        // Refresh windows
        wrefresh(focus_win);
        wrefresh(menu_win);

        // Menu window state machine
        switch (current_screen)
        {
        case TASK_SCREEN:
            current_screen = TaskScreen(menu_win, db, list, highlight, n_tasks);
            break;
        case ADD_TASK_SCREEN:
            current_screen = AddTaskScreen(menu_win, list, db);
            break;
        }
    }

    LOG_INFO("Exiting...");
    clrtoeol();
    refresh();
    getch();
    endwin();
    return 0;
}
