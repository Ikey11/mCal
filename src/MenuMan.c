#include "MenuMan.h"

// ----- Add Task -----

// Add new task
ScreenState AddTaskScreen(WINDOW *menu_win, DoublyLinkedList *list, sqlite3 *db)
{
    echo();
    curs_set(1); // Show cursor

    char name[50];
    char date_str[11], time_str[6];
    uint8_t priority;
    time_t date;
    struct tm tm_info;

    // Get task data from user
    mvwprintw(menu_win, 2, 2, "Enter task name: ");
    wrefresh(menu_win);
    wgetnstr(menu_win, name, 100);

    mvwprintw(menu_win, 3, 2, "Enter task date (YYYY-MM-DD): ");
    wrefresh(menu_win);
    wgetnstr(menu_win, date_str, 11);

    mvwprintw(menu_win, 4, 2, "Enter task time (HH:MM): ");
    wrefresh(menu_win);
    wgetnstr(menu_win, time_str, 6);

    mvwprintw(menu_win, 5, 2, "Enter task priority (0-9): ");
    wrefresh(menu_win);
    wscanw(menu_win, "%hhu", &priority);

    char datetime_str[128];
    snprintf(datetime_str, sizeof(datetime_str), "%s %s", date_str, time_str);
    strptime(datetime_str, "%Y-%m-%d %H:%M", &tm_info);
    date = mktime(&tm_info);

    // Add task
    AddTask(list, date, priority, false, name);                    // Add task to memory
    AddEntry(db, name, date_str, time_str, priority, false, NULL); // Add task to database

    noecho();
    curs_set(0); // Hide cursor

    return TASK_SCREEN;
}

// ----- Task List -----

int PriorityColor(int priority)
{
    // Set color based on priority
    int color;
    switch (priority)
    {
    case 0:
    case 1:
        color = 2;
        break;
    case 2:
    case 3:
        color = 4;
        break;
    case 4:
    case 5:
        color = 5;
        break;
    case 6:
    case 7:
        color = 3;
        break;
    case 8:
    case 9:
        color = 1;
        break;
    default:
        color = 7;
        break;
    }

    return color;
}

void PrintMenu(WINDOW *menu_win, DoublyLinkedList *list, int highlight)
{
    int y, i;
    y = 2; // Start y position

    // Retrieve data
    Node *entry = list->head;
    for (i = 0; i < list->size; ++i)
    {
        Task *data = entry->data;

        if (highlight == i + 1)
        {
            wattron(menu_win, A_REVERSE);
        }

        mvwprintw(menu_win, y, 2, "[");
        mvwprintw(menu_win, y, 3, "%s", data->status ? FILLED_BOX : UNFILLED_BOX);
        mvwprintw(menu_win, y, 4, "]");

        if (highlight == i + 1)
        {
            wattroff(menu_win, A_REVERSE);
        }

        mvwprintw(menu_win, y, 6, "%s", data->name);

        // Convert timestamp to human-readable date
        char date_str[30]; // Buffer for the date string
        struct tm *tm_info;
        tm_info = localtime(&data->date);
        strftime(date_str, 30, "%Y-%m-%d %H:%M", tm_info); // Format as YYYY-MM-DD HH:MM

        mvwprintw(menu_win, ++y, 8, "Date: %s", date_str);

        int color = PriorityColor(data->priority);
        mvwprintw(menu_win, ++y, 8, "Priority: ");
        wattron(menu_win, COLOR_PAIR(color));
        mvwprintw(menu_win, y, 18, "%u", data->priority);
        wattroff(menu_win, COLOR_PAIR(color));

        y += 2;

        // Move to next entry
        entry = entry->next;
    }
    wrefresh(menu_win);
}

ScreenState TaskScreen(WINDOW *menu_win, DoublyLinkedList *list, int *highlight, size_t *n_tasks)
{
    int c;
    PrintMenu(menu_win, list, *highlight);
    while (1)
    {
        c = wgetch(menu_win);
        switch (c)
        {
        case KEY_UP:
            if (*highlight == 1)
                *highlight = *n_tasks;
            else
                --(*highlight);
            break;
        case KEY_DOWN:
            if (*highlight == *n_tasks)
                *highlight = 1;
            else
                ++(*highlight);
            break;
        case 'a': // Add tasks
            return ADD_TASK_SCREEN;
            break;
        case 10: // Enter key
            // Toggle the status of the highlighted choice
            // tasks[highlight - 1].status = !tasks[highlight - 1].status;
            break;
        }
        PrintMenu(menu_win, list, *highlight);
    }
    return TASK_SCREEN;
}