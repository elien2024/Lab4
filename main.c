/* USER CODE BEGIN Header */
/**
  **
  * @file           : main.c
  * @brief          : Main program body
  **
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Keypad4X4.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fonts.h"
#include "ssd1306.h"
/* USER CODE END Includes */
#define NO_KEY '\0' // Définition de NO_KEY

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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
extern char key;
char user_code[7] = ""; // Augmentation de la taille pour accueillir jusqu'à 6 caractères + '\0'
int armed = 0;
char input_code[7] = "";
int input_index = 0;
int code_set = 0; // Indicateur si le code utilisateur est défini
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
void showCountdown(int seconds) {
    char countdownMsg[20];
    for (int i = seconds; i >= 0; i--) {
        SSD1306_Clear();
        SSD1306_GotoXY(0, 0);
        sprintf(countdownMsg, "Leave in %d sec", i);
        SSD1306_Puts(countdownMsg, &Font_11x18, 1);
        SSD1306_UpdateScreen();
        HAL_Delay(1000);
    }
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  SSD1306_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
if (!code_set) {
        SSD1306_Clear();
        SSD1306_GotoXY(0, 0);
        SSD1306_Puts("Set Code:", &Font_11x18, 1);
        SSD1306_UpdateScreen();
        input_index = 0;
        memset(user_code, 0, sizeof(user_code)); // Réinitialiser user_code

        while(input_index < 6) { // Accepter jusqu'à 6 chiffres pour le code
            key = Get_Key();
            HAL_Delay(100); // Debounce delay
            if(key == 'A' && input_index >= 4) { // '#': fin de saisie, avec au moins 4 chiffres
                user_code[input_index] = '\0'; // Terminer la chaîne de caractères
                code_set = 1;
                SSD1306_Clear();
                SSD1306_GotoXY(0, 0);
                SSD1306_Puts("Code Set", &Font_11x18, 1);
                SSD1306_UpdateScreen();
                HAL_Delay(2000); // Donner du temps pour lire le message
                break; // Sortir de la boucle de saisie du code
            } else if (key != NO_KEY && key != 'A') {
                if(input_index < 6) { // Ajouter le chiffre au code
                    user_code[input_index++] = key;
                    SSD1306_GotoXY(input_index * 11, 30);
                    SSD1306_Puts("*", &Font_11x18, 1); // Afficher un astérisque pour le chiffre entré
                    SSD1306_UpdateScreen();
                }
            }
        }
    } else {
        // Une fois le code défini, permettre l'armement/désarmement
        SSD1306_Clear();
        SSD1306_GotoXY(0, 0);
        SSD1306_Puts(armed ? "Disarm:" : "Arm:", &Font_11x18, 1);
        SSD1306_UpdateScreen();
        input_index = 0;
        memset(input_code, 0, sizeof(input_code)); // Réinitialiser input_code

        while(input_index < strlen(user_code)) {
            key = Get_Key();
            HAL_Delay(100); // Debounce delay
            if(key != NO_KEY) {
                input_code[input_index++] = key;
                SSD1306_GotoXY(input_index * 11, 30);
                SSD1306_Puts("*", &Font_11x18, 1);
                SSD1306_UpdateScreen();
            }
            if(input_index == strlen(user_code)) {
                input_code[input_index] = '\0'; // Terminer la chaîne de caractères
                if(strcmp(input_code, user_code) == 0) { // Si le code entré correspond au code utilisateur
                    armed = !armed; // Basculer l'état armé/désarmé
                    SSD1306_Clear();
                    SSD1306_GotoXY(0, 0);
                    SSD1306_Puts(armed ? "System Armed" : "System Disarmed", &Font_11x18, 1);
                    SSD1306_UpdateScreen();
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, armed ? GPIO_PIN_SET : GPIO_PIN_RESET); // LED rouge pour armé, verte pour désarmé
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, armed ? GPIO_PIN_RESET : GPIO_PIN_SET);

                }if (armed) {
                    // Afficher "System Armed" et allumer la LED rouge
                    SSD1306_Clear();
                    SSD1306_GotoXY(0, 0);
                    SSD1306_Puts("System Armed", &Font_11x18, 1);
                    SSD1306_UpdateScreen();
                    HAL_Delay(2000);
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET); // LED rouge
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET); // LED verte éteinte

                    // Compte à rebours de 60 secondes avant d'activer la détection de mouvement
                    showCountdown(10);


                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
                    HAL_Delay(6000); // Simuler attente pour désarmement
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
                    HAL_Delay(6000); // Simuler attente pour désarmement
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
                    HAL_Delay(6000); // Simuler attente pour désarmement
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
                    HAL_Delay(3000); // Simuler attente pour désarmement
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
                    HAL_Delay(6000); // Simuler attente pour désarmement
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
                    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12) == GPIO_PIN_SET) { // Si mouvement détecté

                            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // Activer le buzzer

                             // Attente optionnelle ici - par exemple, pour permettre à l'utilisateur de désarmer le système
                             // Remarque : Pour une réelle application de sécurité, cette attente pourrait être utilisée pour donner
                             // un délai avant que l'alarme ne se déclenche complètement, permettant un désarmement préventif.
                            HAL_Delay(6000); // Attendre 60 secondes (ajustez selon le besoin)

                            // Après l'attente (ou immédiatement si aucun délai n'est souhaité), désactiver le buzzer
                             HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // Désactiver le buzzer
                           break; // Sortir de la boucle si l'alarme a été déclenchée
                                            }


                    // Juste après la fin du compte à rebours
                    SSD1306_Clear();
                    SSD1306_GotoXY(0, 0);
                    SSD1306_Puts("Enter Code:", &Font_11x18, 1);
                    SSD1306_UpdateScreen();

                    input_index = 0;
                    memset(input_code, 0, sizeof(input_code)); // Réinitialisation du code d'entrée

                    while(input_index < strlen(user_code)) {
                        key = Get_Key();
                        HAL_Delay(100); // Delai anti-rebond
                        if(key != NO_KEY) {
                            input_code[input_index++] = key;
                            SSD1306_GotoXY(input_index * 11, 30);
                            SSD1306_Puts("*", &Font_11x18, 1); // Afficher un astérisque pour chaque chiffre entré
                            SSD1306_UpdateScreen();
                        }
                        if(input_index == strlen(user_code)) {
                            input_code[input_index] = '\0'; // Null-termination du code d'entrée
                            if(strcmp(input_code, user_code) == 0) { // Si les codes correspondent
                                armed = 0; // Désarmer le système
                                SSD1306_Clear();
                                SSD1306_GotoXY(0, 0);
                                SSD1306_Puts("System Disarmed", &Font_11x18, 1);
                                SSD1306_UpdateScreen();
                                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); // LED rouge éteinte
                                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET); // LED verte

                                break; // Quitter la boucle car le système est désormais désarmé
                            } else {
                                // Si le code est incorrect, indiquez-le et redemandez
                                SSD1306_Clear();
                                SSD1306_GotoXY(0, 0);
                                SSD1306_Puts("Wrong Code", &Font_11x18, 1);
                                SSD1306_UpdateScreen();
                                HAL_Delay(2000); // Donner du temps pour lire le message
                                input_index = 0; // Réinitialiser pour une nouvelle saisie
                                memset(input_code, 0, sizeof(input_code)); // Effacer le buffer de code d'entrée
                            }
                        }
                    }
                    // Assurez-vous d'ajouter la logique pour gérer le cas où le système reste armé et entre dans la boucle de détection de mouvement


                    // Surveillance de mouvement par le PIR
                    // Surveillance de mouvement par le PIR
                    /*while (armed) {
                        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12) == GPIO_PIN_SET) { // Si mouvement détecté
                            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // Activer le buzzer

                            // Attente optionnelle ici - par exemple, pour permettre à l'utilisateur de désarmer le système
                            // Remarque : Pour une réelle application de sécurité, cette attente pourrait être utilisée pour donner
                            // un délai avant que l'alarme ne se déclenche complètement, permettant un désarmement préventif.
                            HAL_Delay(60000); // Attendre 60 secondes (ajustez selon le besoin)

                            // Après l'attente (ou immédiatement si aucun délai n'est souhaité), désactiver le buzzer
                            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // Désactiver le buzzer
                            break; // Sortir de la boucle si l'alarme a été déclenchée
                        }
                        HAL_Delay(100); // Réduction de l'usage CPU
                    }
                */
                }
                else {
                    SSD1306_Clear();
                    SSD1306_GotoXY(0, 0);
                    SSD1306_Puts("Wrong Code", &Font_11x18, 1);
                    SSD1306_UpdateScreen();
                    HAL_Delay(2000); // Donner du temps pour lire le message
                }
                input_index = 0; // Réinitialiser pour la prochaine entrée
                memset(input_code, 0, sizeof(input_code));
                SSD1306_UpdateScreen();
            }
        }
    }
  }

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_9|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, KC0_Pin|KC3_Pin|KC1_Pin|KC2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC2 PC3 PC10 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_9|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : KC0_Pin KC3_Pin KC1_Pin KC2_Pin */
  GPIO_InitStruct.Pin = KC0_Pin|KC3_Pin|KC1_Pin|KC2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : KR1_Pin */
  GPIO_InitStruct.Pin = KR1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(KR1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : KR3_Pin KR2_Pin */
  GPIO_InitStruct.Pin = KR3_Pin|KR2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : KR0_Pin */
  GPIO_InitStruct.Pin = KR0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(KR0_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

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
