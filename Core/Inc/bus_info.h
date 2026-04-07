#ifndef __BUS_INFO_H
#define __BUS_INFO_H

#include "main.h"

// 公交车信息结构体（只包含必须字段）
typedef struct {
    uint8_t bus_id;        // 公交车线路号
    uint16_t bus_time;   // 剩余时间（秒）
} BusInfo;

// 公交车到站显示函数声明
void BusInfo_Init(void);
void BusInfo_Update(void);
void BusInfo_Display(void);

// 公交车线路调整函数声明
void BusInfo_GetLines(uint8_t *lines);
void BusInfo_GetIntervals(uint16_t *intervals);
void BusInfo_SetLine(uint8_t index, uint8_t line);
void BusInfo_SetInterval(uint8_t index, uint16_t interval);

// 公交车信息数组声明
extern BusInfo bus[3];



#endif
