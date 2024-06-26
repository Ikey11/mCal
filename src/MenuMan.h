#ifndef MENUMAN_H
#define MENUMAN_H

#include "main.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]);

#define UNFILLED_BOX " "
#define FILLED_BOX "x"

typedef enum
{
    TASK_SCREEN,
    ADD_TASK_SCREEN,
    EXIT_APP
} ScreenState;

ScreenState AddTaskScreen(WINDOW *menu_win, DoublyLinkedList *list, sqlite3 *db);

int PriorityColor(int priority);
void PrintMenu(WINDOW *menu_win, DoublyLinkedList *list, int highlight);
ScreenState TaskScreen(WINDOW *menu_win, DoublyLinkedList *list, int *highlight, size_t *n_tasks);

#endif