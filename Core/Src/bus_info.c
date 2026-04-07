#include "stm32f1xx_hal.h"


#include "oled.h"
#include "bus_info.h"
#include "gpio.h"
#include "ds1302.h"
#include "DHT11.h"
#include "adc.h"
#include "setting.h"
#include "bluetooth.h"

#include <string.h>
#include <stdio.h>

// 公交间隔时间数组
static uint16_t bus_intervals[3] = {
    400,  // 第一条公交间隔
    500,  // 第二条公交间隔
    190   // 第三条公交间隔
};

// 公交车信息数组
BusInfo bus[3] = {
    {11, 400},  
    {13, 500},  
    {10, 190}   
};
  DS1302_Time_t time = {0};//时间
	const char *WEEK[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	
	DHT11_Data_TypeDef DHT11_Data;//温度
	
																	
	static uint8_t beep_active = 0;// 蜂鸣器状态
	static uint32_t beep_counter = 0;
	
	static uint8_t count_wendu_guang = 0;/**/
	
	
	
	
// 更新公交车时间
void BusInfo_Update(void)
{
    for(int i = 0; i < 3; i++)
    {
        if(bus[i].bus_time > 0)
        {
            bus[i].bus_time--;
        }
        else
        {
            // 使用bus_intervals数组来重置公交时间
            bus[i].bus_time = bus_intervals[i];
        }
    }
}

// 蜂鸣器控制函数
void BusInfo_BeepControl(void)
{
    
    // 检查是否有公交车即将到站（<3分钟）
    for(int i = 0; i < 3; i++)
    {
        if(bus[i].bus_time <= 180 && bus[i].bus_time > 179)
        {
            // 公交车即将到站，触发蜂鸣器
            beep_active = 1;
            break;
        }
    }
    
    // 控制蜂鸣器
    if(beep_active)
    {
        BEEP_ON();                                            
        // 检查是否还有公交车即将到站
        uint8_t any_arriving = 0;
        for(int i = 0; i < 3; i++)
        {
            if(bus[i].bus_time <= 180 && bus[i].bus_time >=179 )
            {
                any_arriving = 1;
                break;
            }
        }
        
        if(!any_arriving)
        {
            beep_active = 0;
            BEEP_OFF();
        }
    }
    else
    {
        BEEP_OFF();
    }
}

// 获取公交线路号
void BusInfo_GetLines(uint8_t *lines)
{
    for(int i = 0; i < 3; i++)
    {
        lines[i] = bus[i].bus_id;
    }
}

// 获取公交间隔时间
void BusInfo_GetIntervals(uint16_t *intervals)
{
    // 这里返回的是重置时间，即公交间隔时间
    for(int i = 0; i < 3; i++)
    {
        intervals[i] = bus_intervals[i];
    }
}

// 设置公交线路号
void BusInfo_SetLine(uint8_t index, uint8_t line)
{
    if(index < 3)
    {
        bus[index].bus_id = line;
    }
}

// 设置公交间隔时间
void BusInfo_SetInterval(uint8_t index, uint16_t interval)
{
    if(index < 3)
    {
        // 这里修改的是重置时间，即公交间隔时间
        bus_intervals[index] = interval;
    }
}

// 显示公交车信息
void BusInfo_Display(void)
{
    char buffer[32];
    
    
    // 1. 创建新帧
    OLED_NewFrame();
    
    // 2. 显示标题
//    OLED_PrintString(0, 0, "公交到站时间", &font12x12, OLED_COLOR_NORMAL);
    
    // 3. 显示3个公交车信息
    for(int i = 0; i < 3; i++)
    {
        uint8_t min = bus[i].bus_time / 60;
        uint8_t second = bus[i].bus_time % 60;
        
        // 计算显示位置
        uint8_t y_pos = 10 + (i * 12);
        
        // 分段显示公交车信息
        // 显示公交车编号
        sprintf(buffer, "%02d", bus[i].bus_id);
        OLED_PrintASCIIString(0, y_pos, buffer, &afont12x6, OLED_COLOR_NORMAL);
        
        // 显示"路"
        OLED_PrintString(18, y_pos, "路", &font12x12, OLED_COLOR_NORMAL);
        
        // 显示分钟
        sprintf(buffer, "%02d", min);
        OLED_PrintASCIIString(36, y_pos, buffer, &afont12x6, OLED_COLOR_NORMAL);
        
        // 显示"分"
        OLED_PrintString(48, y_pos, "分", &font12x12, OLED_COLOR_NORMAL);
        
        // 显示秒
        sprintf(buffer, "%02d", second);
        OLED_PrintASCIIString(60, y_pos, buffer, &afont12x6, OLED_COLOR_NORMAL);
        
        // 显示"秒"
        OLED_PrintString(72, y_pos, "秒", &font12x12, OLED_COLOR_NORMAL);
        
        // 如果即将到站（<3分钟），反色显示
        if(bus[i].bus_time <= 180)
        {
            // 在当前区域绘制一个反色矩形
            OLED_DrawFilledRectangle(0, y_pos-1, 84, 12, OLED_COLOR_REVERSED);
            
            
            sprintf(buffer, "%02d", bus[i].bus_id);
            OLED_PrintASCIIString(0, y_pos, buffer, &afont12x6, OLED_COLOR_REVERSED);
            
            OLED_PrintString(18, y_pos, "路", &font12x12, OLED_COLOR_REVERSED);
            
            sprintf(buffer, "%02d", min);
            OLED_PrintASCIIString(36, y_pos, buffer, &afont12x6, OLED_COLOR_REVERSED);
            
            OLED_PrintString(48, y_pos, "分", &font12x12, OLED_COLOR_REVERSED);
            
            sprintf(buffer, "%02d", second);
            OLED_PrintASCIIString(60, y_pos, buffer, &afont12x6, OLED_COLOR_REVERSED);
            
            OLED_PrintString(72, y_pos, "秒", &font12x12, OLED_COLOR_REVERSED);
        }
    }
    
    // 4. 显示右侧的时间信息（固定在右侧）
	char temp[100];
	
	sprintf(temp,"20%02d-%02d-%02d",time.year,time.month,time.date);
    OLED_PrintASCIIString(10,0, temp, &afont8x6, OLED_COLOR_NORMAL);
	sprintf(temp,"%02dh%02dm%02ds",time.hour,time.minute,time.second);
	OLED_PrintASCIIString(72,0, temp, &afont8x6, OLED_COLOR_NORMAL);
    
	// 5. 温度光照强度显示
	OLED_PrintString(0,52,"温度",&font12x12,OLED_COLOR_NORMAL);
	sprintf(temp,"%d", DHT11_Data.temperature);
	OLED_PrintASCIIString(24, 52, temp, &afont12x6, OLED_COLOR_NORMAL);
	OLED_PrintString(36,52,"度",&font12x12,OLED_COLOR_NORMAL);
		
	OLED_PrintString(48,52,"湿度",&font12x12,OLED_COLOR_NORMAL);
	sprintf(temp,"%d%%", DHT11_Data.humidity);
	OLED_PrintASCIIString(72, 52, temp, &afont12x6, OLED_COLOR_NORMAL);
	
		// 6. 名称
		OLED_DrawRectangle(92,10,28,39,OLED_COLOR_NORMAL);
		OLED_PrintString(95,12, "智能", &font12x12, OLED_COLOR_REVERSED);
		OLED_PrintString(95,24, "公交", &font12x12, OLED_COLOR_REVERSED);
		OLED_PrintString(95,36, "站台", &font12x12, OLED_COLOR_REVERSED);
    // 7. 刷新显示
	OLED_ShowFrame();
}

// 定时器中断回调 - 控制刷新频率
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint32_t update_counter = 0;
    static uint32_t dht11_counter = 0;
 
    if(htim->Instance == TIM2)
    {
        update_counter++;
        dht11_counter++;
        
        // 控制更新频率（假设TIM2是1ms中断）
        if(update_counter >= 1000)  // 1秒更新一次
        {
            update_counter = 0;
            
            // 更新公交车时间
            BusInfo_Update();
                        
            // 控制蜂鸣器
            BusInfo_BeepControl();
            
            // 读取DS1302时间
            DS1302_ReadTime(&time);
          
        }
        
        // 每10秒读取一次DHT11,光照强度数据
        if(dht11_counter >= 10000)
        {
            dht11_counter = 0;
            DHT11_ReadData(&DHT11_Data);
            
            // 读取新的光照强度
            Get_ADC_sample();
            
            // 更新光照强度变量
            extern uint16_t light_intensity;
            extern uint16_t ADC_Sample;
            
            // 确保light_intensity与ADC_Sample保持同步
            light_intensity = ADC_Sample;
            
            // 检查并发送蓝牙到站通知
            BT_Check_And_Notify();
        }
    }
}