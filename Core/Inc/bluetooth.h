#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "main.h"
#include "usart.h"

// 蓝牙接收缓冲区大小
#define BT_RX_BUFFER_SIZE 32
#define BT_TX_BUFFER_SIZE 256

// 蓝牙接收完成标志
extern volatile uint8_t bt_rx_complete;

// 蓝牙初始化
void BT_Init(void);

// 蓝牙处理函数（在主循环中调用）
void BT_Process(void);

// 发送所有公交信息到蓝牙
void BT_Send_AllBusInfo(void);

// 检查并发送到站通知
void BT_Check_And_Notify(void);

// 串口接收回调
void BT_UART_RxCallback(void);

#endif
