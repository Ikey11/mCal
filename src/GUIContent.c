#include "GUIContent.h"

// Define an enumerator for the input states
typedef enum
{
    INPUT_NAME,
    INPUT_DATE,
    INPUT_TIME,
    INPUT_PRIORITY,
    INPUT_DESCRIPTION,
    INPUT_DONE
} InputState;

// ----- Utility Functions -----

time_t GetTime(int year, int mon, int day, int hour, int min)
{
    struct tm loctime = {0};
    loctime.tm_year = year - 1900;
    loctime.tm_mon = mon;
    loctime.tm_mday = day;
    loctime.tm_hour = hour;
    loctime.tm_min = min;
    loctime.tm_sec = 0;
    loctime.tm_isdst = -1;

    return mktime(&loctime);
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

    int color = PriorityColor(priority);
    wattron(menu_win, COLOR_PAIR(color));
    mvwprintw(menu_win, 0, 1, "%s", name);
    mvwprintw(menu_win, 1, 1, "%s", time_preview);
    wattroff(menu_win, COLOR_PAIR(color));
    wrefresh(menu_win);
}

void PrintDate(WINDOW *menu_win, int year, int mon, int day, int sel)
{
    sel == 0 ? wattron(menu_win, A_REVERSE) : 0;
    mvwprintw(menu_win, 4, 12, "%04d", year);
    sel == 0 ? wattroff(menu_win, A_REVERSE) : 0;

    sel == 1 ? wattron(menu_win, A_REVERSE) : 0;
    mvwprintw(menu_win, 4, 17, "%02d", mon + 1);
    sel == 1 ? wattroff(menu_win, A_REVERSE) : 0;

    sel == 2 ? wattron(menu_win, A_REVERSE) : 0;
    mvwprintw(menu_win, 4, 20, "%02d", day);
    sel == 2 ? wattroff(menu_win, A_REVERSE) : 0;

    wrefresh(menu_win);
}

void PrintTime(WINDOW *menu_win, int hour, int min, int sel)
{
    sel == 0 ? wattron(menu_win, A_REVERSE) : 0;
    mvwprintw(menu_win, 5, 12, "%02d", hour);
    sel == 0 ? wattroff(menu_win, A_REVERSE) : 0;

    sel == 1 ? wattron(menu_win, A_REVERSE) : 0;
    mvwprintw(menu_win, 5, 15, "%02d", min);
    sel == 1 ? wattroff(menu_win, A_REVERSE) : 0;

    wrefresh(menu_win);
}

// ----- Input Functions -----

bool InputName(WINDOW *menu_win, int c, uint16_t *index, char *name)
{
    // if (c == 27)  // Escape
    //     return; // Early exit
    if (c == KEY_BACKSPACE)
    {
        if ((*index) > 0)
        {
            (*index)--;
            mvwdelch(menu_win, 3, (*index) + 12);
        }
    }
    else if ((*index) < NAME_SIZE - 1)
    {
        name[(*index)++] = c;
        mvwaddch(menu_win, 3, (*index) + 11, c);
    }
    name[(*index) + 1] = '\0';

    if (c == '\n')
    {
        return TRUE; // Exit state
    }

    wrefresh(menu_win);
    return FALSE;
}

bool InputDate(WINDOW *menu_win, int c, uint16_t *selection, int *year, int *mon, int *day)
{
    int sel = *selection;

    if (c == '\n')
    {
        return TRUE;
    }
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
            (*year)++;
        else if (sel == 1)
            *mon = (*mon + 1) % 12;
        else if (sel == 2)
            *day = (*day % 31) + 1;
        break;
    case KEY_DOWN:
        if (sel == 0)
            (*year)--;
        else if (sel == 1)
            *mon = (*mon == 0) ? 11 : *mon - 1;
        else if (sel == 2)
            *day = (*day == 1) ? 31 : *day - 1;
        break;
    default:
        break;
    }
    PrintDate(menu_win, *year, *mon, *day, sel);

    *selection = sel;
    return FALSE;
}

void InputTime(WINDOW *menu_win, int *hour, int *min)
{
    int c, sel = 0;

    mvwprintw(menu_win, 5, 1, "Task time: ");
    PrintTime(menu_win, *hour, *min, sel);

    while ((c = wgetch(menu_win)) != '\n')
    {
        switch (c)
        {
        case KEY_LEFT:
            sel = (sel == 0) ? 1 : sel - 1;
            break;
        case KEY_RIGHT:
            sel = (sel == 1) ? 0 : sel + 1;
            break;
        case KEY_UP:
            if (sel == 0)
                *hour = (*hour + 1) % 24;
            else if (sel == 1)
                *min = (*min + 1) % 60;
            break;
        case KEY_DOWN:
            if (sel == 0)
                *hour = (*hour == 0) ? 23 : *hour - 1;
            else if (sel == 1)
                *min = (*min == 0) ? 59 : *min - 1;
            break;
        default:
            break;
        }
        PrintTime(menu_win, *hour, *min, sel);
    }
}

void InputPriority(WINDOW *menu_win, uint8_t *priority)
{
    int c;
    mvwprintw(menu_win, 6, 1, "Priority: ");
    int color = PriorityColor(*priority);
    mvwprintw(menu_win, 6, 11, "< >");
    wattron(menu_win, COLOR_PAIR(color));
    mvwprintw(menu_win, 6, 12, "%d", *priority);
    wattroff(menu_win, COLOR_PAIR(color));
    wrefresh(menu_win);

    while ((c = wgetch(menu_win)) != '\n')
    {
        switch (c)
        {
        case KEY_LEFT:
            *priority = (*priority == 0) ? 9 : *priority - 1;
            break;
        case KEY_RIGHT:
            *priority = (*priority == 9) ? 0 : *priority + 1;
            break;
        default:
            break;
        }
        color = PriorityColor(*priority);
        wattron(menu_win, COLOR_PAIR(color));
        mvwprintw(menu_win, 6, 12, "%d", *priority);
        wattroff(menu_win, COLOR_PAIR(color));
        wrefresh(menu_win);
    }
}

void InputDescription(WINDOW *menu_win, char *desc)
{
    int c, index = 0;
    int cur_y = 7, cur_x = 15; // Initial cursor position after "Description: "
    mvwprintw(menu_win, 7, 1, "Description: ");
    wrefresh(menu_win);

    while ((c = wgetch(menu_win)) != '\n')
    {
        if (c == KEY_BACKSPACE || c == 127 || c == 8)
        {
            if (index > 0)
            {
                index--;
                getyx(menu_win, cur_y, cur_x);
                mvwdelch(menu_win, cur_y, cur_x - 1);
            }
        }
        else if (index < DESC_SIZE - 1)
        {
            desc[index++] = c;
            waddch(menu_win, c);
        }
        wrefresh(menu_win);
    }

    desc[index] = '\0';
}

// ----- Main Task Addition Function -----

void AddTaskScreen(WINDOW *menu_win, Task *task)
{
    // Init parameters
    char name[NAME_SIZE] = {0};
    uint8_t priority = 0;
    char description[DESC_SIZE] = {0};

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

    /** TODO:
     *  PrintPreview is not actively updating on input, solve this by removing nested while loop in each user input function
     * and provide them with a boolean output as an exit flag. This block will deal with state transitions and the look of
     * the print preview, while the functions are strictly for user input logic.
     *  Note that the states will only tell the state machine that it is done, not its transistion, this allows the functions
     * to be reusable in the menu type environment planned for the edit task function.
     */

    // --- State Machine ---
    // Enter with name input
    int c;
    uint16_t index = 0, sel = 0;
    mvwprintw(menu_win, 3, 1, "Task name: ");
    wrefresh(menu_win);
    curs_set(1);
    InputState state = INPUT_NAME;

    while (state != INPUT_DONE)
    {
        c = wgetch(menu_win);

        switch (state)
        {
        case INPUT_NAME:
            if (InputName(menu_win, c, &index, name))
            {
                curs_set(0);
                PrintDate(menu_win, year, mon, day, sel);
                mvwprintw(menu_win, 4, 1, "Task date: ");
                state = INPUT_DATE;
            }
            break;
        case INPUT_DATE:
            if (InputDate(menu_win, c, &sel, &year, &mon, &day))
            {
                sel = 0;
                state = INPUT_TIME;
            }
            break;
        case INPUT_TIME:
            InputTime(menu_win, &hour, &min);
            state = INPUT_PRIORITY;
            break;
        case INPUT_PRIORITY:
            InputPriority(menu_win, &priority);
            state = INPUT_DESCRIPTION;
            break;
        case INPUT_DESCRIPTION:
            InputDescription(menu_win, description);
            state = INPUT_DONE;
            break;
        default:
            break;
        }

        // Print a preview of the task as you go along
        PrintPreview(menu_win, name, year, mon, day, hour, min, priority);
    }

    // Set the task fields after all input is done
    LOG_INFO("Creating task...");
    strncpy(task->name, name, NAME_SIZE);
    task->date = GetTime(year, mon, day, hour, min);
    task->priority = priority;
    strncpy(task->desc, description, DESC_SIZE);
}

// Function to convert time_t to SQL datetime string
void FormatTimeToSQL(time_t time, char *buffer, size_t bufferSize)
{
    struct tm *timeinfo = localtime(&time);
    strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:00.000", timeinfo);
}

// Function to add a task to the database
int AddTaskToDatabase(DoublyLinkedList *list, sqlite3 *db, Task task)
{
    char datetime_str[24];
    char sdatetime_str[24];

    // Format date and sdate to SQL datetime format
    FormatTimeToSQL(task.date, datetime_str, sizeof(datetime_str));
    if (task.sdate != (time_t)-1)
    {
        FormatTimeToSQL(task.sdate, sdatetime_str, sizeof(sdatetime_str));
    }

    // Call AddEntry
    int result;
    if (task.sdate != (time_t)-1)
    {
        result = AddEntry(db, &task.id, task.name, datetime_str, sdatetime_str,
                          task.priority, task.status, task.desc);
    }
    else
    {
        result = AddEntry(db, &task.id, task.name, datetime_str, NULL,
                          task.priority, task.status, task.desc);
    }
    if (result != SQLITE_OK)
    {
        LOG_ERROR("GUIContent::AddTaskToDataBase: Aborting transfer!");
        return 1;
    }

    // Add to memory
    if (AddTask(list, task) == NULL)
    {
        LOG_ERROR("GUIContent::AddTaskToDatabase: Critial memory failure! Aborting transfer!");
        LOG_WARNING("GUIContent::AddTaskToDatabase: Memory is not in sync with database, attempt to reboot the program");
        return 1;
    }

    return result;
}