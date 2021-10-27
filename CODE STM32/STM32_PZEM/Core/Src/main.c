/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "ILI9341_Touchscreen.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "5x5_font.h"
#include "DS3231_H.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
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
#define BTNR        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_5)
#define BTNG        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6)
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t btn[2];
 uint8_t control=0;
extern uint8_t touch;
extern bool status;
extern uint8_t t;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void delay_us(uint16_t au16_us)
{
    htim6.Instance->CNT = 0;
    while (htim6.Instance->CNT < au16_us);
}

void delay_ms(uint16_t au16_ms)
{
    while(au16_ms > 0)
    {
	htim6.Instance->CNT = 0;
	au16_ms--;
	while (htim6.Instance->CNT < 1000);
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim7);
 // HAL_UART_Receive_DMA(&huart2,(uint8_t *)ESP32_recei,7);
  ILI9341_Init(); 
  ILI9341_Fill_Screen(BLACK);
  ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
  ILI9341_Draw_Text("SYSTEM ENERGY", 15, 100, BLUE, 3, BLACK);
  ILI9341_Draw_Text("KTMT-K18", 30, 140, GREEN, 3, BLACK);
  ILI9341_Draw_Text("Please wait..........", 15, 200, CYAN, 2, BLACK);
  HAL_Delay(1000);
  ILI9341_Fill_Screen(BLACK);
  touch=1;
  control=1;
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
 
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if(htim->Instance ==TIM7)
  {
    btn[0]=BTNR;
    btn[1]=BTNG;
    if(btn[1]==1)
    {
      delay_ms(200);
      do 
      {
        btn[1]=BTNG;
      }
      while(btn[1]==1);
      delay_ms(200);
      t=0;
    }
    if(btn[0]==1)
    {
      delay_ms(200);
      do 
      {
        btn[0]=BTNR;
      }
      while(btn[0]==1);
      delay_ms(200);
      status=false;
    }
  }
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
