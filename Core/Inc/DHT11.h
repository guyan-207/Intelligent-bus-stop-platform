#ifndef __DHT11_H
#define __DHT11_H

#include "main.h"

// DHT11数据结构体
typedef struct {
    uint8_t humidity;     // 湿度值（0-99%）
    uint8_t temperature;  // 温度值（0-99℃）
} DHT11_Data_TypeDef;

// 函数声明
void DHT11_Init(void);
uint8_t DHT11_ReadData(DHT11_Data_TypeDef *Data);

#endif /* __DHT11_H */
