#ifndef ENTRYMAN_H
#define ENTRYMAN_H

#include <stdint.h>
#include <time.h>
#include "DLL.h"
#include "SQL.h"

typedef struct
{
    sqlite3_int64 id;
    time_t date;
    uint8_t priority; // Scale of 0-9
    uint8_t status;   // boolean
    char name[100];
    char desc[256]; // Description
    DoublyLinkedList subtask;
} Task;

Task *AddTask(DoublyLinkedList *list, sqlite3_int64 id, const char *name, time_t date, uint8_t priority, uint8_t status, const char *description);

void EatSQL(DoublyLinkedList *list, sqlite3 *db);

void RemoveTask(DoublyLinkedList *list, const char *name);

void SortList(DoublyLinkedList *list);

#endif