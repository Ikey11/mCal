#include "GUITask.h"
#include <stdlib.h>

// Free node data
void free_data(void *data)
{
    free(data);
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
    if (!entry)
        return;

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
        //LOG_INFO("Menu is empty");
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
        if (!(*highlight))
            break;
        if ((*highlight)->prev == NULL)
            *highlight = list->tail;
        else
            *highlight = (*highlight)->prev;
        break;
    case KEY_DOWN:
        if (!(*highlight))
            break;
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
            // Open data to identify data to remove
            Task *selected_data = (*highlight)->data;
            LOG_INFO("Deleting %s...", selected_data->name);
            RemoveEntry(db, selected_data->id);

            Node *rm = *highlight;
            if (list->size == 1)
                *highlight = NULL;
            else if ((*highlight)->prev == NULL)
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
