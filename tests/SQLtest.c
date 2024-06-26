#include <stdio.h>
#include <sqlite3.h>
#include "../src/SQL.h"

int main() {
    sqlite3 *db;
    

    // Initialize the database and table
    if (InitSQL(&db) != SQLITE_OK) {
        return 1;
    }

    // Add a task with a description
    if (AddEntry(db, "C moment", "2024-06-24", "14:00", 1, 0, "Complete C project") != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }

    // Add a task without a description
    if (AddEntry(db, "Stare at sun", "2024-06-24", "10:00", 2, 0, NULL) != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }

    // Retrieve tasks for a specific date
    if (RetrieveEntry(db, "2024-06-24") != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }

    // Retrieve tasks for a specific date
    if (RetrieveEntry(db, "2024-06-25") != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }

    // Remove a task
    if (RemoveEntry(db, 1) != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }

    // Close the database
    sqlite3_close(db);
    return 0;
}
