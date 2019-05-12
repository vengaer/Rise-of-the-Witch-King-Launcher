#ifndef WIN_GUI_H
#define WIN_GUI_H

#if defined __CYGWIN__ || defined _WIN32
#include <windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif
#endif
