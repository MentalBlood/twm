void setupWorkspaces() {
    workspaces = (Workspace*)malloc(sizeof(Workspace) * 10);
    unsigned int i = 0;
    for (; i < 10; i++) {
        workspaces[i].windows = NULL;
        workspaces[i].currentWindow = NULL;
    }
    currentWorkspaceIndex = 0;
}

void setCurrentWindow(Window *window) {
    workspaces[currentWorkspaceIndex].currentWindow = window;
}

void focusCurrentWindow() {
    focusWindow(getCurrentWindow());
}

void setWorkspace(unsigned int newWorkspaceIndex) {
    printf("setWorkspace %d\n", newWorkspaceIndex);
    if (newWorkspaceIndex == currentWorkspaceIndex)
        return;

    Window *windowPointer = workspaces[currentWorkspaceIndex].windows;
    for (; windowPointer; windowPointer = windowPointer->next)
        minimizeWindow(windowPointer);
    
    currentWorkspaceIndex = newWorkspaceIndex;
    windowPointer = workspaces[currentWorkspaceIndex].windows;
    for (; windowPointer; windowPointer = windowPointer->next)
        restoreWindow(windowPointer);
    
    focusCurrentWindow();
}

#define VERTICALLY true
#define HORIZONTALLY false
bool splitMode = VERTICALLY;

void changeSplitMode() {
    splitMode = !splitMode;
}

void addWindowToWorkspace(Window *window, unsigned int workspaceIndex) {
    if (workspaces[workspaceIndex].windows) {
        if (splitMode == VERTICALLY)
            attachWindowLeft(workspaces[workspaceIndex].currentWindow, window);
        else
            attachWindowDown(workspaces[workspaceIndex].currentWindow, window);
        changeSplitMode();
    }
    else {
        makeWindowFullscreen(window);
        splitMode = VERTICALLY;
    }

    window->next = workspaces[workspaceIndex].windows;
    if (window->next)
        window->next->previous = window;
    workspaces[workspaceIndex].windows = window;
    workspaces[workspaceIndex].currentWindow = window;
    setCurrentWindow(window);
    if (workspaceIndex == currentWorkspaceIndex)
        focusCurrentWindow();
}

void rearrangeWindows() {
    printf("rearrange\n");
    Window *window = workspaces[currentWorkspaceIndex].windows;
    if (!window)
        return;
    makeWindowFullscreen(window);
    
    splitMode = VERTICALLY;
    window = window->next;
    for (; window; window = window->next) {
        if (splitMode == VERTICALLY)
            attachWindowLeft(window->previous, window);
        else
            attachWindowDown(window->previous, window);
        changeSplitMode();
    }
    printf("rearranged\n");
}

void addWindowFromHandleToCurrentWorkspace(HWND windowHandle) {
    addWindowToWorkspace(windowFromHandle(windowHandle), currentWorkspaceIndex);
}

void removeCurrentWindowFromWorkspace() {
    printf("removeCurrentWindowFromWorkspace\n");
    Window *currentWindow = getCurrentWindow();
    if (!currentWindow)
        return;
    if (currentWindow->next)
        currentWindow->next->previous = currentWindow->previous;
    if (currentWindow->previous)
        currentWindow->previous->next = currentWindow->next;
    else
        workspaces[currentWorkspaceIndex].windows = currentWindow->next;

    if (currentWindow->next)
        setCurrentWindow(currentWindow->next);
    else
        setCurrentWindow(workspaces[currentWorkspaceIndex].currentWindow->previous);
    free(currentWindow);

    rearrangeWindows();
    restoreWindow(getCurrentWindow());
}

void moveCurrentWindowToWorkspace(unsigned int workspaceIndex) {
    if (workspaceIndex == currentWorkspaceIndex)
        return;

    Window *currentWindow = getCurrentWindow();
    removeCurrentWindowFromWorkspace();

    workspaces[currentWorkspaceIndex].currentWindow = workspaces[currentWorkspaceIndex].windows;
    focusCurrentWindow();
    
    minimizeWindow(currentWindow);
    printf("minimized\n");
    addWindowToWorkspace(currentWindow, workspaceIndex);
    printf("added\n");
}

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

bool intersects(unsigned int a1, unsigned int b1, unsigned int a2, unsigned int b2) {
    return (min(b1, b2) - max(a1, a2)) > 0;
}

long isNearbyThisWindow(Window *window, Window *anotherWindow,  char direction) {
    RECT    windowPosition = getWindowPosition(window),
            anotherWindowPosition = getWindowPosition(anotherWindow);

    return  (
                (
                    (
                        ((direction == LEFT) && (windowPosition.left == anotherWindowPosition.right))
                        || ((direction == RIGHT) && (windowPosition.right == anotherWindowPosition.left))
                    )
                    && intersects(windowPosition.top, windowPosition.bottom, anotherWindowPosition.top, anotherWindowPosition.bottom)
                )
                ||
                (
                    (
                        ((direction == UP) && (windowPosition.top == anotherWindowPosition.bottom))
                        || ((direction == DOWN) && (windowPosition.bottom == anotherWindowPosition.top))
                    )
                    && intersects(windowPosition.left, windowPosition.right, anotherWindowPosition.left, anotherWindowPosition.right)
                )
            );
}

long getDeltaFromCurrentWindow(Window *window, char direction) {
    if ((direction == LEFT) || (direction == RIGHT))
        return abs(getWindowCenterY(window) - getWindowCenterY(getCurrentWindow()));
    else
        return abs(getWindowCenterX(window) - getWindowCenterX(getCurrentWindow()));
}

Window* findNearbyWindow(char direction) {
    Window  *window = workspaces[currentWorkspaceIndex].windows,
            *currentWindow = getCurrentWindow();
    
    if ((!currentWindow) || (!window->next))
        return NULL;    
    
    Window *windowWithMinDelta = NULL;
    printf("------------------------------------------------------\n");
    long minDelta = 0;
    for (; window; window = window->next) {
        if ((window == currentWindow) || (!isNearbyThisWindow(currentWindow, window, direction)))
            continue;
        long delta = getDeltaFromCurrentWindow(window, direction);
        printf("---------------------------------delta = %ld\n", delta);
        if ((delta < minDelta) || (!windowWithMinDelta)) {
            minDelta = delta;
            windowWithMinDelta = window;
        }
    }

    printf("minDelta == %ld\n", minDelta);
    return windowWithMinDelta;
}

bool movingWindowSide = false;

void focusNearbyWindow(char direction) {
    movingWindowSide = false;

    Window *nearbyWindow = findNearbyWindow(direction);
    if (!nearbyWindow)
        return;

    focusWindow(nearbyWindow);
    setCurrentWindow(nearbyWindow);
}

void swapWithNearbyWindow(char direction) {
    movingWindowSide = false;

    Window *nearbyWindow = findNearbyWindow(direction);
    if (!nearbyWindow)
        return;
    
    swapCurrentWindowWith(nearbyWindow);
}

//changing window size

bool windowSideMovementPossible;
char currentMovingSide;

void startMovingWindowSide(char side) {
    movingWindowSide = true;
    windowSideMovementPossible = true;
    currentMovingSide = side;

    Window *window = workspaces[currentWorkspaceIndex].windows;
    for (; window; window = window->next) {
        window->sizeChanged = false;
        if (window->possibleMovement) {
           free(window->possibleMovement);
            window->possibleMovement = NULL;
        }
    }
}

bool tryToMoveWindowSide(Window *window, char side, bool direction) {
    if (window->sizeChanged)
        return true;
    if (!windowSideMovementPossible)
        return false;

    RECT currentPosition = getWindowPosition(window);
    Window *anotherWindow = workspaces[currentWorkspaceIndex].windows;

    if (side == LEFT_SIDE) {
        if (direction == FROM_CENTER) {
            if (currentPosition.left > 0) {
                
                window->sizeChanged = true;
                for (; anotherWindow; anotherWindow = anotherWindow->next)
                    if ((anotherWindow != window) && isNearbyThisWindow(window, anotherWindow, LEFT))
                        if (!tryToMoveWindowSide(anotherWindow, RIGHT_SIDE, TO_CENTER)) {
                            windowSideMovementPossible = false;
                            return false;
                        }

                savePossibleMovement(window, side, direction);
            }
        } else {
            if (currentPosition.left == 0)
                return true;
            if ((currentPosition.right - currentPosition.left) < 400)
                return false;

            window->sizeChanged = true;
            for (; anotherWindow; anotherWindow = anotherWindow->next)
                if ((anotherWindow != window) && isNearbyThisWindow(window, anotherWindow, LEFT))
                    if (!tryToMoveWindowSide(anotherWindow, RIGHT_SIDE, FROM_CENTER)) {
                        windowSideMovementPossible = false;
                        return false;
                    }
        
            savePossibleMovement(window, side, direction);
        }
    } else
    if (side == RIGHT_SIDE) {
        if  (direction == FROM_CENTER) {
            if (currentPosition.right < currentWorkArea->width) {

                window->sizeChanged = true;
                for (; anotherWindow; anotherWindow = anotherWindow->next)
                    if ((anotherWindow != window) && isNearbyThisWindow(window, anotherWindow, RIGHT))
                        if (!tryToMoveWindowSide(anotherWindow, LEFT_SIDE, TO_CENTER)) {
                            windowSideMovementPossible = false;
                            return false;
                        }

                savePossibleMovement(window, side, direction);
            }
        } else {
            if (currentPosition.right == currentWorkArea->width)
                return true;
            if ((currentPosition.right - currentPosition.left) < 400)
                return false;

            window->sizeChanged = true;
            for (; anotherWindow; anotherWindow = anotherWindow->next)
                if ((anotherWindow != window) && isNearbyThisWindow(window, anotherWindow, RIGHT))
                    if (!tryToMoveWindowSide(anotherWindow, LEFT_SIDE, FROM_CENTER)) {
                        windowSideMovementPossible = false;
                        return false;
                    }

            savePossibleMovement(window, side, direction);
        }
    } else
    if (side == TOP_SIDE) {
        if  (direction == FROM_CENTER) {
            if (currentPosition.top > 0) {

                window->sizeChanged = true;
                for (; anotherWindow; anotherWindow = anotherWindow->next)
                    if ((anotherWindow != window) && isNearbyThisWindow(window, anotherWindow, UP))
                        if (!tryToMoveWindowSide(anotherWindow, BOTTOM_SIDE, TO_CENTER)) {
                            windowSideMovementPossible = false;
                            return false;
                        }

                savePossibleMovement(window, side, direction);
            }
        } else {
            if (currentPosition.top == 0)
                return true;
            if ((currentPosition.bottom - currentPosition.top) < 400)
                return false;

            window->sizeChanged = true;
            for (; anotherWindow; anotherWindow = anotherWindow->next)
                if ((anotherWindow != window) && isNearbyThisWindow(window, anotherWindow, UP))
                    if (!tryToMoveWindowSide(anotherWindow, BOTTOM_SIDE, FROM_CENTER)) {
                        windowSideMovementPossible = false;
                        return false;
                    }

            savePossibleMovement(window, side, direction);
        }
    } else
    if (side == BOTTOM_SIDE) {
        if  (direction == FROM_CENTER) {
            if (currentPosition.bottom < currentWorkArea->height) {

                window->sizeChanged = true;
                for (; anotherWindow; anotherWindow = anotherWindow->next)
                    if ((anotherWindow != window) && isNearbyThisWindow(window, anotherWindow, DOWN))
                        if (!tryToMoveWindowSide(anotherWindow, TOP_SIDE, TO_CENTER)) {
                            windowSideMovementPossible = false;
                            return false;
                        }
                
                savePossibleMovement(window, side, direction);
            }
        } else {
            if (currentPosition.bottom == currentWorkArea->height)
                return true;
            if ((currentPosition.bottom - currentPosition.top) < 400)
                return false;

            window->sizeChanged = true;
            for (; anotherWindow; anotherWindow = anotherWindow->next)
                if ((anotherWindow != window) && isNearbyThisWindow(window, anotherWindow, DOWN))
                    if (!tryToMoveWindowSide(anotherWindow, TOP_SIDE, FROM_CENTER)) {
                        windowSideMovementPossible = false;
                        return false;
                    }
            
            savePossibleMovement(window, side, direction);
        }
    }
}

void doSideMovement(Window *window) {
    if (!window->possibleMovement)
        return;

    RECT currentPosition = getWindowPosition(window);

    if (window->possibleMovement->side == LEFT_SIDE) {
        if (window->possibleMovement->direction == FROM_CENTER) {
            unsigned int distanceToMove = min(16, currentPosition.left);
            setWindowPosition(window, currentPosition.left - distanceToMove, currentPosition.top, currentPosition.right - currentPosition.left + distanceToMove, currentPosition.bottom - currentPosition.top, SWP_SHOWWINDOW);
        } else {
            unsigned int distanceToMove = min(16, currentWorkArea->width - currentPosition.left);
            setWindowPosition(window, currentPosition.left + distanceToMove, currentPosition.top, currentPosition.right - currentPosition.left - distanceToMove, currentPosition.bottom - currentPosition.top, SWP_SHOWWINDOW);
        }
    } else
    if (window->possibleMovement->side == RIGHT_SIDE) {
        if  (window->possibleMovement->direction == FROM_CENTER) {
            unsigned int distanceToMove = min(16, currentWorkArea->width - currentPosition.right);
            setWindowPosition(window, 0, 0, currentPosition.right - currentPosition.left + distanceToMove, currentPosition.bottom - currentPosition.top, SWP_NOMOVE);
        } else {
            unsigned int distanceToMove = min(16, currentPosition.right);
            setWindowPosition(window, 0, 0, currentPosition.right - currentPosition.left - distanceToMove, currentPosition.bottom - currentPosition.top, SWP_NOMOVE);
        }
    } else
    if (window->possibleMovement->side == TOP_SIDE) {
        if  (window->possibleMovement->direction == FROM_CENTER) {
            unsigned int distanceToMove = min(16, currentPosition.top);
            setWindowPosition(window, currentPosition.left, currentPosition.top - distanceToMove, currentPosition.right - currentPosition.left, currentPosition.bottom - currentPosition.top + distanceToMove, SWP_SHOWWINDOW);
        } else {
            unsigned int distanceToMove = min(16, currentWorkArea->height - currentPosition.top);
            setWindowPosition(window, currentPosition.left, currentPosition.top + distanceToMove, currentPosition.right - currentPosition.left, currentPosition.bottom - currentPosition.top - distanceToMove, SWP_SHOWWINDOW);
        }
    } else
    if (window->possibleMovement->side == BOTTOM_SIDE) {
        if  (window->possibleMovement->direction == FROM_CENTER) {
            unsigned int distanceToMove = min(16, currentWorkArea->height - currentPosition.bottom);
            setWindowPosition(window, 0, 0, currentPosition.right - currentPosition.left, currentPosition.bottom - currentPosition.top + distanceToMove, SWP_NOMOVE);
        } else {
            unsigned int distanceToMove = min(16, currentPosition.bottom);
            setWindowPosition(window, 0, 0, currentPosition.right - currentPosition.left, currentPosition.bottom - currentPosition.top - distanceToMove, SWP_NOMOVE);
        }
    }
}

void doSidesMovements() {
    Window *window = workspaces[currentWorkspaceIndex].windows;
    for (; window; window = window->next)
        doSideMovement(window);
}

void moveWindowSide(Window *window, char side, bool direction) {
    tryToMoveWindowSide(window, side, direction);
    if (windowSideMovementPossible)
        doSidesMovements();
}

// Callbacks for adding desktop windows to the grid to be tiled

BOOL CALLBACK EnumWindowsProc(HWND windowHandle, LPARAM lParam) {
    printf("EnumWindowsProc\n");
    if (isGoodWindow(windowHandle))
        addWindowFromHandleToCurrentWorkspace(windowHandle);
    else {
        printf("bad\n");
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK EnumWindowsRestore(HWND windowHandle, LPARAM lParam) {
    printf("EnumWindowsRestore\n");
    if (isGoodWindow(windowHandle))
        restoreWindowFromHadnle(windowHandle);
    else {
        printf("bad\n");
        return FALSE;
    }
    return TRUE;
}