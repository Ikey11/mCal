#include "Logger.h"

WINDOW *console_win = NULL; // Initialize the global console window
bool console_enabled = false; // Initialize the console enabled flag

void LogMessage(LogLevel level, const char *format, ...) {
    if (!console_enabled || console_win == NULL) {
        return; // If the console is not enabled or not initialized, return
    }

    va_list args;
    va_start(args, format);
    
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    switch (level) {
        case LOG_INFO:
            wattron(console_win, COLOR_PAIR(0));
            wprintw(console_win, "INFO: %s\n", buffer);
            wattroff(console_win, COLOR_PAIR(0));
            break;
        case LOG_WARNING:
            wattron(console_win, COLOR_PAIR(3));
            wprintw(console_win, "WARNING: %s\n", buffer);
            wattroff(console_win, COLOR_PAIR(3));
            break;
        case LOG_ERROR:
            wattron(console_win, COLOR_PAIR(1));
            wprintw(console_win, "ERROR: %s\n", buffer);
            wattroff(console_win, COLOR_PAIR(1));
            break;
    }
    
    wrefresh(console_win);
    va_end(args);
}
