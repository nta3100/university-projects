/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/***************************** LCD i2c ***************************************/
/* LCD control pins */
#define PIN_RS                 (1 << 0)
#define PIN_EN                 (1 << 2)
#define BACKLIGHT              (1 << 3)
/* I2C Module Address, this is Hardware dependent */
#define SLAVE_ADDRESS_LCD 		0x4E
/*LCD Commands*/
#define LCD_FUNCTION_SET1      	0x33
#define LCD_FUNCTION_SET2      	0x32
#define LCD_4BIT_2LINE_MODE    	0x28
#define LCD_DISP_CURS_ON       	0x0E
#define LCD_DISP_ON_CURS_OFF   	0x0C  //Display on, cursor off
#define LCD_DISPLAY_OFF        	0x08
#define LCD_DISPLAY_ON         	0x04
#define LCD_CLEAR_DISPLAY      	0x01
#define LCD_ENTRY_MODE_SET     	0x04
#define LCD_INCREMENT_CURSER   	0x06
#define LCD_SET_ROW1_COL1      	0x80  //Force cursor to beginning ( 1st line)
#define LCD_SET_ROW2_COL1      	0xC0  //Force cursor to beginning ( 2nd line)
#define LCD_MOVE_DISPLAY_LEFT  	0x18
#define LCD_MOVE_DISPLAY_RIGHT 	0x1C
/***************************** LCD i2c ***************************************/
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/***************************** template ***************************************/
/***************************** template ***************************************/
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/***************************** DHT11 ***************************************/

/***************************** DHT11 ***************************************/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/***************************** LED 7 THANH ***************************************/
uint8_t SEG7_LOOKUP_TABLE[] = {0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110};
uint8_t SEG7_LED_LOOKUP_TABLE[] = {0b00000000, 0b00001000, 0b00000100, 0b00000010, 0b00000001};
uint8_t SEG7_Multiplex = 1;
int SEG7_displayNumber;
// seg_data tuong ung 0bABCDEFGdot
// led_data 0bxxxx1234 tuong tung led 7 thanh so 1, so 2, so 3, so 4
void SEG7_Display(uint8_t seg_data, uint8_t led_data)
{
	HAL_GPIO_WritePin(SEGA_GPIO_Port, SEGA_Pin, seg_data&0b10000000);
	HAL_GPIO_WritePin(SEGB_GPIO_Port, SEGB_Pin, seg_data&0b01000000);
	HAL_GPIO_WritePin(SEGC_GPIO_Port, SEGC_Pin, seg_data&0b00100000);
	HAL_GPIO_WritePin(SEGD_GPIO_Port, SEGD_Pin, seg_data&0b00010000);
	HAL_GPIO_WritePin(SEGE_GPIO_Port, SEGE_Pin, seg_data&0b00001000);
	HAL_GPIO_WritePin(SEGF_GPIO_Port, SEGF_Pin, seg_data&0b00000100);
	HAL_GPIO_WritePin(SEGG_GPIO_Port, SEGG_Pin, seg_data&0b00000010);
	HAL_GPIO_WritePin(SEG_DOT_GPIO_Port, SEG_DOT_Pin, seg_data&0b00000001);

	HAL_GPIO_WritePin(GPIOB, LED1_Pin, !(led_data&0b00001000));
	HAL_GPIO_WritePin(GPIOB, LED2_Pin, !(led_data&0b00000100));
	HAL_GPIO_WritePin(GPIOB, LED3_Pin, !(led_data&0b00000010));
	HAL_GPIO_WritePin(GPIOB, LED4_Pin, !(led_data&0b00000001));
}

void display_number(int number)
{
	char temp;
	switch(SEG7_Multiplex)
	{
	case 1:
		temp = number%10;
		number /= 10;
	case 2:
		temp = number%10;
		number /= 10;
	case 3:
		temp = number%10;
		number /= 10;
	case 4:
		temp = number%10;
	}
	if(SEG7_Multiplex == 3) // truong hop co dau cham
	{
		SEG7_Display(SEG7_LOOKUP_TABLE[temp]|0x01, SEG7_LED_LOOKUP_TABLE[SEG7_Multiplex]);
	}
	else
	{
		SEG7_Display(SEG7_LOOKUP_TABLE[temp], SEG7_LED_LOOKUP_TABLE[SEG7_Multiplex]);
	}
	SEG7_Multiplex++;
	if(SEG7_Multiplex > 4)
	{
		SEG7_Multiplex = 1;
	}
}
/***************************** LED 7 THANH ***************************************/

/***************************** TIMER ***************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)
	{
		char temp;
		int number = SEG7_displayNumber;
			switch(SEG7_Multiplex)
			{
			case 1:
				temp = number%10;
				number /= 10;
			case 2:
				temp = number%10;
				number /= 10;
			case 3:
				temp = number%10;
				number /= 10;
			case 4:
				temp = number%10;
			}
			if(SEG7_Multiplex == 3) // truong hop co dau cham
			{
				SEG7_Display(SEG7_LOOKUP_TABLE[temp]|0x01, SEG7_LED_LOOKUP_TABLE[SEG7_Multiplex]);
			}
			else
			{
				SEG7_Display(SEG7_LOOKUP_TABLE[temp], SEG7_LED_LOOKUP_TABLE[SEG7_Multiplex]);
			}
			SEG7_Multiplex++;
			if(SEG7_Multiplex > 4)
			{
				SEG7_Multiplex = 1;
			}
	}
}
/***************************** TIMER ***************************************/

/***************************** DHT11 ***************************************/
uint8_t DHT11_Temperature = 0;
uint8_t DHT11_Humidity = 0;
uint8_t hByte1, hByte2, tByte1, tByte2, presence;
uint8_t checksum;
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void DHT11_GpioInput(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void DHT11_GpioOutput(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// h??m delay us trong qu?? tr??nh doc d??? li???u t??? dht11
void TIM2_DelayUs(uint16_t us)
{
  __HAL_TIM_SET_COUNTER(&htim2, 0);
  while ((uint16_t)__HAL_TIM_GET_COUNTER(&htim2) < us)
    ;
}

// g???i t??n hi???u b???t ?????u truy????n d??? li???u cho dht11
void DHT11_SendStart(void)
{
  DHT11_GpioOutput();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_Delay(18);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
  TIM2_DelayUs(20);
  DHT11_GpioInput();
}

// h??m ki???m tra ph???n h???i c???a dht11
uint8_t DHT11_CheckRespone(void)
{
  uint8_t respone = 0;
  TIM2_DelayUs(40);
  if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15))
  {
    TIM2_DelayUs(80);
    if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)))
    {
      respone = 1;
    }
    else
    {
      respone = -1;
    }
  }
  while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15))
    ;
  return respone;
}

// ??????c 1 byte d??? li???u t??? dht11
uint8_t DHT11_ReadByte(void)
{
  uint8_t ret = 0, i;
  for (i = 0; i < 8; i++)
  {
    while (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15))
      ;
    TIM2_DelayUs(40);
    if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15))
    {
      ret &= ~(1 << (7 - i));
    }
    else
    {
      ret |= (1 << (7 - i));
    }
    while ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)))
      ;
  }
  return ret;
}

// ??????c d??? li???u t??? dht11 v?? chuy???n ?????i th??nh gi?? tr??? nhi???t ????? v?? ????? ???m
void DHT11_ReadSensor(void)
{
  DHT11_SendStart();
  presence = DHT11_CheckRespone();
  hByte1 = DHT11_ReadByte();
  hByte2 = DHT11_ReadByte();
  tByte1 = DHT11_ReadByte();
  tByte2 = DHT11_ReadByte();
  checksum = DHT11_ReadByte();
  DHT11_Temperature = tByte1;
  DHT11_Humidity = hByte1;
}
/***************************** DHT11 ***************************************/

/***************************** LCD1602 I2C ***************************************/
void LCD_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void LCD_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void LCD_init (void)
{
	// 4 bit init
	HAL_Delay(50);  // wait for >40ms
	LCD_send_cmd (0x30);
	HAL_Delay(5);  // wait for >4.1ms
	LCD_send_cmd (0x30);
	HAL_Delay(1);  // wait for >100us
	LCD_send_cmd (0x30);
	HAL_Delay(10);
	LCD_send_cmd (0x20);  // 4bit mode
	HAL_Delay(10);

  // dislay init
	LCD_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	LCD_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	LCD_send_cmd (0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	LCD_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	LCD_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void LCD_send_string (char *str)
{
	while (*str) LCD_send_data (*str++);
}

void LCD_clear_display (void)
{
	LCD_send_cmd (0x80);
	for (int i=0; i<70; i++)
	{
		LCD_send_data (' ');
	}
}

void LCD_goto_XY (int row, int col)
{
	switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    LCD_send_cmd (col);
}

// Hien thi thong tin nhiet do va do am len LCD
void LCD_Display(void)
{
  char tempString[4];
  LCD_clear_display();
  LCD_goto_XY(0, 0);
  LCD_send_string("Temp: ");
  sprintf(tempString, "%u", DHT11_Temperature);
  LCD_send_string(tempString);
  LCD_send_string(" *C");
  LCD_goto_XY(1,0);
  LCD_send_string("Humid: ");
  sprintf(tempString, "%u", DHT11_Humidity);
  LCD_send_string(tempString);
  LCD_send_string(" %");
}
/***************************** LCD1602 I2C ***************************************/

/***************************** MQ135 ***************************************/
uint16_t MQ135_Raw;
float MQ135_R0 = 176.0;
float MQ135_R2 = 1000.0;
float MQ135_Rs;
float MQ135_ppm = 0;
void MQ135_ReadSensor()
{
	float volts;
	MQ135_Raw = HAL_ADC_GetValue(&hadc1);
//	// chuyen gia tri adc sang dien ap
	volts = (float) MQ135_Raw*3.3;
	volts = volts/4095.0;
//	// tinh gia tri Rs
	MQ135_Rs = (float) MQ135_R2*(1-volts);
	MQ135_Rs = (float) MQ135_Rs/volts;
//	// tinh nong do ppm
	MQ135_ppm = 159.6-133.33*(MQ135_Rs/MQ135_R0);
}
/***************************** MQ135 ***************************************/


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t MSG[35] = {'\0'};
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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start(&htim2);
  HAL_ADC_Start(&hadc1);
//  LCD_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Doc cam bien DHT11
//	  DHT11_ReadSensor();
	  //  Hien thi LCD i2c
//	  LCD_Display();
	  // Doc cam bien MQ135
	  MQ135_ReadSensor();
	  // Hien thi thong tin debug qua uart
	  sprintf(MSG, "Temperature = %d\r\n", DHT11_Temperature);
	  HAL_UART_Transmit(&huart1, MSG, sizeof(MSG), 100);
	  sprintf(MSG, "Humidity = %d\r\n", DHT11_Humidity);
	  HAL_UART_Transmit(&huart1, MSG, sizeof(MSG), 100);
	  sprintf(MSG, "MQ135 = %d\r\n", (int)MQ135_ppm*10);
	  HAL_UART_Transmit(&huart1, MSG, sizeof(MSG), 100);
	  display_number((int)MQ135_ppm*10);
	  HAL_Delay(100);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* TIM3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* I2C1_EV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */
//	HAL_I2C_MspInit(&hi2c1);
  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 16-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffffffff-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1599;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 9999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SEGA_Pin|SEGB_Pin|SEGC_Pin|SEGD_Pin
                          |SEGE_Pin|SEGF_Pin|SEGG_Pin|SEG_DOT_Pin
                          |LED_GAS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SEGA_Pin SEGB_Pin SEGC_Pin SEGD_Pin
                           SEGE_Pin SEGF_Pin SEGG_Pin SEG_DOT_Pin
                           LED_GAS_Pin */
  GPIO_InitStruct.Pin = SEGA_Pin|SEGB_Pin|SEGC_Pin|SEGD_Pin
                          |SEGE_Pin|SEGF_Pin|SEGG_Pin|SEG_DOT_Pin
                          |LED_GAS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_Pin LED2_Pin LED3_Pin LED4_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DHT11_Pin */
  GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
