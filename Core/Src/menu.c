#include "OLED.h"
#include "bus_info.h"
#include "key.h"
#include "menu.h"
#include "setting.h"

MenuState current_menu_state = MAIN_MENU;

int menu1()
{
    BusInfo_Display();
    
    s KEY;
    KEY = Key_GetNum();
    if (KEY == setting)
    {
        current_menu_state = SETTING_MENU;
        return 1;
    }
    return 0;
}

void menu2_setting(void)
{
    uint8_t flag = 1;
    
    while (current_menu_state == SETTING_MENU)
    {
        Display_Setting_Menu(flag);
        
        s KEY;
        KEY = Key_GetNum();
        
        if (KEY == up)
        {
            flag--;
            if (flag == 0) {flag = 4;}
        }
        else if (KEY == down)
        {
            flag++;
            if (flag == 5) {flag = 1;}
        }
        else if (KEY == setting)
        {
            switch(flag)
            {
                case 1:
                    current_menu_state = TIME_SETTING;
                    TimeSetting_Menu();
                    break;
                case 2:
                    current_menu_state = BUS_SETTING;
                    BusSetting_Menu();
                    break;
                case 3:
                    current_menu_state = LIGHT_SETTING;
                    LightSetting_Menu();
                    break;
                case 4:
                    break;
            }
        }
        else if (KEY == exti)
        {
            current_menu_state = MAIN_MENU;
        }
    }
}
