#ifndef MENU_H
#define MENU_H



typedef enum {
    MENU_NONE,
    MENU_MAIN,
    MENU_PAUSE,
    MENU_INVENTORY,
    MENU_EQUIPMENT, // flags for is owner, target, visibility level 
    MENU_TRADE,
    MENU_SKILLS,
    MENU_MAP,
    MENU_DIALOGUE,
    MENU_DEBUG,
} MenuType;

typedef enum {
    MENU_MAIN_NEW_GAME,
    MENU_MAIN_CONTINUE,
    MENU_MAIN_LOAD_GAME,
    MENU_MAIN_SETTINGS,
    MENU_MAIN_ABOUT,
    MENU_MAIN_EXIT,
} MenuMainSubType;

typedef enum {
    MENU_PAUSE_RESUME,
    MENU_PAUSE_SAVE,
    MENU_PAUSE_LOAD,
    MENU_PAUSE_SETTINGS,
    MENU_PAUSE_MAIN_MENU,
    MENU_PAUSE_EXIT,
} MenuPauseSubType;




#endif // MENUS_H