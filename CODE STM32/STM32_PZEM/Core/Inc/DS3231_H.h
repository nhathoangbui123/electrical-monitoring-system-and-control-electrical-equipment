#ifndef  DS3231_H_
#define	 DS3231_H_
#include "main.h"
#include "stm32f4xx_hal.h"

#define DS3231_ADDRESS         (0x68<<1)
#define DS3231_REG_TIME         (0x00)
#define DS3231_REG_ALARM_1      (0x07)
#define DS3231_REG_ALARM_2      (0x0B)
#define DS3231_REG_CONTROL      (0x0E)
#define DS3231_REG_STATUS       (0x0F)
#define DS3231_REG_TEMPERATURE  (0x11)

typedef struct {
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint8_t day;
  uint8_t date;
  uint8_t month;
  uint8_t year;
  uint8_t I2C_Buffer[8];
}DS3231_t;

uint8_t BCD_DEC(uint8_t c);
uint8_t DEC_BCD(uint8_t c);
void GetTime(void);
void SetTime(uint8_t hour,uint8_t min,uint8_t sec,uint8_t date,uint8_t month,uint8_t year,uint8_t day);
float Get_Temp (void);
void force_temp_conv (void);
#endif