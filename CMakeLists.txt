cmake_minimum_required(VERSION 3.10)

# Set the project name
project(mcal)

# Find packages
find_path(NCURSESW_INCLUDE_DIR ncursesw/ncurses.h)
find_library(NCURSESW_LIBRARY ncursesw)
find_package(SQLite3 REQUIRED)

# Include directories
include_directories(${NCURSESW_INCLUDE_DIR} ${SQLite3_INCLUDE_DIRS})

# Add the main executable
add_executable(mcal src/main.c src/DLL.c src/EntryMan.c src/SQL.c src/MenuMan.c)

# Link libraries to the main executable
target_link_libraries(mcal ${NCURSESW_LIBRARY} ${SQLite3_LIBRARIES})

# Add source files for the doubly linked list library
add_library(doubly_linked_list src/DLL.c)

# Add test executables
add_executable(testDLL tests/DLLtests.c src/DLL.c)
add_executable(testSQL tests/SQLtest.c src/SQL.c src/DLL.c)

# Link libraries to test executables
target_link_libraries(testDLL ${NCURSESW_LIBRARY})
target_link_libraries(testSQL ${SQLite3_LIBRARIES} ${NCURSESW_LIBRARY})

# Enable testing and add the tests directory
enable_testing()

# Add tests
add_test(NAME DLLtests COMMAND testDLL)
add_test(NAME SQLtest COMMAND testSQL)
