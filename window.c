//Function for filtering windows

bool isGoodWindow(HWND windowHandle) {
    printf("isGoodWindow\n");
    if ((!IsWindowVisible(windowHandle)) || (GetParent(windowHandle) != 0))
        return false;
    printf("1\n");
    
    int exstyle = GetWindowLong(windowHandle, GWL_EXSTYLE);
    HWND owner = GetWindow(windowHandle, GW_OWNER);
    if (!(
            (
                ((exstyle & WS_EX_TOOLWINDOW) == 0) && (owner == 0)
            )
            ||
            (
                (exstyle & WS_EX_APPWINDOW) && (owner != 0)
            )
        ))
        return false;
    
    RECT rect;
    GetWindowRect(windowHandle, &rect);
    if  (
            (!(
                ((rect.left > currentWorkArea->x) || (rect.right > currentWorkArea->x))
                &&
                ((rect.left < (currentWorkArea->x + currentWorkArea->width)) || (rect.right < (currentWorkArea->x + currentWorkArea->width)))
            ))
            ||
            (!(
                ((rect.top > currentWorkArea->y) || (rect.bottom > currentWorkArea->y))
                &&
                ((rect.top < (currentWorkArea->y + currentWorkArea->height)) || (rect.bottom < (currentWorkArea->y + currentWorkArea->height)))
            ))
        )
        return false;

    //printf("good\n");
    return true;
}

#define LEFT_SIDE 0
#define RIGHT_SIDE 1
#define TOP_SIDE 2
#define BOTTOM_SIDE 3

#define FROM_CENTER 0
#define TO_CENTER 1

typedef struct Movement {
    char side;
    bool direction;
} Movement;

typedef struct Window {
    struct Window   *previous,
                    *next;
    HWND handle;
    bool sizeChanged;
    Movement *possibleMovement;
} Window;

void savePossibleMovement(Window *window, char side, bool direction) {
    window->possibleMovement = (Movement*)malloc(sizeof(Movement));
    window->possibleMovement->side = side;
    window->possibleMovement->direction = direction;
}

typedef struct Workspace {
    Window  *windows,
            *currentWindow;
} Workspace;

Workspace *workspaces;
unsigned int currentWorkspaceIndex;

Window* getCurrentWindow() {
    return workspaces[currentWorkspaceIndex].currentWindow;
}

Window* windowFromHandle(HWND windowHandle) {
    printf("windowFromHandle\n");
    Window **window = (Window**)malloc(sizeof(Window*));
    *window = (Window*)malloc(sizeof(Window));
    
    WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
    GetWindowPlacement(windowHandle, &placement);
    printf("(%ld %ld) (%ld %ld)\n", placement.rcNormalPosition.left, placement.rcNormalPosition.top, placement.rcNormalPosition.right, placement.rcNormalPosition.bottom);
    
    (*window)->handle = windowHandle;
    (*window)->next = NULL;
    (*window)->previous = NULL;
    (*window)->possibleMovement = NULL;
    return *window;
}

//Actions with window

void focusWindow(Window *window) {
    if (window)
        SetForegroundWindow(window->handle);
}

void closeWindow(Window *window) {
    DeregisterShellHookWindow(window->handle);
    unregisterDefaultHotKeys(window->handle);
    DestroyWindow(window->handle);
}

RECT getWindowPosition(Window *window) {
    WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
    GetWindowPlacement(window->handle, &placement);
    return placement.rcNormalPosition;
}

long getWindowCenterX(Window *window) {
    RECT windowPosition = getWindowPosition(window);
    return windowPosition.left + (windowPosition.right - windowPosition.left) / 2;
}

long getWindowCenterY(Window *window) {
    RECT windowPosition = getWindowPosition(window);
    return windowPosition.top + (windowPosition.bottom - windowPosition.top) / 2;
}

void setWindowPosition(Window *window, int x, int y, int width, int height, unsigned int options) {
    SetWindowPos(window->handle, NULL, x, y, width, height, options);
}

void splitWindowVertically(Window *window) {
    RECT currentPosition = getWindowPosition(window);
    setWindowPosition(window, 0, 0, (currentPosition.right - currentPosition.left) / 2, currentPosition.bottom - currentPosition.top, SWP_NOMOVE);
}

void splitWindowHorizontally(Window *window) {
    RECT currentPosition = getWindowPosition(window);
    setWindowPosition(window, 0, 0, currentPosition.right - currentPosition.left, (currentPosition.bottom - currentPosition.top) / 2, SWP_NOMOVE);
}

void makeWindowFullscreen(Window *window) {
    setWindowPosition(window, currentWorkArea->x, currentWorkArea->y, currentWorkArea->width, currentWorkArea->height, SWP_SHOWWINDOW);
}

void attachWindowLeft(Window *oldWindow, Window *newWindow) {
    splitWindowVertically(oldWindow);

    RECT currentWindowPosition = getWindowPosition(oldWindow);
    setWindowPosition(newWindow, currentWindowPosition.right, currentWindowPosition.top, currentWindowPosition.right - currentWindowPosition.left, currentWindowPosition.bottom - currentWindowPosition.top, SWP_SHOWWINDOW);
}

void attachWindowDown(Window *oldWindow, Window *newWindow) {
    splitWindowHorizontally(oldWindow);

    RECT currentWindowPosition = getWindowPosition(oldWindow);
    setWindowPosition(newWindow, currentWindowPosition.left, currentWindowPosition.bottom, currentWindowPosition.right - currentWindowPosition.left, currentWindowPosition.bottom - currentWindowPosition.top, SWP_SHOWWINDOW);
}

void swapCurrentWindowWith(Window *anotherWindow) {
    Window *currentWindow = getCurrentWindow();
    RECT    currentWindowPosition = getWindowPosition(currentWindow),
            anotherWindowPosition = getWindowPosition(anotherWindow);
    setWindowPosition(anotherWindow, currentWindowPosition.left, currentWindowPosition.top, currentWindowPosition.right - currentWindowPosition.left, currentWindowPosition.bottom - currentWindowPosition.top, SWP_SHOWWINDOW);
    setWindowPosition(currentWindow, anotherWindowPosition.left, anotherWindowPosition.top, anotherWindowPosition.right - anotherWindowPosition.left, anotherWindowPosition.bottom - anotherWindowPosition.top, SWP_SHOWWINDOW);
}

void minimizeWindow(Window *window) {
    printf("minimizeWindow\n");
    WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
    GetWindowPlacement(window->handle, &placement);
    placement.showCmd = SW_SHOWMINIMIZED;
    SetWindowPlacement(window->handle, &placement);
    printf("OK\n");
}

void restoreWindowFromHadnle(HWND windowHandle) {
    printf("restoreWindowFromHadnle\n");
    WINDOWPLACEMENT placement = { sizeof(WINDOWPLACEMENT) };
    GetWindowPlacement(windowHandle, &placement);
    placement.showCmd = SW_RESTORE;
    SetWindowPlacement(windowHandle, &placement);
    printf("OK\n");
}

void restoreWindow(Window *window) {
    if (window != NULL)
        restoreWindowFromHadnle(window->handle);
}