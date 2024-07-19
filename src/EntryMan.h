#ifndef ENTRYMAN_H
#define ENTRYMAN_H

#include <stdint.h>
#include <time.h>
#include "defs.h"
#include "DLL.h"
#include "SQL.h"

typedef struct
{
    sqlite3_int64 id;
    time_t date;
    uint8_t priority; // Scale of 0-9
    uint8_t status;   // boolean
    char name[NAME_SIZE];
    char desc[DESC_SIZE]; // Description
    DoublyLinkedList subtask;
} Task;

typedef enum
{
    DATE,        // Date Soonest->Latest
    DATE_DES,    // Date Latest->Soonest
    PRIORITY,    // Priority Greatest->Least
    PRIORITY_ASC // Priority Least->Greatest
} SortParam;

Task *AddTask(DoublyLinkedList *list, sqlite3_int64 id, const char *name, time_t date, uint8_t priority, uint8_t status, const char *description);

void EatSQL(DoublyLinkedList *list, sqlite3 *db);

void RemoveTask(DoublyLinkedList *list, const char *name);

void SortList(DoublyLinkedList **list, SortParam param);

#endif