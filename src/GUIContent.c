#include "GUIContent.h"

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

void PrintTime(WINDOW *menu_win, int hour, int min, int sel, bool dst)
{
    // Print info
    sel == 0 ? wattron(menu_win, A_REVERSE) : 0;
    mvwprintw(menu_win, 5, 12, dst ? "%02d DST" : "%02d", dst ? hour + 1 : hour);
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
    uint8_t priority;
    time_t date;

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

    // Determine whether date is in DST
    int dst;
    {
        time_t time = GetTime(year, mon, day, 0, 0);
        struct tm *temptime = gmtime(&time);
        dst = temptime->tm_isdst;
        if (dst < 0)
            LOG_ERROR("Could not obtain DST info");
    }

    c = 0;
    sel = 0;

    // Print working date
    mvwprintw(menu_win, 5, 1, "Task time: ");
    PrintPreview(menu_win, name, year, mon, day, hour, min, 0);
    PrintDate(menu_win, year, mon, day, -1);
    PrintTime(menu_win, hour, min, sel, dst);

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
        PrintTime(menu_win, hour, min, sel, dst);
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
    PrintTime(menu_win, hour, min, -1, dst);
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

    // Export as plaintext
    char datetime_str[24];

    time_t datetime = GetTime(year, mon, day, hour, min);

    snprintf(datetime_str, sizeof(datetime_str), "%04d-%02d-%02d %02d:%02d:00.000",
             year, mon, day, hour, min);

    // Add task
    sqlite3_int64 id;                                                   // ID of new entry
    AddEntry(db, &id, name, datetime_str, NULL, priority, false, NULL); // Add task to database
    LOG_INFO("Adding entry %ld to memory...", id);
    AddTask(list, id, name, datetime, priority, false, NULL); // Add task to memory
    SortList(&list, DATE);

    noecho();
    curs_set(0); // Hide cursor

    return TASK_SCREEN;
}