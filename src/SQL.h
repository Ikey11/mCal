#ifndef SQL_H
#define SQL_H

#include <sqlite3.h>

// Initializes the database and ensures the tasks table exists
int InitSQL(sqlite3 **db);

// Adds a new entry to the tasks table
int AddEntry(sqlite3 *db, const char *name, const char *date, const char *time, int priority, int completed, const char *description);

// Removes an entry from the tasks table based on the id
int RemoveEntry(sqlite3 *db, int id);

// Retrieves entries from the tasks table based on the date
int RetrieveEntry(sqlite3 *db, const char *date);

#endif  // SQL_H