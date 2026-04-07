#include "bluetooth.h"
#include "bus_info.h"
#include <string.h>
#include <stdio.h>

// 蓝牙接收缓冲区
static uint8_t bt_rx_buffer[BT_RX_BUFFER_SIZE];
static uint8_t bt_rx_index = 0;
volatile uint8_t bt_rx_complete = 0;

// 记录上次通知的时间，防止重复发送
static uint16_t last_notify_time[3] = {0};

// 公交信息数组通过bus_info.h声明

// 蓝牙初始化
void BT_Init(void)
{
    bt_rx_index = 0;
    bt_rx_complete = 0;
    
    // 启动串口接收中断
    HAL_UART_Receive_IT(&huart1, &bt_rx_buffer[bt_rx_index], 1);
}

// 串口接收回调处理
void BT_UART_RxCallback(void)
{
    // 检测结束符（回车或换行）
    if (bt_rx_buffer[bt_rx_index] == '\r' || bt_rx_buffer[bt_rx_index] == '\n')
    {
        bt_rx_buffer[bt_rx_index] = '\0';  // 字符串结束符
        if (bt_rx_index > 0)  // 确保有有效数据
        {
            bt_rx_complete = 1;  // 标记接收完成
        }
        bt_rx_index = 0;     // 重置索引
    }
    else
    {
        bt_rx_index++;
        if (bt_rx_index >= BT_RX_BUFFER_SIZE - 1)
        {
            bt_rx_index = 0;  // 防止溢出
        }
    }
    
    // 继续接收
    HAL_UART_Receive_IT(&huart1, &bt_rx_buffer[bt_rx_index], 1);
}

// 处理蓝牙查询
void BT_Process(void)
{
    if (bt_rx_complete)
    {
        bt_rx_complete = 0;
        
        // 无论收到什么指令，都返回所有公交信息
        BT_Send_AllBusInfo();
    }
}

// 发送所有公交信息到蓝牙
void BT_Send_AllBusInfo(void)
{
    char buffer[BT_TX_BUFFER_SIZE];
    
    // 构建响应字符串
    sprintf(buffer, "=== 智能公交站台 ===\r\n");
    
    for (int i = 0; i < 3; i++)
    {
        uint8_t min = bus[i].bus_time / 60;
        uint8_t sec = bus[i].bus_time % 60;
        
        char line[64];
        sprintf(line, "%d路: %02d分%02d秒\r\n", bus[i].bus_id, min, sec);
        strcat(buffer, line);
    }
    
    strcat(buffer, "==================\r\n");
    
    // 发送数据
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 200);
}

// 检查并发送到站通知
void BT_Check_And_Notify(void)
{
    for (int i = 0; i < 3; i++)
    {
        // 即将到站（3分钟内）且未通知过
        if (bus[i].bus_time <= 180 && bus[i].bus_time != last_notify_time[i])
        {
            last_notify_time[i] = bus[i].bus_time;
            
            char buffer[64];
            sprintf(buffer, "【提醒】%d路公交即将到站，剩余%d分%d秒\r\n",
                    bus[i].bus_id, 
                    bus[i].bus_time / 60,
                    bus[i].bus_time % 60);
            
            HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 100);
        }
        else if (bus[i].bus_time > 180)
        {
            // 重置通知记录
            last_notify_time[i] = 0;
        }
    }
}
