void getShellHookId(HWND windowHandle) {
    BOOL (__stdcall *RegisterShellHookWindow_)(HWND) =  // RegisterShellHookWindow function. For compatibillity we get it out of the dll though it is in the headers now
    (BOOL (__stdcall *)(HWND))GetProcAddress(GetModuleHandle("USER32.DLL"), "RegisterShellHookWindow");

    if (RegisterShellHookWindow_ == NULL)
        fatality("Could not find RegisterShellHookWindow");

    RegisterShellHookWindow_(windowHandle);
    shellHookId = RegisterWindowMessage("SHELLHOOK"); // Grab a dynamic id for the SHELLHOOK message to be used later
}

void registerSelfWindow(HINSTANCE hInstance) {
    WNDCLASSEX windowClass;

    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = 0;
    windowClass.lpfnWndProc = processMessage;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = NULL;
    windowClass.hIconSm = NULL;
    windowClass.hCursor = NULL;
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = NAME;

    if (!RegisterClassEx(&windowClass))
        fatality("Error Registering Window Class");
}

void ready(HINSTANCE hInstance, HWND windowHandle) {
    registerSelfWindow(hInstance);

    windowHandle = CreateWindowEx(0, NAME, NAME, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
    if (!windowHandle)
        fatality("Error while creating window");

    currentWorkArea = getWorkAreaData();
    registerDefaultHotKeys(windowHandle);

    setupWorkspaces();
    EnumWindows(EnumWindowsRestore, 0); // Restore windows on startup so they get tiled
    EnumWindows(EnumWindowsProc, 0);

    getShellHookId(windowHandle);
}