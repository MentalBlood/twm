#define NAME "NullTWM"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {false, true} bool;

#include "hotKeys.c"
#include "workArea.c"

unsigned int shellHookId;
WorkAreaData *currentWorkArea;

void fatality(const char *errorMessageText) {
    MessageBox(NULL, errorMessageText, "Error", MB_OK | MB_ICONERROR);
    exit(1);
}

#include "window.c"
#include "workspace.c"

#include "processHotKey.c"
#include "processMessage.c"
#include "initializing.c"

void fight() {
    MSG message;
    while (GetMessage(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    HWND windowHandle;

    ready(hInstance, windowHandle); //initializing
    fight(); //processing messages

    return 0;
}