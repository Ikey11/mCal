#ifndef GUICONTENT_H
#define GUICONTENT_H

#include "defs.h"
#include "main.h"

int AddTaskToDatabase(DoublyLinkedList *list, sqlite3 *db, Task task);

void AddTaskScreen(WINDOW *menu_win, Task *task);

#endif