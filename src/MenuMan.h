#ifndef MENUMAN_H
#define MENUMAN_H

#include "defs.h"
#include "main.h"
#include "SQL.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]);

#define UNFILLED_BOX " "
#define FILLED_BOX "x"

typedef enum
{
    TASK_SCREEN,
    ADD_TASK_SCREEN,
    EXIT_APP
} ScreenState;

void FocusMenu(WINDOW *focus_win, Node *highlight);

ScreenState AddTaskScreen(WINDOW *menu_win, DoublyLinkedList *list, sqlite3 *db);
ScreenState TaskScreen(WINDOW *menu_win, sqlite3 *db, DoublyLinkedList *list, Node **highlight, size_t *n_tasks);

#endif