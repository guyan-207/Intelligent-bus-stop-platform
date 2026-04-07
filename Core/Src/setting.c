#include "OLED.h"
#include "bus_info.h"
#include "key.h"
#include "setting.h"
#include "ds1302.h"
#include "menu.h"
#include "adc.h"
#include "tim.h"

// 显示设置菜单
void Display_Setting_Menu(uint8_t selected_option)
{
    if(selected_option > 3)
    {
        OLED_NewFrame();
        OLED_PrintString(10, 0, "求助按钮", &font16x16, (selected_option == 4) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
        OLED_ShowFrame();
    }
    else
    {
        OLED_NewFrame();
        
        OLED_PrintString(30, 0, "设置菜单", &font16x16, OLED_COLOR_NORMAL);
        
        OLED_PrintString(10, 16 , "时间调整", &font16x16, 
                                        (selected_option == 1) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
        OLED_PrintString(10, 32, "公交线路调整", &font16x16, 
                                        (selected_option == 2) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
        OLED_PrintString(10, 48, "照明开关", &font16x16, 
                                        (selected_option == 3) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
        
        OLED_ShowFrame();
    }
}

// 显示时间调整子菜单
void Display_TimeSetting_Menu(TimeEditIndex index, DS1302_Time_t *time)
{
    char buffer[32];
    
    OLED_NewFrame();
    
    // 显示标题
    OLED_PrintString(30, 0, "时间调整", &font16x16, OLED_COLOR_NORMAL);
    /*如果当前下标处在xx则反显*/
    // 显示年
    sprintf(buffer, "20%02d", time->year);
    OLED_PrintASCIIString(0, 20, buffer, &afont12x6, 
                         (index == TIME_YEAR) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    // 显示月
    sprintf(buffer, "-%02d", time->month);
    OLED_PrintASCIIString(24, 20, buffer, &afont12x6, 
                         (index == TIME_MONTH) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    // 显示日
    sprintf(buffer, "-%02d", time->date);
    OLED_PrintASCIIString(42, 20, buffer, &afont12x6, 
                         (index == TIME_DATE) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    // 显示时
    sprintf(buffer, "%02d", time->hour);
    OLED_PrintASCIIString(72, 20, buffer, &afont12x6, 
                         (index == TIME_HOUR) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    // 显示分
    sprintf(buffer, "-%02d", time->minute);
    OLED_PrintASCIIString(84, 20, buffer, &afont12x6, 
                         (index == TIME_MINUTE) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    // 显示秒
    sprintf(buffer, "-%02d", time->second);
    OLED_PrintASCIIString(102, 20, buffer, &afont12x6, 
                         (index == TIME_SECOND) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    // 显示操作提示
    OLED_PrintString(0, 40, " u d修改  s移动", &font12x12, OLED_COLOR_NORMAL);
	  OLED_PrintString(0, 52, "  e保存", &font12x12, OLED_COLOR_NORMAL);
    
    OLED_ShowFrame();
}

// 时间调整子菜单函数
void TimeSetting_Menu(void)
{
    TimeEditIndex index = TIME_YEAR;
    DS1302_Time_t temp_time;
    
    // 读取当前时间到临时变量
    DS1302_ReadTime(&temp_time);
    
    while (1)
    {
        // 显示时间调整界面
        Display_TimeSetting_Menu(index, &temp_time);
        
        // 检测按键
        s KEY;
        KEY = Key_GetNum();
        
        switch(KEY)
        {
            case up:  // 增加当前字段值
                switch(index)
                {
                    case TIME_YEAR:
                        temp_time.year++;
                        if (temp_time.year > 99) temp_time.year = 0;
                        break;
                    case TIME_MONTH:
                        temp_time.month++;
                        if (temp_time.month > 12) temp_time.month = 1;
                        break;
                    case TIME_DATE:
                        temp_time.date++;
                        if (temp_time.date > 31) temp_time.date = 1;
                        break;
                    case TIME_HOUR:
                        temp_time.hour++;
                        if (temp_time.hour > 23) temp_time.hour = 0;
                        break;
                    case TIME_MINUTE:
                        temp_time.minute++;
                        if (temp_time.minute > 59) temp_time.minute = 0;
                        break;
                    case TIME_SECOND:
                        temp_time.second++;
                        if (temp_time.second > 59) temp_time.second = 0;
                        break;
                    default:
                        break;
                }
                break;
                
            case down:  // 减少当前字段值
                switch(index)
                {
                    case TIME_YEAR:
                        temp_time.year--;
                        if (temp_time.year < 0) temp_time.year = 99;
                        break;
                    case TIME_MONTH:
                        temp_time.month--;
                        if (temp_time.month < 1) temp_time.month = 12;
                        break;
                    case TIME_DATE:
                        temp_time.date--;
                        if (temp_time.date < 1) temp_time.date = 31;
                        break;
                    case TIME_HOUR:
                        temp_time.hour--;
                        if (temp_time.hour < 0) temp_time.hour = 23;
                        break;
                    case TIME_MINUTE:
                        temp_time.minute--;
                        if (temp_time.minute < 0) temp_time.minute = 59;
                        break;
                    case TIME_SECOND:
                        temp_time.second--;
                        if (temp_time.second < 0) temp_time.second = 59;
                        break;
                    default:
                        break;
                }
                break;
                
            case setting:  // 切换到下一个字段
            {
                index++;
                if (index > TIME_SECOND)
                {
                    // 完成修改，保存时间
                    uint8_t buf[8];
                    DS1302_WriteTime(buf);
                    return;
                }
                break;
            }
                
            case exti:  // 保存并返回
            {
                uint8_t buf[8];
                buf[1] = temp_time.year;
                buf[2] = temp_time.month;
                buf[3] = temp_time.date;
                buf[4] = temp_time.hour;
                buf[5] = temp_time.minute;
                buf[6] = temp_time.second;
                buf[7] = temp_time.week;
                DS1302_WriteTime(buf);
                current_menu_state = SETTING_MENU;
                return;
            }
        }
    }
}

// 显示公交线路调整子菜单
void Display_BusSetting_Menu(BusEditIndex index, uint8_t *lines, uint16_t *intervals)
{
    char buffer[32];
    
    OLED_NewFrame();
    
    // 显示标题
    OLED_PrintString(20, 0, "公交线路调整", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(0,24,"线路",&font16x16,OLED_COLOR_NORMAL);
		OLED_PrintString(60,24,"间隔",&font16x16,OLED_COLOR_NORMAL);
    // 显示第一条公交信息
		
    sprintf(buffer, "%02d", lines[0]);
    OLED_PrintASCIIString(40, 20, buffer, &afont12x6, 
                         (index == BUS_LINE_1) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    sprintf(buffer, "%d", intervals[0]);
    OLED_PrintASCIIString(96, 20, buffer, &afont12x6, 
                         (index == BUS_INTERVAL_1) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    // 显示第二条公交信息
    sprintf(buffer, "%02d", lines[1]);
    OLED_PrintASCIIString(40, 35, buffer, &afont12x6, 
                         (index == BUS_LINE_2) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    sprintf(buffer, "%d", intervals[1]);
    OLED_PrintASCIIString(96, 35, buffer, &afont12x6, 
                         (index == BUS_INTERVAL_2) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    // 显示第三条公交信息
    sprintf(buffer, "%02d", lines[2]);
    OLED_PrintASCIIString(40, 50, buffer, &afont12x6, 
                         (index == BUS_LINE_3) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    sprintf(buffer, "%d", intervals[2]);
    OLED_PrintASCIIString(96, 50, buffer, &afont12x6, 
                         (index == BUS_INTERVAL_3) ? OLED_COLOR_REVERSED : OLED_COLOR_NORMAL);
    
    // 显示操作提示
//    OLED_PrintString(0, 65, "↑↓修改  →确认  退出保存", &font12x12, OLED_COLOR_NORMAL);
    
    OLED_ShowFrame();
}

// 公交线路调整子菜单函数
void BusSetting_Menu(void)
{
    BusEditIndex index = BUS_LINE_1;
    uint8_t temp_lines[3];
    uint16_t temp_intervals[3];
    
    // 读取当前公交线路和间隔时间到临时变量
    BusInfo_GetLines(temp_lines);
    BusInfo_GetIntervals(temp_intervals);
    
    while (1)
    {
        // 显示公交线路调整界面
        Display_BusSetting_Menu(index, temp_lines, temp_intervals);
        
        // 检测按键
        s KEY;
        KEY = Key_GetNum();
        
        switch(KEY)
        {
            case up:  // 增加当前字段值
                switch(index)
									{/*上先是100路*/
                    case BUS_LINE_1:
                        temp_lines[0]++;
                        if (temp_lines[0] > 99) temp_lines[0] = 1;
                        break;
                    case BUS_LINE_2:
                        temp_lines[1]++;
                        if (temp_lines[1] > 99) temp_lines[1] = 1;
                        break;
                    case BUS_LINE_3:
                        temp_lines[2]++;
                        if (temp_lines[2] > 99) temp_lines[2] = 1;
                        break;
                    case BUS_INTERVAL_1:
                        temp_intervals[0] += 10;
                        if (temp_intervals[0] > 3600) temp_intervals[0] = 60;
                        break;
                    case BUS_INTERVAL_2:
                        temp_intervals[1] += 10;
                        if (temp_intervals[1] > 3600) temp_intervals[1] = 60;
                        break;
                    case BUS_INTERVAL_3:
                        temp_intervals[2] += 10;
                        if (temp_intervals[2] > 3600) temp_intervals[2] = 60;
                        break;
                    default:
                        break;
                }
                break;
                
            case down:  // 减少当前字段值
                switch(index)
                {
                    case BUS_LINE_1:
                        temp_lines[0]--;
                        if (temp_lines[0] < 1) temp_lines[0] = 99;
                        break;
                    case BUS_LINE_2:
                        temp_lines[1]--;
                        if (temp_lines[1] < 1) temp_lines[1] = 99;
                        break;
                    case BUS_LINE_3:
                        temp_lines[2]--;
                        if (temp_lines[2] < 1) temp_lines[2] = 99;
                        break;
                    case BUS_INTERVAL_1:
                        temp_intervals[0] -= 10;
                        if (temp_intervals[0] < 60) temp_intervals[0] = 3600;
                        break;
                    case BUS_INTERVAL_2:
                        temp_intervals[1] -= 10;
                        if (temp_intervals[1] < 60) temp_intervals[1] = 3600;
                        break;
                    case BUS_INTERVAL_3:
                        temp_intervals[2] -= 10;
                        if (temp_intervals[2] < 60) temp_intervals[2] = 3600;
                        break;
                    default:
                        break;
                }
                break;
                
            case setting:  // 切换到下一个字段
            {
                index++;
                if (index > BUS_INTERVAL_3)
                {
                    // 完成修改，保存公交线路和间隔时间
                    for(int i = 0; i < 3; i++)
                    {
                        BusInfo_SetLine(i, temp_lines[i]);
                        BusInfo_SetInterval(i, temp_intervals[i]);
                    }
                    return;
                }
                break;
            }
                
            case exti:  // 保存并返回
            {
                // 保存公交线路和间隔时间
                for(int i = 0; i < 3; i++)
                {
                    BusInfo_SetLine(i, temp_lines[i]);
                    BusInfo_SetInterval(i, temp_intervals[i]);
                }
                current_menu_state = SETTING_MENU;
                return;
            }
        }
    }
}

// 全局变量：照明状态
LightStatus light_status = LIGHT_OFF;

// 全局变量：光照强度
uint16_t light_intensity = 0;

// 显示照明开关子菜单
void Display_LightSetting_Menu(LightStatus status, uint16_t light_intensity)
{
    char buffer[32];
    
    OLED_NewFrame();
    
    // 显示标题
    OLED_PrintString(30, 0, "照明开关", &font12x12, OLED_COLOR_NORMAL);
    
    // 显示照明状态
    OLED_PrintString(10, 20, "状态:", &font12x12, OLED_COLOR_NORMAL);
    if (status == LIGHT_ON)
    {
        OLED_PrintString(60, 20, "开", &font12x12, OLED_COLOR_REVERSED);
    }
    else
    {
        OLED_PrintString(60, 20, "关", &font12x12, OLED_COLOR_REVERSED);
    }
    
    // 显示光照强度
    sprintf(buffer, "光照强度:%d", light_intensity);
    OLED_PrintString(10, 40, buffer, &font16x16, OLED_COLOR_NORMAL);
    
    // 显示操作提示
//    OLED_PrintString(0, 60, "↑↓切换  确认/退出", &font12x12, OLED_COLOR_NORMAL);
    
    OLED_ShowFrame();
}

// 初始化光照强度
void Init_LightIntensity(void)
{
    Get_ADC_sample();
    light_intensity = ADC_Sample;
}

// 更新光照强度（可在中断中调用）
void Update_LightIntensity(void)
{
    Get_ADC_sample();
    // 确保使用最新的ADC_Sample值
    extern uint16_t ADC_Sample;
    light_intensity = ADC_Sample;
}

// 更新灯的亮度
void Update_LightBrightness(void)
{
    if (light_status == LIGHT_ON)
    {
        // 当灯打开时，亮度随周围光的强度降低而变亮
        // 光照强度范围：0-4095，值越小表示光照越弱
        uint16_t brightness = 65534 - (light_intensity * 65534 / 4095);
        
        // 使用TIM3的PWM来控制LED的亮度
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, brightness);
    }
    else
    {
        // 关闭灯
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    }
}

// 照明开关子菜单函数
void LightSetting_Menu(void)
{
    // 确保光照强度有初始值
    if (light_intensity == 0)
    {
        Init_LightIntensity();
    }
    
    // 用于控制更新频率的计数器
    static uint32_t update_counter = 0;
    
    while (1)
    {
        // 修正光照强度显示，使其与实际光照强度一致
        uint16_t display_intensity = 4095 - light_intensity;
        
        // 显示照明开关界面
        Display_LightSetting_Menu(light_status, display_intensity);
        
        // 检测按键
        s KEY;
        KEY = Key_GetNum();
        
        switch(KEY)
        {
            case up:  // 切换到开状态
            case down:  // 切换到关状态
                light_status = (light_status == LIGHT_ON) ? LIGHT_OFF : LIGHT_ON;
                
                // 当照明状态变化时，立即更新PWM值
//                Update_LightBrightness();
                break;
                
            case setting:  // 确认并返回
            case exti:  // 退出
            {
                // 保存照明状态并返回
                current_menu_state = SETTING_MENU;
                return;
            }
        }
        
        // 每10秒更新一次灯的亮度
        if (update_counter >= 10000)  // 假设1ms调用一次，10000次就是10秒
        {
            update_counter = 0;
            Update_LightBrightness();
        }
        else
        {
            update_counter++;
        }
        
        // 延时1ms，控制更新频率
        HAL_Delay(1);
    }
}