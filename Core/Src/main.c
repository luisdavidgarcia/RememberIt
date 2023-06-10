/*
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ILI9341.h"
#include <stdint.h>
#include <stdlib.h>
#include "joystick.h"
#include "adc.h"

/* Private variables ---------------------------------------------------------*/
RNG_HandleTypeDef hrng;
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;
UART_HandleTypeDef huart2;

// Define the ADC global variables
volatile uint16_t adc1_value = 0;
volatile uint8_t adc1_ready = 0;
volatile uint16_t adc2_value = 0;
volatile uint8_t adc2_ready = 0;

// State machine states
typedef enum {
	IDLE, GENERATE, DISPLAY, READ, GAME_OVER, PASS
} STATE;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_RNG_Init(void);

// ADC interrupt handler
void ADC1_2_IRQHandler(void);



int main(void) {

	HAL_Init();
	SystemClock_Config();

	//touchscreen initialization
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_SPI1_Init();
	MX_RNG_Init();
	ILI9341_Init();

	//joystick initialization
	ADC12_Init();
	setupJoystickButton();

	//keep track of state in state machine
	STATE state = IDLE;

	//valid directions for next level
	DIR options[4] = { UP, DOWN, LEFT, RIGHT };

	//keep track of solutions thus far
	DIR cache[50];

	//curent level or amount of blue flashes to memorize
	uint8_t cnt = 0;
	while (1) {

		//state machine
		switch (state) {

			//do nothing state, wait for button press
			case IDLE: {
				state = GENERATE;	//next state generate random direction
				displayEmpty();	//clear screen

				//wait for btn press to move one
				while (detectButtonPress() == -1);
				HAL_Delay(400);
				break;
			}

			//generate next random direction
			case GENERATE: {
				//get first 2 bits of RNG since only 4 options to choose from direction-wise
				cache[cnt] = options[RNG->DR & 0x3];
				state = DISPLAY;	//display pattern so far for user to mimic
				cnt++;
				break;
			}

			//display pattern to mimic
			case DISPLAY: {
				//display all paterns up to cnt
				for (uint8_t i = 0; i < cnt; i++) {
					displayDir(cache[i]);
					HAL_Delay(500);
					displayEmpty();
					HAL_Delay(50);
				}
				//go on to read state to read user input and check againt solution
				state = READ;
				break;
			}

			//read user input and compare to solution
			case READ: {
				displayEmpty();	//clear LCD

				//read # of user inputs equal to current level
				uint8_t i = 0;
				while (i < cnt) {
					//reset joystick value
					adc1_value = 1900;
					adc2_value = 1900;	//reset add values

					//wait for joystick joystick to be moved by user
					while (readJoystick(adc1_value, adc2_value) == NONE) {
						startJoystickRead();		//start adc conversion
						while (!checkJoystickComplete(adc1_ready, adc2_ready));	//wait for both adc conversoin to be complete
					}
					//get joystick directoin if valid direction inputted
					DIR dir = readJoystick(adc1_value, adc2_value);

					//display input joystick direction
					displayDir(dir);
					HAL_Delay(500);
					displayEmpty();

					//if input not equal to answer game over
					if (dir != cache[i]) {
						state = GAME_OVER;
						break;
					}
					//if correct joystick get next joystick input
					else {
						i++;

						//if level finished exit state and get ready for next level
						if (i == cnt) {
							state = PASS;
						}
					}
				}
				break;
			} //end case

			//game over screen (displays red)
			case GAME_OVER: {
				displayFail();
				state = IDLE;		//go back to starting state
				cnt = 0;

				//wait for btn press to move one
				while (detectButtonPress() == -1)
					;
				HAL_Delay(200);
				break;
			}

			//pass level screen (displays green)
			case PASS: {
				displayPass();
				state = GENERATE;

				//reset if max count reached
				if (cnt == 50) {
					cnt = 0;
					state = IDLE;
				}
				HAL_Delay(500);
				break;
			}

			default:
				break;
		} //end switch
	}
}

void ADC1_2_IRQHandler(void) {
	//check if ADC1 conversion done
	if (ADC1->ISR & ADC_ISR_EOC) {
		// Save the digital conversion to a global variable
		adc1_value = ADC1->DR;
		// Set the global flag
		adc1_ready = 1;
		// Clear the end of conversion flag
		ADC1->ISR &= ~(ADC_ISR_EOC);
	}

	//check if ADC2 conversion done
	if (ADC2->ISR & ADC_ISR_EOC) {
		// Save the digital conversion of ADC2 to a global variable
		adc2_value = ADC2->DR;
		// Set the global flag for ADC2
		adc2_ready = 1;
		// Clear the end of conversion flag for ADC2
		ADC2->ISR &= ~(ADC_ISR_EOC);
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief RNG Initialization Function
 * @param None
 * @retval None
 */
static void MX_RNG_Init(void) {
	/*
	Initialize random number generator generated from IOC
	*/

	hrng.Instance = RNG;
	if (HAL_RNG_Init(&hrng) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN RNG_Init 2 */

	/* USER CODE END RNG_Init 2 */

}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LCD_BACKLIGHT_GPIO_Port, LCD_BACKLIGHT_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, LCD_RESET_Pin | LCD_DC_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LCD_BACKLIGHT_Pin */
	GPIO_InitStruct.Pin = LCD_BACKLIGHT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_BACKLIGHT_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LCD_CS_Pin */
	GPIO_InitStruct.Pin = LCD_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LCD_RESET_Pin LCD_DC_Pin */
	GPIO_InitStruct.Pin = LCD_RESET_Pin | LCD_DC_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_
	Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
