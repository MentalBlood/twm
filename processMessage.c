LRESULT CALLBACK processMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
    //printf("processMessage\n");
    switch (message) {
        case WM_CREATE:
            //printf("WM_CREATE\n");
            break;
        case WM_CLOSE:
            closeWindow(workspaces[currentWorkspaceIndex].currentWindow);
            break;
        case WM_HOTKEY:
            //printf("WM_HOTKEY\n");
            processHotKey(wParam);
            break;
        default:
            if (message == shellHookId) { // Handle the Shell Hook message
                switch (wParam) {
                    case HSHELL_WINDOWCREATED:
                        if (isGoodWindow((HWND)lParam)) {
                            addWindowFromHandleToCurrentWorkspace((HWND)lParam);
                        }
                        break;
                }
            }
            else
                return DefWindowProc(windowHandle, message, wParam, lParam);
    }
    return 0;
}