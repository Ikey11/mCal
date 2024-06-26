#ifndef ENTRYMAN_H
#define ENTRYMAN_H

#include <stdint.h>
#include <time.h>
#include "DLL.h"
#include "SQL.h"

typedef struct
{
    size_t id;
    time_t date;
    uint8_t priority; // Scale of 0-9
    uint8_t status;   // boolean
    char name[100];
} Task;

Task *AddTask(DoublyLinkedList *list, time_t date, uint8_t priority, uint8_t status, const char *name);

void EatSQL(DoublyLinkedList *list, sqlite3 *db);

void RemoveTask(DoublyLinkedList *list, const char *name);

Task *CompleteTask(DoublyLinkedList *list, Task *task);

void SortList(DoublyLinkedList *list);

#endif