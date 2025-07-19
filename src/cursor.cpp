#ifdef _WIN32
#include <windows.h>
#else
#include <curses.h>
#endif

#include "cursor.hpp"
#include <iostream>

void initCursorControl()
{
#ifndef _WIN32
    initscr();
    cbreak();
    noecho();
#endif
}

void hideCursor()
{
#ifdef _WIN32
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
#else
    curs_set(0);
#endif
}

void showCursor()
{
#ifdef _WIN32
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
#else
    curs_set(1);
    endwin(); // clean up curses
#endif
}