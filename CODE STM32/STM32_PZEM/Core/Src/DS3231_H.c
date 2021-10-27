
#include "DS3231_H.h"
extern I2C_HandleTypeDef hi2c1;  //i2c
DS3231_t DS3231;
uint8_t BCD_DEC(uint8_t c)
{
  return (c>>4)*10 +(c&0x0f);
}
uint8_t DEC_BCD(uint8_t c)
{
  return (c/10)<<4|(c%10);
}
void GetTime(void)
{
  //HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, DS3231_REG_TIME, 1, DS3231.I2C_Buffer, 7, 1000); 
  HAL_I2C_Mem_Read_IT(&hi2c1, DS3231_ADDRESS, DS3231_REG_TIME, 1, DS3231.I2C_Buffer, 7);
  DS3231.day    = BCD_DEC(DS3231.I2C_Buffer[3]);
  DS3231.date   = BCD_DEC(DS3231.I2C_Buffer[4]);
  DS3231.month  = BCD_DEC(DS3231.I2C_Buffer[5]);
  DS3231.year   = BCD_DEC(DS3231.I2C_Buffer[6]);
  DS3231.hour   = BCD_DEC(DS3231.I2C_Buffer[2]);
  DS3231.min    = BCD_DEC(DS3231.I2C_Buffer[1]);
  DS3231.sec    = BCD_DEC(DS3231.I2C_Buffer[0]);
}
void SetTime(uint8_t hour,uint8_t min,uint8_t sec,uint8_t date,uint8_t month,uint8_t year,uint8_t day)
{
  DS3231.I2C_Buffer[0] = DEC_BCD(sec);
  DS3231.I2C_Buffer[1] = DEC_BCD(min);
  DS3231.I2C_Buffer[2] = DEC_BCD(hour);
  DS3231.I2C_Buffer[3] = DEC_BCD(day);
  DS3231.I2C_Buffer[4] = DEC_BCD(date);
  DS3231.I2C_Buffer[5] = DEC_BCD(month);
  DS3231.I2C_Buffer[6] = DEC_BCD(year);
  //HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, DS3231_REG_TIME, 1, DS3231.I2C_Buffer, 7, 1000);
  HAL_I2C_Mem_Write_IT(&hi2c1, DS3231_ADDRESS, DS3231_REG_TIME, 1, DS3231.I2C_Buffer, 7);
}
float Get_Temp (void)
{
  uint8_t temp[2];
  //HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, DS3231_REG_TEMPERATURE, 1, temp, 2, 1000);
  HAL_I2C_Mem_Read_IT(&hi2c1, DS3231_ADDRESS, DS3231_REG_TEMPERATURE, 1, temp, 2);
  return ((temp[0])+(temp[1]>>6)/4.0);
}
void force_temp_conv (void)
{
  uint8_t status=0;
  uint8_t control=0;
  //HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, DS3231_REG_STATUS, 1, &status, 1, 100);  // read status register
  HAL_I2C_Mem_Read_IT(&hi2c1, DS3231_ADDRESS, DS3231_REG_STATUS, 1, &status, 1);
  if (!(status&0x04))  // if the BSY bit is not set
  {
    //HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, DS3231_REG_CONTROL, 1, &control, 1, 100);  // read control register
    //HAL_I2C_Mem_Write(&hi2c1,DS3231_ADDRESS, DS3231_REG_CONTROL, 1, (uint8_t *)(control|(0x20)), 1, 100);  // write modified control register with CONV bit as'1'
    HAL_I2C_Mem_Read_IT(&hi2c1, DS3231_ADDRESS, DS3231_REG_CONTROL, 1, &control, 1);
    HAL_I2C_Mem_Read_IT(&hi2c1, DS3231_ADDRESS, DS3231_REG_CONTROL, 1, (uint8_t *)(control|(0x20)), 1);
  }
}