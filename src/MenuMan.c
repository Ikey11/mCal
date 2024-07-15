#include "MenuMan.h"
#include <stdlib.h>

// Free node data
void free_data(void *data)
{
    free(data);
}

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
    sqlite3_int64 id;                                                   // ID of new entry
    AddEntry(db, &id, name, date_str, time_str, priority, false, NULL); // Add task to database
    LOG_INFO("Adding entry %ld to memory...", id);
    AddTask(list, id, name, date, priority, false, NULL); // Add task to memory

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

void FocusMenu(WINDOW *focus_win, Node *entry)
{
    Task *data = entry->data;

    wattron(focus_win, COLOR_PAIR(5));
    mvwprintw(focus_win, 0, 1, "%s", data->name);
    wattroff(focus_win, COLOR_PAIR(5));

    // Convert timestamp to human-readable date
    char date_str[30]; // Buffer for the date string
    struct tm *tm_info;
    tm_info = localtime(&data->date);
    strftime(date_str, 30, "%Y-%m-%d %H:%M", tm_info); // Format as YYYY-MM-DD HH:MM

    mvwprintw(focus_win, 1, 4, "Due Date: %s", date_str);

    int color = PriorityColor(data->priority);
    mvwprintw(focus_win, 2, 4, "Priority: ");
    wattron(focus_win, COLOR_PAIR(color));
    mvwprintw(focus_win, 2, 14, "%u", data->priority);
    wattroff(focus_win, COLOR_PAIR(color));

    // Parse description (+ word wrap)
    if (data->desc == NULL)
        return;
    
    wmove(focus_win, 4, 1);
    for (size_t i = 0; i < DESC_SIZE; i++)
    {
        char c = data->desc[i];
        if (c == '\0')
            break;

        waddch(focus_win, c);
    }
}

void PrintMenu(WINDOW *menu_win, DoublyLinkedList *list, Node *highlight)
{
    if (!highlight)
    {
        LOG_INFO("Menu is empty");
        return;
    }

    int y, i;
    y = 1; // Start y position

    // Retrieve data
    Node *entry = list->head;
    while (entry)
    {
        Task *data = entry->data;
        Task *selectedData = highlight->data;

        if (selectedData->id == data->id)
        {
            wattron(menu_win, A_REVERSE);
        }

        mvwprintw(menu_win, y, 2, "[");
        mvwprintw(menu_win, y, 3, "%s", data->status ? FILLED_BOX : UNFILLED_BOX);
        mvwprintw(menu_win, y, 4, "]");

        if (selectedData->id == data->id)
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

ScreenState TaskScreen(WINDOW *menu_win, sqlite3 *db, DoublyLinkedList *list, Node **highlight, size_t *n_tasks)
{
    int c;
    PrintMenu(menu_win, list, *highlight);
    c = wgetch(menu_win);

    switch (c)
    {
    case KEY_UP:
        if ((*highlight)->prev == NULL)
            *highlight = list->tail;
        else
            *highlight = (*highlight)->prev;
        break;
    case KEY_DOWN:
        if ((*highlight)->next == NULL)
            *highlight = list->head;
        else
            *highlight = (*highlight)->next;
        break;
    case 'a': // Add tasks
        return ADD_TASK_SCREEN;
        break;
    case 27: // Exit (Esc key)
        return EXIT_APP;
        break;
    case 10: // Enter key
        // Toggle the status of the highlighted choice
        if (*highlight)
        {
            Task *selected_data = (*highlight)->data;
            selected_data->status = !selected_data->status;
            CompleteEntry(db, selected_data->id);
        }
        break;
    case 'd': // Delete
        if (*highlight)
        {
            Task *selected_data = (*highlight)->data;
            LOG_INFO("Deleting %s...", selected_data->name);
            RemoveEntry(db, selected_data->id);

            Node *rm = *highlight;
            if ((*highlight)->prev == NULL)
                *highlight = list->tail;
            else
                *highlight = (*highlight)->prev;
            delete_node(list, rm, free_data);
        }
        else
        {
            LOG_WARNING("Cannot delete entry: No object selected");
        }
        break;
    }
    PrintMenu(menu_win, list, *highlight);
    return TASK_SCREEN;
}
