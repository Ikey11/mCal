#include <stdio.h>
#include <sqlite3.h>

#include "Logger.h"
#include "SQL.h"

/// @brief Initializes the database and ensures the tasks table exists
/// @param db Database to be inited
/// @return SQLITE error code
int InitSQL(sqlite3 **db)
{
    int rc = sqlite3_open("calendar.db", db);
    if (rc)
    {
        LOG_ERROR("SQL::InitSQL: Can't open database: %s", sqlite3_errmsg(*db));
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
        LOG_ERROR("SQL::InitSQL: SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return rc;
    }

    return SQLITE_OK;
}

/// @brief Adds an entry to the tasks table
/// @param db Database
/// @param id   Outputs the id of the entry
/// @param name Task name
/// @param date
/// @param time
/// @param priority The importance of the task (0-9)
/// @param completed    Whether the task is complete
/// @param description (optional) additional information about the task
/// @return
int AddEntry(sqlite3 *db, sqlite3_int64 *id, const char *name, const char *date, const char *time, int priority, int completed, const char *description)
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
        LOG_ERROR("SQL::AddEntry: SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return rc;
    }

    LOG_INFO("Entry added!");

    // Get the ID of the newly inserted row
    sqlite3_int64 ID = sqlite3_last_insert_rowid(db);

    *id = ID;

    LOG_INFO("SQL::AddEntry: Added %s with ID %lld", name, *id);

    return SQLITE_OK;
}

int RemoveEntry(sqlite3 *db, sqlite3_int64 id)
{
    char *zErrMsg = 0;
    char sql[1024];
    snprintf(sql, sizeof(sql), "DELETE FROM tasks WHERE id = %lld;", id);

    int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        LOG_ERROR("SQL::RemoveEntry: SQL error: %s\n", zErrMsg);
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
        LOG_ERROR("SQL::RetrieveEntry: Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    LOG_INFO("SQL::RetrieveEntry: Retrieving tasks at date: %s", date);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        const unsigned char *task_date = sqlite3_column_text(stmt, 2);
        const unsigned char *time = sqlite3_column_text(stmt, 3);
        int priority = sqlite3_column_int(stmt, 4);
        int completed = sqlite3_column_int(stmt, 5);
        const unsigned char *description = sqlite3_column_text(stmt, 6);
        LOG_INFO("  ID: %d, Name: %s, Date: %s, Time: %s, Priority: %d, Completed: %d, Description: %s",
                 id, name, task_date, time, priority, completed, description);
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

/// @brief Toggles the completion status of an entry in the tasks table
/// @param db Database
/// @param id The ID of the entry to be toggled
void CompleteEntry(sqlite3 *db, sqlite3_int64 id)
{
    char *zErrMsg = 0;
    char sql[1024];
    sqlite3_stmt *stmt;
    int rc;
    int current_status;

    // Prepare SQL statement to get the current completion status
    snprintf(sql, sizeof(sql), "SELECT completed FROM tasks WHERE id = %lld;", id);

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        LOG_ERROR("SQL::CompleteEntry: Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Execute the statement and get the current completion status
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        current_status = sqlite3_column_int(stmt, 0);
    } else {
        LOG_ERROR("SQL::CompleteEntry: Failed to get current status for ID %lld\n", id);
        sqlite3_finalize(stmt);
        return;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    // Toggle the completion status
    int new_status = !current_status;

    // Prepare SQL statement to update the completion status
    snprintf(sql, sizeof(sql), "UPDATE tasks SET completed = %d WHERE id = %lld;", new_status, id);

    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg); // Run SQL command

    // Error checking
    if (rc != SQLITE_OK) {
        LOG_ERROR("SQL::CompleteEntry: SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return;
    }

    LOG_INFO("SQL::CompleteEntry: Toggled completion status for ID %lld to %d", id, new_status);
}