void processHotKey(WPARAM wParam) {
    if (wParam >= KEY_TOGGLE_T1 && wParam < (KEY_TOGGLE_T1 + 10)) {
        moveCurrentWindowToWorkspace(wParam - KEY_TOGGLE_T1);
        return;
    } else
    if (wParam >= KEY_SWITCH_T1 && wParam < (KEY_SWITCH_T1 + 10)) {
        setWorkspace(wParam - KEY_SWITCH_T1);
        return;
    }
    switch (wParam) {
        case KEY_EXIT:
            fatality("exit");
            break;
        case KEY_CLOSE_WINDOW:
            PostMessage(workspaces[currentWorkspaceIndex].currentWindow->handle, WM_CLOSE, 0, 0);
            closeWindow(workspaces[currentWorkspaceIndex].currentWindow);
            removeCurrentWindowFromWorkspace();
            break;
        case KEY_CHANGE_SPLIT_MODE:
            changeSplitMode();
            break;
        case KEY_SELECT_LEFT:
            focusNearbyWindow(LEFT);
            break;
        case KEY_SELECT_RIGHT:
            focusNearbyWindow(RIGHT);
            break;
        case KEY_SELECT_UP:
            focusNearbyWindow(UP);
            break;
        case KEY_SELECT_DOWN:
            focusNearbyWindow(DOWN);
            break;
        case KEY_MOVE_LEFT:
            swapWithNearbyWindow(LEFT);
            break;
        case KEY_MOVE_RIGHT:
            swapWithNearbyWindow(RIGHT);
            break;
        case KEY_MOVE_UP:
            swapWithNearbyWindow(UP);
            break;
        case KEY_MOVE_DOWN:
            swapWithNearbyWindow(DOWN);
            break;
        case KEY_CHANGE_SIZE_LEFT:
            if (!movingWindowSide)
                startMovingWindowSide(LEFT_SIDE);
            else {
                movingWindowSide = false;
                if (currentMovingSide == LEFT_SIDE)
                    moveWindowSide(getCurrentWindow(), LEFT_SIDE, FROM_CENTER);
                else
                if (currentMovingSide == RIGHT_SIDE)
                    moveWindowSide(getCurrentWindow(), RIGHT_SIDE, TO_CENTER);
            }
            break;
        case KEY_CHANGE_SIZE_RIGHT:
            if (!movingWindowSide)
                startMovingWindowSide(RIGHT_SIDE);
            else {
                movingWindowSide = false;
                if (currentMovingSide == LEFT_SIDE)
                    moveWindowSide(getCurrentWindow(), LEFT_SIDE, TO_CENTER);
                else
                if (currentMovingSide == RIGHT_SIDE)
                    moveWindowSide(getCurrentWindow(), RIGHT_SIDE, FROM_CENTER);
            }
            break;
        case KEY_CHANGE_SIZE_TOP:
            if (!movingWindowSide)
                startMovingWindowSide(TOP_SIDE);
            else {
                movingWindowSide = false;
                if (currentMovingSide == TOP_SIDE)
                    moveWindowSide(getCurrentWindow(), TOP_SIDE, FROM_CENTER);
                else
                if (currentMovingSide == BOTTOM_SIDE)
                    moveWindowSide(getCurrentWindow(), BOTTOM_SIDE, TO_CENTER);
            }
            break;
        case KEY_CHANGE_SIZE_BOTTOM:
            if (!movingWindowSide)
                startMovingWindowSide(BOTTOM_SIDE);
            else {
                movingWindowSide = false;
                if (currentMovingSide == TOP_SIDE)
                    moveWindowSide(getCurrentWindow(), TOP_SIDE, TO_CENTER);
                else
                if (currentMovingSide == BOTTOM_SIDE)
                    moveWindowSide(getCurrentWindow(), BOTTOM_SIDE, FROM_CENTER);
            }
            break;
    }
}