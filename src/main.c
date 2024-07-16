#include "main.h"
#include "MenuMan.h"

int main()
{
    WINDOW *menu_win, *focus_win,
        *menu_box, *focus_box, *console_box;

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

    focus_win = newwin(FOCUS_LINES, FOCUS_WIDTH, 1, 1);
    focus_box = newwin(FOCUS_LINES + 2, FOCUS_WIDTH + 2, 0, 0);
    menu_win = newwin(MENU_LINES, MENU_WIDTH, 1, FOCUS_WIDTH + 4);
    menu_box = newwin(MENU_LINES + 2, MENU_WIDTH + 2, 0, FOCUS_WIDTH + 3);

    console_win = newwin(CONSOLE_LINES, CONSOLE_WIDTH, FOCUS_LINES + 3, 1);
    console_box = newwin(CONSOLE_LINES + 2, CONSOLE_WIDTH + 2, FOCUS_LINES + 2, 0);
    console_enabled = true; // Enables console logging

    LOG_WARNING("This is a warning!");
    LOG_ERROR("This is an error!");

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

    Node *highlight = list->head; // Currently selected node

    // ----- Output to terminal -----
    bool running = true;

    // Draw boxes
    wattron(focus_box, COLOR_PAIR(6));
    box(focus_box, 0, 0);
    wattroff(focus_box, COLOR_PAIR(6));
    wrefresh(focus_box);

    box(menu_box, 0, 0);
    wrefresh(menu_box);

    if (console_enabled)
    {
        wattron(console_box, COLOR_PAIR(1));
        box(console_box, 0, 0);
        wattroff(console_box, COLOR_PAIR(1));
        wrefresh(console_box);
        LOG_INFO("Boxes drawn");    // Important to use or console will not display immediantly on boot
    }

    while (running)
    {
        // Clear the windows
        wclear(focus_win);
        wclear(menu_win);

        FocusMenu(focus_win, highlight);

        // Refresh windows
        wrefresh(focus_win);
        wrefresh(menu_win);

        // Menu window state machine
        switch (current_screen)
        {
        case TASK_SCREEN:
            current_screen = TaskScreen(menu_win, db, list, &highlight, n_tasks);
            break;
        case ADD_TASK_SCREEN:
            current_screen = AddTaskScreen(menu_win, list, db);
            break;
        case EXIT_APP:
            running = false;
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
