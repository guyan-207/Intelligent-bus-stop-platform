#ifndef _MENU_H
#define _MENU_H

#include "main.h"

typedef enum {
    MAIN_MENU,      // 主页面
    SETTING_MENU,   // 设置菜单（显示三个选项）
    TIME_SETTING,   // 时间调整子菜单
    BUS_SETTING,    // 公交线路调整子菜单
    LIGHT_SETTING   // 照明开关子菜单
} MenuState;

extern MenuState current_menu_state;

int menu1();
void menu2_setting(void);

#endif
