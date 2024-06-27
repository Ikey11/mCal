#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Logger.h"
#include "EntryMan.h"
#include "SQL.h"


/// @brief Adds a task to the program
/// @param list List of tasks to append to
/// @param date Time task is due
/// @param priority The priority of the task (0-9)
/// @param status Whether the task is completed (0-1)
/// @param name Name of task
/// @return Added task or NULL on memory allocation failure
Task *AddTask(DoublyLinkedList *list, sqlite3_int64 id, const char *name, time_t date, uint8_t priority, uint8_t status, const char *description)
{
    // Allocate memory for a new Task
    Task *task = (Task *)malloc(sizeof(Task));
    if (task == NULL)
    {
        return NULL;
    }

    task->id = id;
    task->date = date;
    task->priority = priority;
    task->status = status;
    strncpy(task->name, name, 99);
    task->name[sizeof(task->name) - 1] = '\0'; // Ensure null termination
    if (description)
    {
        strncpy(task->desc, description, 255);
        task->desc[sizeof(task->desc) - 1] = '\0'; // Ensure null termination
    }

    // Insert the new Task into the list
    insert_front(list, task);
    LOG_INFO("EntryMan::AddTask: Added %s (%lld)", name, id);

    return task;
}

void EatSQL(DoublyLinkedList *list, sqlite3 *db)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM tasks WHERE completed = 0;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        LOG_ERROR("EntryMan::EatSQL: Failed to fetch data: %s", sqlite3_errmsg(db));
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        sqlite3_int64 id = sqlite3_column_int64(stmt, 0);
        const char *name = sqlite3_column_text(stmt, 1);
        const char *date_str = sqlite3_column_text(stmt, 2);
        const char *time_str = sqlite3_column_text(stmt, 3);
        int priority = sqlite3_column_int(stmt, 4);
        // int completed = sqlite3_column_int(stmt, 5);
        uint8_t completed = sqlite3_column_int(stmt, 5);
        const unsigned char *description = sqlite3_column_text(stmt, 6);

        char datetime_str[128];
        snprintf(datetime_str, sizeof(datetime_str), "%s %s", date_str, time_str);

        struct tm tm;
        memset(&tm, 0, sizeof(struct tm));
        strptime(datetime_str, "%Y-%m-%d %H:%M", &tm);
        time_t date = mktime(&tm);

        AddTask(list, id, name, date, priority, completed, description);
    }

    sqlite3_finalize(stmt);
}

Task *CompleteTask(DoublyLinkedList *tasks, Task *task)
{
    return task;
}

/// @brief Sorts list from earliest to latest data via Bubble sort
/// @param list The list to sort
void SortList(DoublyLinkedList *list)
{
    Node *entry = list->head;

    // Iterate to n-1
    while (entry->next != NULL)
    {
        Task *data = entry->data;

        entry = entry->next;
    }
}

/// @brief Test function
/// @param list List to read
void read_list(DoublyLinkedList *list)
{
    if (!list)
    {
        LOG_ERROR("EntryMan::read_list: Invalid DoublyLinkedList!");
        return;
    }
    Node *current = list->head;
    LOG_INFO("EntryMan: Printing list:");
    while (current)
    {
        Task *data = current->data;

        LOG_INFO("  %s", data->name);
        current = current->next;
    }
    return;
}