#include "MenuMan.h"
#include <stdlib.h>

// Free node data
void free_data(void *data)
{
    free(data);
}

// Color table
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

// ----- Add Task -----

time_t GetTime(int year, int mon, int day, int hour, int min)
{
    struct tm loctime = {0};
    loctime.tm_year = year - 1900;
    loctime.tm_mon = mon;
    loctime.tm_mday = day;
    loctime.tm_hour = hour;
    loctime.tm_min = min;
    loctime.tm_sec = 0;

    time_t temp_time = mktime(&loctime);
}

void PrintPreview(WINDOW *menu_win, const char *name, int year, int mon, int day, int hour, int min, int priority)
{
    char time_preview[26];

    time_t temp_time = GetTime(year, mon, day, hour, min);

    if (temp_time == (time_t)-1)
    {
        strcpy(time_preview, "Invalid date");
    }
    else
    {
        char *temp_str = ctime(&temp_time);
        strncpy(time_preview, temp_str, strlen(temp_str) - 1); // Remove newline
        time_preview[strlen(temp_str) - 1] = '\0';
    }

    // Print human readable
    int color = PriorityColor(priority);
    wattron(menu_win, COLOR_PAIR(color));
    mvwprintw(menu_win, 0, 1, "%s", name);
    mvwprintw(menu_win, 1, 1, "%s", time_preview);
    wattroff(menu_win, COLOR_PAIR(color));
}

void PrintDate(WINDOW *menu_win, int year, int mon, int day, int sel)
{
    // Print info
    sel == 0 ? wattron(menu_win, A_REVERSE) : 0;
    mvwprintw(menu_win, 4, 12, "%04d", year);
    sel == 0 ? wattroff(menu_win, A_REVERSE) : 0;

    sel == 1 ? wattron(menu_win, A_REVERSE) : 0;
    mvwprintw(menu_win, 4, 17, "%02d", mon + 1); // Month is 0-based in struct tm
    sel == 1 ? wattroff(menu_win, A_REVERSE) : 0;

    sel == 2 ? wattron(menu_win, A_REVERSE) : 0;
    mvwprintw(menu_win, 4, 20, "%02d", day);
    sel == 2 ? wattroff(menu_win, A_REVERSE) : 0;

    wrefresh(menu_win);
}

void PrintTime(WINDOW *menu_win, int hour, int min, int sel)
{
    // Print info
    sel == 0 ? wattron(menu_win, A_REVERSE) : 0;
    mvwprintw(menu_win, 5, 12, "%02d", hour);
    sel == 0 ? wattroff(menu_win, A_REVERSE) : 0;

    sel == 1 ? wattron(menu_win, A_REVERSE) : 0;
    mvwprintw(menu_win, 5, 15, "%02d", min);
    sel == 1 ? wattroff(menu_win, A_REVERSE) : 0;

    wrefresh(menu_win);
}

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
    mvwprintw(menu_win, 3, 1, "Task name: ");
    wrefresh(menu_win);
    wgetnstr(menu_win, name, NAME_SIZE);

    // Get current time
    time_t currtime = time(NULL);
    if (currtime == ((time_t)-1))
    {
        LOG_ERROR("Failed to obtain the current time.");
        currtime = 1031718000; // Fallback time
    }

    struct tm *loctime = localtime(&currtime);
    int year = loctime->tm_year + 1900;
    int mon = loctime->tm_mon;
    int day = loctime->tm_mday;
    int hour = loctime->tm_hour;
    int min = loctime->tm_min;
    int sel = 0; // Current time interval selected

    // Print initial date
    mvwprintw(menu_win, 4, 1, "Task date: ");
    PrintPreview(menu_win, name, year, mon, day, 0, 0, 0);
    PrintDate(menu_win, year, mon, day, sel);

    curs_set(0); // Hide cursor
    int c;       // Latest character input
    while (c != 10)
    {
        c = wgetch(menu_win);

        switch (c)
        {
        case KEY_LEFT:
            sel = (sel == 0) ? 2 : sel - 1;
            break;

        case KEY_RIGHT:
            sel = (sel == 2) ? 0 : sel + 1;
            break;

        case KEY_UP:
            if (sel == 0)
                year++;
            else if (sel == 1)
                mon = (mon + 1) % 12;
            else if (sel == 2)
                day = (day % 31) + 1;
            break;

        case KEY_DOWN:
            if (sel == 0)
                year--;
            else if (sel == 1)
                mon = (mon == 0) ? 11 : mon - 1;
            else if (sel == 2)
                day = (day == 1) ? 31 : day - 1;
            break;

        default:
            break;
        }

        PrintPreview(menu_win, name, year, mon, day, 0, 0, 0);
        PrintDate(menu_win, year, mon, day, sel);
    }

    c = 0;
    sel = 0;

    // Print working date
    mvwprintw(menu_win, 5, 1, "Task time: ");
    PrintPreview(menu_win, name, year, mon, day, hour, min, 0);
    PrintDate(menu_win, year, mon, day, -1);
    PrintTime(menu_win, hour, min, sel);

    curs_set(0); // Hide cursor
    while (c != 10)
    {
        c = wgetch(menu_win);

        switch (c)
        {
            // TODO: Allow for selection of AM/PM
        case KEY_LEFT:
            sel = (sel == 0) ? 1 : sel - 1;
            break;

        case KEY_RIGHT:
            sel = (sel == 1) ? 0 : sel + 1;
            break;

        case KEY_UP:
            if (sel == 0)
                hour = (hour + 1) % 24;
            else if (sel == 1)
                min = (min + 1) % 60;
            break;

        case KEY_DOWN:
            if (sel == 0)
                hour = (hour == 0) ? 23 : hour - 1;
            else if (sel == 1)
                min = (min == 0) ? 59 : min - 1;
            break;

        default:
            break;
        }

        PrintPreview(menu_win, name, year, mon, day, hour, min, 0);
        PrintTime(menu_win, hour, min, sel);
    }

    c = 0;
    sel = 0;
    priority = 0;
    mvwprintw(menu_win, 6, 1, "Priority: ");
    int color = PriorityColor(priority);
    mvwprintw(menu_win, 6, 11, "< >");
    wattron(menu_win, COLOR_PAIR(color));
    mvwprintw(menu_win, 6, 12, "%d", priority);
    wattroff(menu_win, COLOR_PAIR(color));

    PrintPreview(menu_win, name, year, mon, day, hour, min, priority);
    PrintTime(menu_win, hour, min, -1);
    curs_set(0); // Hide cursor
    while (c != 10)
    {
        c = wgetch(menu_win);

        switch (c)
        {
            // TODO: Allow for selection of AM/PM
        case KEY_LEFT:
            priority = (priority == 0) ? 9 : priority - 1;
            break;

        case KEY_RIGHT:
            priority = (priority == 9) ? 0 : priority + 1;
            break;

        default:
            break;
        }

        color = PriorityColor(priority);
        wattron(menu_win, COLOR_PAIR(color));
        mvwprintw(menu_win, 6, 12, "%d", priority);
        wattroff(menu_win, COLOR_PAIR(color));
        PrintPreview(menu_win, name, year, mon, day, hour, min, priority);
        wrefresh(menu_win);
    }

    char datetime_str[128];
    snprintf(datetime_str, sizeof(datetime_str), "%s %s", date_str, time_str);
    strptime(datetime_str, "%Y-%m-%d %H:%M", &tm_info);
    date = mktime(&tm_info);

    // Add task
    sqlite3_int64 id;                                                   // ID of new entry
    AddEntry(db, &id, name, date_str, time_str, priority, false, NULL); // Add task to database
    LOG_INFO("Adding entry %ld to memory...", id);
    AddTask(list, id, name, date, priority, false, NULL); // Add task to memory
    SortList(&list, DATE);

    noecho();
    curs_set(0); // Hide cursor

    return TASK_SCREEN;
}

// ----- Task List -----

// Main function to handle smart word wrapping and printing
void PrintDescription(WINDOW *focus_win, const char *desc)
{
    if (desc == NULL)
        return;

    size_t w = 0;                  // Word iterator
    char smartdesc[DESC_SIZE * 2]; // Buffer to hold formatted description
    size_t smartdesc_pos = 0;      // Current position in smartdesc
    smartdesc[0] = '\0';           // Initialize smartdesc

    char descword[DESC_SIZE]; // Buffer to hold current word
    size_t line_length = 0;   // Current length of the line

    for (size_t i = 0; i < DESC_SIZE && desc[i] != '\0'; i++)
    {
        char c = desc[i]; // Get char
        if (c == ' ' || c == '\n')
        {
            descword[w] = '\0'; // Terminate current word

            // Check if the word fits in the current line
            if (line_length + w + 1 > FOCUS_WIDTH)
            {
                // If not, insert a newline
                smartdesc[smartdesc_pos++] = '\n';
                line_length = 0;
            }

            // Add the word to smartdesc
            if (line_length > 0)
            {
                smartdesc[smartdesc_pos++] = ' '; // Add a space before the word if it's not the first word in the line
                line_length++;
            }

            strncpy(smartdesc + smartdesc_pos, descword, w);
            smartdesc_pos += w;
            line_length += w;

            // Reset word buffer
            w = 0;

            if (c == '\n')
            {
                smartdesc[smartdesc_pos++] = '\n';
                line_length = 0;
            }
        }
        else
        {
            descword[w++] = c; // Add char to current word
        }
    }

    // Ensure the last word is added
    if (w > 0)
    {
        descword[w] = '\0'; // Terminate the last word

        if (line_length + w + 1 > FOCUS_WIDTH)
        {
            smartdesc[smartdesc_pos++] = '\n';
        }
        else if (line_length > 0)
        {
            smartdesc[smartdesc_pos++] = ' ';
        }

        strncpy(smartdesc + smartdesc_pos, descword, w);
        smartdesc_pos += w;
    }

    smartdesc[smartdesc_pos] = '\0'; // Terminate the smartdesc buffer

    // Print formatted text (if possible)
    if (strlen(smartdesc) < DESC_SIZE)
    {
        // Print formatted text without ncurses wrapping
        int row = 4, col = 0;
        for (size_t i = 0; i < smartdesc_pos; i++)
        {
            if (smartdesc[i] == '\n')
            {
                row++;
                col = 0;
            }
            else
            {
                mvwaddch(focus_win, row, col++, smartdesc[i]);
            }
        }
    }
    else
        mvwprintw(focus_win, 4, 0, "%s", smartdesc);
}

void FocusMenu(WINDOW *focus_win, Node *entry)
{
    Task *data = entry->data;

    int color = PriorityColor(data->priority);
    wattron(focus_win, COLOR_PAIR(color));
    mvwprintw(focus_win, 0, 0, "%s", data->name);
    wattroff(focus_win, COLOR_PAIR(color));

    // Convert timestamp to human-readable date
    char date_str[30]; // Buffer for the date string
    struct tm *tm_info;
    tm_info = localtime(&data->date);
    strftime(date_str, 30, "%Y-%m-%d %H:%M", tm_info); // Format as YYYY-MM-DD HH:MM

    mvwprintw(focus_win, 1, 0, "Due: %s", date_str);

    mvwprintw(focus_win, 1, 24, "Prio:");
    wattron(focus_win, COLOR_PAIR(color));
    mvwprintw(focus_win, 1, 29, "%u", data->priority);
    wattroff(focus_win, COLOR_PAIR(color));

    PrintDescription(focus_win, data->desc);
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

        mvwprintw(menu_win, y, 1, "[");
        mvwprintw(menu_win, y, 2, "%s", data->status ? FILLED_BOX : UNFILLED_BOX);
        mvwprintw(menu_win, y, 3, "]");

        if (selectedData->id == data->id)
        {
            wattroff(menu_win, A_REVERSE);
        }

        // Print name
        int color = PriorityColor(data->priority);
        wattron(menu_win, COLOR_PAIR(color));
        mvwprintw(menu_win, y, 5, "%s", data->name);
        wattroff(menu_win, COLOR_PAIR(color));

        // Convert timestamp to human-readable date
        char date_str[30]; // Buffer for the date string
        struct tm *tm_info;
        tm_info = localtime(&data->date);
        strftime(date_str, 30, "%Y-%m-%d %H:%M", tm_info); // Format as YYYY-MM-DD HH:MM

        mvwprintw(menu_win, y, 38, " Due: %s", date_str);

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
