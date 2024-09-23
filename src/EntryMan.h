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
    time_t date, sdate;
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

extern SortParam sort_param;

Task *AddTask(DoublyLinkedList *list, Task taskValues);

void EatSQL(DoublyLinkedList *list, sqlite3 *db);

void RemoveTask(DoublyLinkedList *list, const char *name);

void SortList(DoublyLinkedList **list);

#endif