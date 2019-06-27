int hotKeysModifiers = MOD_CONTROL;

enum hotKeyMessage {
    KEY_CHANGE_SPLIT_MODE = 1,

    KEY_SELECT_UP,
    KEY_SELECT_DOWN,
    KEY_SELECT_LEFT,
    KEY_SELECT_RIGHT,
    
    KEY_MOVE_UP,
    KEY_MOVE_DOWN,
    KEY_MOVE_LEFT,
    KEY_MOVE_RIGHT,

    KEY_CHANGE_SIZE_LEFT,
    KEY_CHANGE_SIZE_RIGHT,
    KEY_CHANGE_SIZE_TOP,
    KEY_CHANGE_SIZE_BOTTOM,

    KEY_CLOSE_WINDOW,
    KEY_SWITCH_T1 = 100,
    KEY_TOGGLE_T1 = 200,
    KEY_EXIT
};

typedef struct HotKey {
    int message;
    unsigned int virtualKeyCode;
    struct HotKey *nextInLinkedList;
} HotKey;

typedef struct HotKeysMap {
    int hotKeysModifiers;
    HotKey *hotKeysLinkedListBegin;
} HotKeysMap;

void registerDefaultHotKeys(HWND windowHandle) {
    RegisterHotKey(windowHandle, KEY_CHANGE_SPLIT_MODE, hotKeysModifiers, 0x55); //u

    RegisterHotKey(windowHandle, KEY_SELECT_UP, hotKeysModifiers, 0x49); //i
    RegisterHotKey(windowHandle, KEY_SELECT_DOWN, hotKeysModifiers, 0x4B); //k
    RegisterHotKey(windowHandle, KEY_SELECT_LEFT, hotKeysModifiers, 0x4A); //j
    RegisterHotKey(windowHandle, KEY_SELECT_RIGHT, hotKeysModifiers, 0x4C); //l

    RegisterHotKey(windowHandle, KEY_CHANGE_SIZE_TOP, hotKeysModifiers, 0x57); //w
    RegisterHotKey(windowHandle, KEY_CHANGE_SIZE_BOTTOM, hotKeysModifiers, 0x53); //s
    RegisterHotKey(windowHandle, KEY_CHANGE_SIZE_LEFT, hotKeysModifiers, 0x41); //a
    RegisterHotKey(windowHandle, KEY_CHANGE_SIZE_RIGHT, hotKeysModifiers, 0x44); //d

    RegisterHotKey(windowHandle, KEY_MOVE_UP, hotKeysModifiers | MOD_SHIFT, 0x49); //i
    RegisterHotKey(windowHandle, KEY_MOVE_DOWN, hotKeysModifiers | MOD_SHIFT, 0x4B); //k
    RegisterHotKey(windowHandle, KEY_MOVE_LEFT, hotKeysModifiers | MOD_SHIFT, 0x4A); //j
    RegisterHotKey(windowHandle, KEY_MOVE_RIGHT, hotKeysModifiers | MOD_SHIFT, 0x4C); //l

    RegisterHotKey(windowHandle, KEY_CLOSE_WINDOW, hotKeysModifiers | MOD_SHIFT, 0x43); //c
    RegisterHotKey(windowHandle, KEY_EXIT, hotKeysModifiers | MOD_SHIFT, 0x51); //q

    char key[2];
    unsigned int i = 0;
    for (; i < 10; i++) {
        sprintf(key, "%d", i);
        RegisterHotKey(windowHandle, KEY_SWITCH_T1 + i, hotKeysModifiers, *key);
        RegisterHotKey(windowHandle, KEY_TOGGLE_T1 + i, hotKeysModifiers | MOD_SHIFT, *key); // Toggle tag N
    }
}

void unregisterDefaultHotKeys(HWND windowHandle) {
    int i = 1;
    for (; i <= 27; i++)
        UnregisterHotKey(windowHandle, 10);
}