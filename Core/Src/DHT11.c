#include "DHT11.h"
#include "main.h"
#include "dwt_delay.h"  // 使用DWT_Delay替代定时器延时
// 定义DHT11数据引脚
#define DHT11_PORT GPIOB
#define DHT11_PIN GPIO_PIN_15

// 宏定义操作
#define DHT11_SET() HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET)
#define DHT11_RESET() HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET)
#define DHT11_READ() HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)

// 超时定义
#define DHT11_TIMEOUT 1000  // 最大等待时间（微秒）

// 切换引脚模式
static void DHT11_SetOutputMode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void DHT11_SetInputMode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

// DHT11初始化
void DHT11_Init(void)
{
    DHT11_SetOutputMode();
    DHT11_SET();
}

// 等待指定电平，带超时
static uint8_t DHT11_WaitForLevel(GPIO_PinState level, uint32_t timeout_us)
{
    uint32_t timeout = timeout_us;
    while (DHT11_READ() == level)
    {
        delayUS_DWT(1);
        if (--timeout == 0)
        {
            return 1;  // 超时
        }
    }
    return 0;  // 成功
}

// 读取DHT11数据
uint8_t DHT11_ReadData(DHT11_Data_TypeDef *Data)
{
    uint8_t i, j;
    uint8_t buf[5] = {0};
    
    // 1. 主机发送开始信号
    DHT11_SetOutputMode();
    DHT11_RESET();
    delayUS_DWT(20000);  // 至少18ms，确保DHT11能检测到
    DHT11_SET();
    delayUS_DWT(30);  // 拉高30us等待DHT11响应
    
    // 2. 切换到输入模式，等待DHT11响应
    DHT11_SetInputMode();
    
    // 3. 检查DHT11响应信号
    // 等待低电平（DHT11拉低总线作为响应）
    if (DHT11_WaitForLevel(GPIO_PIN_SET, DHT11_TIMEOUT))
    {
        DHT11_SetOutputMode();
        DHT11_SET();
        return 1;  // 无响应超时
    }
    
    // 等待高电平（80us低电平后的80us高电平）
    if (DHT11_WaitForLevel(GPIO_PIN_RESET, DHT11_TIMEOUT))
    {
        DHT11_SetOutputMode();
        DHT11_SET();
        return 2;  // 响应错误
    }
    
    // 等待高电平结束，准备数据
    if (DHT11_WaitForLevel(GPIO_PIN_SET, DHT11_TIMEOUT))
    {
        DHT11_SetOutputMode();
        DHT11_SET();
        return 3;  // 准备数据超时
    }
    
    // 4. 读取40位数据
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 8; j++)
        {
            // 等待低电平结束（每一位数据开始）
            if (DHT11_WaitForLevel(GPIO_PIN_RESET, DHT11_TIMEOUT))
            {
                DHT11_SetOutputMode();
                DHT11_SET();
                return 4;  // 读取数据超时
            }
            
            // 延时40us，判断数据位
            delayUS_DWT(40);
            
            // 读取数据位
            if (DHT11_READ() == GPIO_PIN_SET)
            {
                buf[i] = (buf[i] << 1) | 0x01;
                // 等待高电平结束
                if (DHT11_WaitForLevel(GPIO_PIN_SET, DHT11_TIMEOUT))
                {
                    DHT11_SetOutputMode();
                    DHT11_SET();
                    return 4;  // 读取数据超时
                }
            }
            else
            {
                buf[i] = (buf[i] << 1) | 0x00;
            }
        }
    }
    
    // 5. 返回输出模式
    DHT11_SetOutputMode();
    DHT11_SET();
    
    // 6. 校验数据
    if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
    {
        Data->humidity = buf[0];
        Data->temperature = buf[2];
        return 0;  // 读取成功
    }
    
    return 5;  // 校验错误
}

// 简便的读取函数，直接返回温度和湿度
uint8_t DHT11_Read(uint8_t *temp, uint8_t *humi)
{
    DHT11_Data_TypeDef data;
    uint8_t ret = DHT11_ReadData(&data);
    
    if (ret == 0)
    {
        *temp = data.temperature;
        *humi = data.humidity;
    }
    
    return ret;
}