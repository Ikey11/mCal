#include <stdio.h>
#include <sqlite3.h>
#include "SQL.h"

/// @brief Initializes the database and ensures the tasks table exists
/// @param db Database to be inited
/// @return SQLITE error code
int InitSQL(sqlite3 **db)
{
    int rc = sqlite3_open("calendar.db", db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    }

    // Ensures tasks table exists
    char *sql = "CREATE TABLE IF NOT EXISTS tasks ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "name TEXT NOT NULL,"
                "date TEXT NOT NULL,"
                "time TEXT NOT NULL,"
                "priority INTEGER NOT NULL,"
                "completed INTEGER NOT NULL,"
                "description TEXT);";
    char *zErrMsg = 0;

    rc = sqlite3_exec(*db, sql, 0, 0, &zErrMsg); // Run SQL command

    // Error checking
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return rc;
    }

    return SQLITE_OK;
}

/// @brief Adds an entry to the tasks table
/// @param db Database
/// @param date
/// @param time
/// @param priority The importance of the task (0-9)
/// @param completed    Whether the task is complete
/// @param description (optional) additional information about the task
/// @return
int AddEntry(sqlite3 *db, const char *name, const char *date, const char *time, int priority, int completed, const char *description)
{
    char *zErrMsg = 0;
    char sql[1024];

    // Account for optional description
    if (description)
    {
        snprintf(sql, sizeof(sql), "INSERT INTO tasks (name, date, time, priority, completed, description) "
                                   "VALUES ('%s', '%s', '%s', %d, %d, '%s');",
                 name, date, time, priority, completed, description);
    }
    else
    {
        snprintf(sql, sizeof(sql), "INSERT INTO tasks (name, date, time, priority, completed) "
                                   "VALUES ('%s', '%s', '%s', %d, %d);",
                 name, date, time, priority, completed);
    }

    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg); // Run SQL command

    // Error checking
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return rc;
    }

    return SQLITE_OK;
}

int RemoveEntry(sqlite3 *db, int id)
{
    char *zErrMsg = 0;
    char sql[1024];
    snprintf(sql, sizeof(sql), "DELETE FROM tasks WHERE id = %d;", id);

    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return rc;
    }

    return SQLITE_OK;
}

int RetrieveEntry(sqlite3 *db, const char *date)
{
    sqlite3_stmt *stmt;
    char sql[1024];
    snprintf(sql, sizeof(sql), "SELECT * FROM tasks WHERE date = '%s';", date);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        const unsigned char *task_date = sqlite3_column_text(stmt, 2);
        const unsigned char *time = sqlite3_column_text(stmt, 3);
        int priority = sqlite3_column_int(stmt, 4);
        int completed = sqlite3_column_int(stmt, 5);
        const unsigned char *description = sqlite3_column_text(stmt, 6);

        printf("ID: %d, Name: %s, Date: %s, Time: %s, Priority: %d, Completed: %d, Description: %s\n",
               id, name, task_date, time, priority, completed, description);
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}
