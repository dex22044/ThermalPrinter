/* USER CODE BEGIN Header */
/**
    ******************************************************************************
    * @file           : main.c
    * @brief          : Main program body
    ******************************************************************************
    * @attention
    *
    * Copyright (c) 2024 STMicroelectronics.
    * All rights reserved.
    *
    * This software is licensed under terms that can be found in the LICENSE file
    * in the root directory of this software component.
    * If no LICENSE file comes with this software, it is provided AS-IS.
    *
    ******************************************************************************
    */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "peripherals.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim1,0);  // set the counter value a 0
    while (__HAL_TIM_GET_COUNTER(&htim1) < us);  // wait for the counter to reach the us input in the parameter
}

uint16_t minimalRowDelay = 300;
uint16_t motorStepDelay = 1700;
uint16_t rowBurnTime = 400;
uint8_t pktIn[64], pktOut[64];
uint8_t lineData[54];

void processPacket() {
    if(pktIn[0] == 0x01) { // Ping
        pktOut[0] = 0x71;
        pktOut[1] = pktIn[1];
        return;
    }

    if(pktIn[0] == 0x02) { // Set param
        if(pktIn[1] == 0x01) { // Minimal row delay
            minimalRowDelay = ((uint16_t)pktIn[2] << 8) | pktIn[3];
            pktOut[0] = 0x72;
            pktOut[1] = 0x01;
            return;
        }
        if(pktIn[1] == 0x02) { // Motor step delay
            motorStepDelay = ((uint16_t)pktIn[2] << 8) | pktIn[3];
            pktOut[0] = 0x72;
            pktOut[1] = 0x02;
            return;
        }
        if(pktIn[1] == 0x03) { // Row burn time
            rowBurnTime = ((uint16_t)pktIn[2] << 8) | pktIn[3];
            pktOut[0] = 0x72;
            pktOut[1] = 0x03;
            return;
        }
        return;
    }

    if(pktIn[0] == 0x03) { // Get param
        if(pktIn[1] == 0x01) { // Minimal row delay
            pktOut[0] = 0x73;
            pktOut[1] = 0x01;
            pktOut[2] = (minimalRowDelay >> 8);
            pktOut[3] = (minimalRowDelay & 0xFF);
            return;
        }
        if(pktIn[1] == 0x02) { // Motor step delay
            pktOut[0] = 0x73;
            pktOut[1] = 0x02;
            pktOut[2] = (motorStepDelay >> 8);
            pktOut[3] = (motorStepDelay & 0xFF);
            return;
        }
        if(pktIn[1] == 0x03) { // Row burn time
            pktOut[0] = 0x73;
            pktOut[1] = 0x03;
            pktOut[2] = (rowBurnTime >> 8);
            pktOut[3] = (rowBurnTime & 0xFF);
            return;
        }
        return;
    }

    if(pktIn[0] == 0x04) { // Print line
        uint8_t enableStep = pktIn[1];
        memcpy(lineData, &pktIn[2], 54);
        printerBurnNahuyLine(lineData, rowBurnTime);
        if(enableStep) motorStep();
        delay_us(minimalRowDelay);
        pktOut[0] = 0x74;
        return;
    }

    if(pktIn[0] == 0x05) { // Motor step
        uint8_t direction = pktIn[1];
        uint16_t stepCount = ((uint16_t)pktIn[2] << 8) | pktIn[3];
        motorSetDirection(direction);
        for(uint16_t i = 0; i < stepCount; i++) {
            motorStep();
            delay_us(motorStepDelay);
        }
        motorSetDirection(0);
        pktOut[0] = 0x75;
        return;
    }

    if(pktIn[0] == 0x06) { // Enable/disable motor
        uint8_t enable = pktIn[1];
        motorSetEnabled(enable);
        pktOut[0] = 0x76;
        return;
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
    MX_USB_DEVICE_Init();
    MX_ADC1_Init();
    MX_TIM1_Init();
    /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start(&htim1);
    blinkLed(100, 30);
    motorSetEnabled(0);
    motorReset();
    motorSetDirection(0);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    cdc_init();

    while(1) {
        if(cdc_queueAvailable() < 64) continue;
        cdc_queueRead(pktIn, 64);
        processPacket();
        CDC_Transmit_FS(pktOut, 64);
        // delay_us(500);
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
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
    Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
    Error_Handler();
    }
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

    /** Common config
    */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
    Error_Handler();
    }

    /** Configure Regular Channel
    */
    sConfig.Channel = ADC_CHANNEL_9;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
    Error_Handler();
    }
    /* USER CODE BEGIN ADC1_Init 2 */

    /* USER CODE END ADC1_Init 2 */

}

/**
    * @brief TIM1 Initialization Function
    * @param None
    * @retval None
    */
static void MX_TIM1_Init(void)
{

    /* USER CODE BEGIN TIM1_Init 0 */

    /* USER CODE END TIM1_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM1_Init 1 */

    /* USER CODE END TIM1_Init 1 */
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 72-1;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 0xFFFF;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
    Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
    Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
    Error_Handler();
    }
    /* USER CODE BEGIN TIM1_Init 2 */

    /* USER CODE END TIM1_Init 2 */

}

/**
    * @brief GPIO Initialization Function
    * @param None
    * @retval None
    */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(INTERNAL_LED_GPIO_Port, INTERNAL_LED_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, MOT_DIR_Pin|MOT_STP_Pin|MOT_SLP_Pin|MOT_MS3_Pin
                            |MOT_MS2_Pin|MOT_MS1_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, MOT_RST_Pin|MOT_EN_Pin, GPIO_PIN_SET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(PRNT_CLK_GPIO_Port, PRNT_CLK_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, PRNT_LAT_Pin|PRNT_STB1_Pin|PRNT_STB3_Pin|PRNT_STB2_Pin
                            |PRNT_DAT_Pin, GPIO_PIN_SET);

    /*Configure GPIO pin : INTERNAL_LED_Pin */
    GPIO_InitStruct.Pin = INTERNAL_LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(INTERNAL_LED_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : MOT_DIR_Pin MOT_STP_Pin MOT_SLP_Pin MOT_RST_Pin
                             MOT_MS3_Pin MOT_MS2_Pin MOT_MS1_Pin MOT_EN_Pin */
    GPIO_InitStruct.Pin = MOT_DIR_Pin|MOT_STP_Pin|MOT_SLP_Pin|MOT_RST_Pin
                            |MOT_MS3_Pin|MOT_MS2_Pin|MOT_MS1_Pin|MOT_EN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PRNT_CLK_Pin PRNT_LAT_Pin PRNT_STB1_Pin PRNT_STB3_Pin
                             PRNT_STB2_Pin PRNT_DAT_Pin */
    GPIO_InitStruct.Pin = PRNT_CLK_Pin|PRNT_LAT_Pin|PRNT_STB1_Pin|PRNT_STB3_Pin
                            |PRNT_STB2_Pin|PRNT_DAT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
