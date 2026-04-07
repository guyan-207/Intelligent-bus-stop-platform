#ifndef _SETTING_H
#define _SETTING_H

#include "main.h"
#include "ds1302.h"

typedef enum {
    SETTING_TIME,      // 时间调整
    SETTING_BUS,       // 公交线路调整
    SETTING_LIGHT,     // 照明开关
    SETTING_HELP       // 求助按钮
} SettingOption;

typedef enum {
    TIME_YEAR,   // 年
    TIME_MONTH,  // 月
    TIME_DATE,   // 日
    TIME_HOUR,   // 时
    TIME_MINUTE, // 分
    TIME_SECOND, // 秒
    TIME_DONE    // 完成
} TimeEditIndex;

typedef enum {
    BUS_LINE_1,   // 第一条公交线路
    BUS_LINE_2,   // 第二条公交线路
    BUS_LINE_3,   // 第三条公交线路
    BUS_INTERVAL_1, // 第一条公交间隔
    BUS_INTERVAL_2, // 第二条公交间隔
    BUS_INTERVAL_3, // 第三条公交间隔
    BUS_DONE      // 完成
} BusEditIndex;

typedef enum {
    LIGHT_OFF,    // 照明关闭
    LIGHT_ON      // 照明打开
} LightStatus;

void Display_Setting_Menu(uint8_t selected_option);
void TimeSetting_Menu(void);
void BusSetting_Menu(void);
void LightSetting_Menu(void);

#endif