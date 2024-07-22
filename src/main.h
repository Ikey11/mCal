// Essential tools for console output
#ifndef MAIN_H
#define MAIN_H

#define _XOPEN_SOURCE
#include <ncurses.h>
#include <time.h>
#include <string.h>

#include "Logger.h"

// Data management libs
#include "EntryMan.h"
#include "SQL.h"

typedef enum
{
    TASK_SCREEN,
    ADD_TASK_SCREEN,
    EXIT_APP
} ScreenState;

// Color table
int PriorityColor(int priority);

// Main function to handle smart word wrapping and printing
void WordWrap(WINDOW *focus_win, const char *buffer, size_t buffer_size, int y, int x, int width);

#endif