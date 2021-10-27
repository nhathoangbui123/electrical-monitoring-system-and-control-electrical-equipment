/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//control device
#define D1_on    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET)
#define D1_off   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET)
#define D2_on    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET)
#define D2_off   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET)
#define D3_on    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET)
#define D3_off   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET)
#define DALL_on  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET)
#define DALL_off HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET)
//control led and speaker
#define LEDG_on    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET)
#define LEDG_off   HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET)
#define LEDY_on    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_RESET)
#define LEDY_off   HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_SET)
#define LEDR_on    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET)
#define LEDR_off   HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET)
#define SPK_on     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_RESET)
#define SPK_off    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_SET)
#define KICH_on     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET)
#define KICH_off    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET)
//read button
#define BTNR        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6)
#define BTNG        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_5)
//library
#include "math.h"
#include "ILI9341_Touchscreen.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "5x5_font.h"
#include "DS3231_H.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//PZEM
uint8_t pzem_cmd[8] = {0xf8, 0x04, 0x00, 0x00, 0x00, 0x0A, 0x64, 0x64};//pzem cmd
//uint8_t pzem_cmd[4] = {0xf8, 0x42,0x41, 0xC2};//
//uint8_t pzem_cmd[6] = {0xF8,0x41,0x37,0x21,0x78,0xB7};
uint8_t data[25];//data pzem
uint16_t int_volt,dec_volt,int_freq,dec_freq,dec_current,int_current,dec_power,int_power,dec_energy,int_energy;//data display
//button
uint8_t btn[2];
extern uint8_t control;
//uart ESP32
uint8_t ESP32_send[17],ESP32_recei[10];
//control tft
uint16_t x_pos,y_pos;
uint8_t touch=0,d0=0,d1=0,d2=0,d3=0,k;
uint8_t d[8];
int cnt=0;
uint32_t threshold=0;
char Buffer[40];
uint8_t t=0;
bool on1=false,on2=false,on3=false,on4=false,status,event_touch;
//control task
int task1,task2,task3,task4,task5,task6;
extern DS3231_t DS3231;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
typedef struct {
  uint16_t volt;
  uint32_t current;
  uint16_t freq;
  uint32_t energy;
  uint32_t power;
}PZEM_t;
PZEM_t PZEM; 
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_TFT */
osThreadId_t Task_TFTHandle;
const osThreadAttr_t Task_TFT_attributes = {
  .name = "Task_TFT",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 256 * 4
};
/* Definitions for Task_TOUCH */
osThreadId_t Task_TOUCHHandle;
const osThreadAttr_t Task_TOUCH_attributes = {
  .name = "Task_TOUCH",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 128 * 4
};
/* Definitions for Task_PZEM */
osThreadId_t Task_PZEMHandle;
const osThreadAttr_t Task_PZEM_attributes = {
  .name = "Task_PZEM",
  .priority = (osPriority_t) osPriorityAboveNormal7,
  .stack_size = 128 * 4
};
/* Definitions for Task_ESP32 */
osThreadId_t Task_ESP32Handle;
const osThreadAttr_t Task_ESP32_attributes = {
  .name = "Task_ESP32",
  .priority = (osPriority_t) osPriorityAboveNormal7,
  .stack_size = 128 * 4
};
/* Definitions for Task_DS3231 */
osThreadId_t Task_DS3231Handle;
const osThreadAttr_t Task_DS3231_attributes = {
  .name = "Task_DS3231",
  .priority = (osPriority_t) osPriorityAboveNormal7,
  .stack_size = 128 * 4
};
/* Definitions for Task_RUN */
osThreadId_t Task_RUNHandle;
const osThreadAttr_t Task_RUN_attributes = {
  .name = "Task_RUN",
  .priority = (osPriority_t) osPriorityAboveNormal7,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Start_Task_TFT(void *argument);
void Start_Task_TOUCH(void *argument);
void Start_Task_PZEM(void *argument);
void Start_Task_ESP32(void *argument);
void Start_Task_DS3231(void *argument);
void Start_Task_RUN(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  LEDR_on;
  LEDG_on;
  LEDY_off;
  SPK_off;
  DALL_off;
  D1_off;
  D2_off;
  D3_off;
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_TFT */
  Task_TFTHandle = osThreadNew(Start_Task_TFT, NULL, &Task_TFT_attributes);

  /* creation of Task_TOUCH */
  Task_TOUCHHandle = osThreadNew(Start_Task_TOUCH, NULL, &Task_TOUCH_attributes);

  /* creation of Task_PZEM */
  Task_PZEMHandle = osThreadNew(Start_Task_PZEM, NULL, &Task_PZEM_attributes);

  /* creation of Task_ESP32 */
  Task_ESP32Handle = osThreadNew(Start_Task_ESP32, NULL, &Task_ESP32_attributes);

  /* creation of Task_DS3231 */
  Task_DS3231Handle = osThreadNew(Start_Task_DS3231, NULL, &Task_DS3231_attributes);

  /* creation of Task_RUN */
  Task_RUNHandle = osThreadNew(Start_Task_RUN, NULL, &Task_RUN_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Start_Task_TFT */
/**
  * @brief  Function implementing the Task_TFT thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Start_Task_TFT */
void Start_Task_TFT(void *argument)
{
  /* USER CODE BEGIN Start_Task_TFT */
  /* Infinite loop */
  for(;;)
  {
    task1++;
    if(touch==0)//page 0
    {
      ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
      ILI9341_Draw_Text("SYSTEM ENERGY", 15, 10, BLUE, 3, BLACK);  
      //volt
      sprintf(Buffer,"VOLT: %0.2d.%dV",int_volt,dec_volt);
      ILI9341_Draw_Text(Buffer, 20, 40, MAGENTA, 3,BLACK);
      //current
      sprintf(Buffer,"CURR: %0.1d.%dA ",int_current,dec_current);
      ILI9341_Draw_Text(Buffer, 20, 60, RED, 3, BLACK);
      //freq
      sprintf(Buffer,"FREQ: %0.2d.%dHz ",int_freq,dec_freq);
      ILI9341_Draw_Text(Buffer, 20, 80, ORANGE, 3, BLACK);
      //energy
      sprintf(Buffer,"ENER: %dWh",PZEM.energy);
      ILI9341_Draw_Text(Buffer, 20, 100, GREEN, 3, BLACK);
      //power
      sprintf(Buffer,"POWER: %0.1d.%dW ",int_power,dec_power);
      ILI9341_Draw_Text(Buffer, 20, 120, CYAN, 3, BLACK);
      //time-date
      sprintf(Buffer, "TIME-%0.2d:%0.2d:%0.2d", DS3231.hour,DS3231.min,DS3231.sec);
      ILI9341_Draw_Text(Buffer, 20, 180, RED, 3, BLACK);
      sprintf(Buffer, "T%d-%02d-%02d-20%02d",DS3231.day,DS3231.date,DS3231.month,DS3231.year);
      ILI9341_Draw_Text(Buffer, 20, 200, PINK, 3, BLACK);
      
      ILI9341_Draw_Filled_TAMGIAC(290,100,50,GREEN);
    }
    else if(touch==1)
    {
      ILI9341_Fill_Screen(BLACK);
      touch=2;
    }
    else if(touch==2)//page 1
    {
      ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
      ILI9341_Draw_Text("SYSTEM ENERGY", 15, 10, BLUE, 3, BLACK);
      ILI9341_Draw_Text("DEVICE CONTROL", 80, 40, GREEN, 2, BLACK);
      //name
      ILI9341_Draw_Text("ALL DEVICE", 40,  80, RED, 3, BLACK);
      ILI9341_Draw_Text("DEVICE1", 40, 120, RED, 3, BLACK);
      ILI9341_Draw_Text("DEVICE2", 40, 160, RED, 3, BLACK);
      ILI9341_Draw_Text("DEVICE3", 40, 200, RED, 3, BLACK);
      if(d0==1)//device all
      {
        ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
        ILI9341_Draw_Text("ON ", 230,  80, YELLOW, 2, BLACK);
        on1=true;
        DALL_on;
      }   
      else if(d0==2)
      {
        ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
        ILI9341_Draw_Text("OFF", 230,  80, YELLOW, 2, BLACK);
        on1=false;
        DALL_off;
      }  
      if(d1==3)//device 1
      {
        ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
        ILI9341_Draw_Text("ON ", 190,  120,YELLOW, 2,BLACK);
        on2=true;
        D1_on;
      }   
      else if(d1==4)
      {
        ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
        ILI9341_Draw_Text("OFF", 190,  120, YELLOW, 2,BLACK);
        on2=false;
        D1_off;
      }   
      if(d2==5)//device 2
      {
        ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
        ILI9341_Draw_Text("ON ", 190,  160, YELLOW, 2,BLACK);
        on2=true;
        D2_on;
      }   
      else if(d2==6)
      {
        ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
        ILI9341_Draw_Text("OFF", 190,  160, YELLOW, 2, BLACK);
        on2=false;
        D2_off;
      }   
      if(d3==5)//device 3
      {
        ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
        ILI9341_Draw_Text("ON ", 190,  200, YELLOW, 2, BLACK);
        on3=true;
        D3_on;
      }   
      else if(d3==6)
      {
        ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
        ILI9341_Draw_Text("OFF", 190,  200, YELLOW, 2, BLACK);
        on3=false;
        D3_off;
      }   
      ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
      ILI9341_Draw_Filled_TAMGIAC(290,100,50,GREEN);
    }
    if(touch==3)
    {
      ILI9341_Fill_Screen(BLACK);
      touch=0;
    }
    //device all on with app
    if(control==1)
    {
      if(d[0]==49)d0=1;
      else if(d[0]==48)d0=2;
      
      if(d[1]==49)d1=3;
      else if(d[1]==48)d1=4;
      
      if(d[2]==49)d2=5;
      else if(d[2]==48)d2=6;
      
      if(d[3]==49)d3=5;
      else if(d[3]==48)d3=6;
    }
    osDelay(1);
    //osThreadSuspend(Task_TFTHandle);
  }
  /* USER CODE END Start_Task_TFT */
}

/* USER CODE BEGIN Header_Start_Task_TOUCH */
/**
* @brief Function implementing the Task_TOUCH thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_Task_TOUCH */
void Start_Task_TOUCH(void *argument)
{
  /* USER CODE BEGIN Start_Task_TOUCH */
  /* Infinite loop */
  for(;;)
  {
    task2++;
//    if(TP_Touchpad_Pressed())
//    {
//      uint16_t position_array[2];
//      if(TP_Read_Coordinates(position_array) == TOUCHPAD_DATA_OK)
//      {
//        x_pos = position_array[0];
//        y_pos = position_array[1];
//        event_touch=true;
//        if(touch==0)//page 0
//        {
//          if(x_pos<250&&x_pos>200&&y_pos<50&&y_pos>20)touch=1;//page 0 sang page 1
//        }
//        else if(touch==2)
//        {
//          if(x_pos<240&&x_pos>200&&y_pos<300&&y_pos>280)touch=3;//page 1 ve page 0
//          if(x_pos<220&&x_pos>180&&y_pos<265&&y_pos>140&&on1==false)d0=1;//device all
//          if(x_pos<220&&x_pos>180&&y_pos<265&&y_pos>140&&on1==true)d0=2;
//          if(x_pos<250&&x_pos>220&&y_pos<265&&y_pos>140&&on2==false)d1=3;//device 1
//          if(x_pos<250&&x_pos>220&&y_pos<265&&y_pos>140&&on2==true)d1=4;
//          if(x_pos<280&&x_pos>250&&y_pos<265&&y_pos>140&&on3==false)d2=5;//device 2
//          if(x_pos<280&&x_pos>250&&y_pos<265&&y_pos>140&&on3==true)d2=6;
//          //if(x_pos<240&&x_pos>200&&y_pos<265&&y_pos>140)touch=3;//device 3
//        }
//      }							
//    }
    osDelay(1);
  }
  /* USER CODE END Start_Task_TOUCH */
}

/* USER CODE BEGIN Header_Start_Task_PZEM */
/**
* @brief Function implementing the Task_PZEM thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_Task_PZEM */
void Start_Task_PZEM(void *argument)
{
  /* USER CODE BEGIN Start_Task_PZEM */
  /* Infinite loop */
  for(;;)
  {
    task3++;
    if(status==true)HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_12);
    
    HAL_UART_Transmit_IT(&huart1,(uint8_t *)pzem_cmd,8);//send_cmd den pzem
    HAL_UART_Receive_DMA(&huart1,(uint8_t *)data,25);//receive data tu pzem
    
    PZEM.volt=(data[3]<<8)+data[4];  //volt
    PZEM.current=(data[7]<<24)+(data[8]<<16)+(data[5]<<8)+(data[6]);//current
    PZEM.freq=(data[17]<<8)+data[18];//freq
    PZEM.energy=(data[15]<<24)+(data[16]<<16)+(data[13]<<8)+(data[14]);//energy
    PZEM.power=(data[11]<<24)+(data[12]<<16)+(data[9]<<8)+(data[10]);//power
    
    dec_volt=PZEM.volt%10;
    int_volt=PZEM.volt/10;
    
    dec_freq=PZEM.freq%10;
    int_freq=PZEM.freq/10;
    
    int_current=PZEM.current/1000;
    dec_current=PZEM.current%1000;
    
    dec_power=PZEM.power%10;
    int_power=PZEM.power/10;
    
    dec_energy=PZEM.energy%1000;
    int_energy=PZEM.energy/1000;
    
    osDelay(1);
    //osThreadSuspend(Task_PZEMHandle); 
  }
  /* USER CODE END Start_Task_PZEM */
}

/* USER CODE BEGIN Header_Start_Task_ESP32 */
/**
* @brief Function implementing the Task_ESP32 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_Task_ESP32 */
void Start_Task_ESP32(void *argument)
{
  /* USER CODE BEGIN Start_Task_ESP32 */
  /* Infinite loop */
  for(;;)
  {
    task4++;
    ESP32_send[0]=0xFF;//checksum
  
    ESP32_send[1]=data[3];
    ESP32_send[2]=data[4];//volt
    
    ESP32_send[3]=data[17];
    ESP32_send[4]=data[18];//freq
    
    ESP32_send[5]=data[7];
    ESP32_send[6]=data[8];//current
    ESP32_send[7]=data[5];
    ESP32_send[8]=data[6];
    
    ESP32_send[9]=data[15];
    ESP32_send[10]=data[16];//energy
    ESP32_send[11]=data[13];
    ESP32_send[12]=data[14];
    
    ESP32_send[13]=data[11];
    ESP32_send[14]=data[12];//power
    ESP32_send[15]=data[9];
    ESP32_send[16]=data[10];
    
    
    HAL_UART_Transmit_DMA(&huart2,(uint8_t *)ESP32_send,17);
    HAL_UART_Receive_DMA(&huart2,(uint8_t *)&ESP32_recei,10);
    
    if(ESP32_recei[0]==70&&ESP32_recei[9]==69)
    {
      d[0]=ESP32_recei[1]; // all device      
      d[1]=ESP32_recei[2]; //device 1
      d[2]=ESP32_recei[3];//device 2
      d[3]=ESP32_recei[4];//device 3
      d[4]=ESP32_recei[5]; //threshold
      d[5]=ESP32_recei[6]; //threshold
      d[6]=ESP32_recei[7]; //threshold
      d[7]=ESP32_recei[8]; //threshold
    }
    threshold=(((d[4]-48)*1000)+((d[5]-48)*100)+((d[6]-48)*10)+(d[7]-48));
    
      if(t==0){
        if(int_power>threshold) //so sanh power voi nguong
        {
          status=true;
          t=1;
        }
      }
      if(t==1){
        if(status==true)
        {
          SPK_on;
          LEDG_off;
        }
        else{
          SPK_off;
          LEDG_on;
        }
      }

    osDelay(1);
    
  }
  /* USER CODE END Start_Task_ESP32 */
}

/* USER CODE BEGIN Header_Start_Task_DS3231 */
/**
* @brief Function implementing the Task_DS3231 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_Task_DS3231 */
void Start_Task_DS3231(void *argument)
{
  /* USER CODE BEGIN Start_Task_DS3231 */
  /* Infinite loop */
  for(;;)
  {
    //task5++;
    GetTime();
    osDelay(10);
  }
  /* USER CODE END Start_Task_DS3231 */
}

/* USER CODE BEGIN Header_Start_Task_RUN */
/**
* @brief Function implementing the Task_RUN thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_Task_RUN */
void Start_Task_RUN(void *argument)
{
  /* USER CODE BEGIN Start_Task_RUN */
  /* Infinite loop */
  for(;;)
  {
    LEDY_on;
    delay_ms(5);
    LEDY_off;

    osDelay(1);
  }
  /* USER CODE END Start_Task_RUN */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
