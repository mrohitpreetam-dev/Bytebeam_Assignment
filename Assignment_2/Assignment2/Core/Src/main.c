/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "os.h"
#include "stdio.h"
#include "string.h"
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
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
  Queue queue;
  BinarySemaphore sem;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
  void heartbeat_task(void);
  void task_1(void);
  void task_2(void);
  void uart_task(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
  void heartbeat_task(void){
      // printf("Heartbeat\n");
      char my_msg[] = "Heartbeat\r\n";
      HAL_UART_Transmit(&huart1, (uint8_t*)my_msg, sizeof(my_msg) - 1, HAL_MAX_DELAY);
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
      os_sleep(1000);
  }

  void task_1(void){
      static int count = 0;
      
      // printf("Enqueued value: %d\n", count);
      enqueue(&queue, count);

      char msg[16];
      sprintf(msg, "Enqueued: %d\r\n", count);
      HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

      count++;
      sem_post(&sem);
      os_sleep(5000);
      if(os_yield()) return;
  }

  void task_2(void){
      int value;

      if(!sem_wait(&sem)){
          return;
      }

      if(dequeue(&queue, &value) == 0){
          // printf("Dequeued value: %d\n", value);
          char msg[16];
          sprintf(msg, "dEnqueued: %d\r\n", value);
          HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
      }
      os_sleep(5000);
      if(os_yield()) return;

  }

  void uart_task(void){
    char rx_buffer[20];
    int i = 0;
    char c;
    HAL_StatusTypeDef status;
      // HAL_UART_Receive(&huart1, (uint8_t*)&c, 1, 100);
    char my_msg[] = "In Uart task\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)my_msg, sizeof(my_msg) - 1, HAL_MAX_DELAY);
    while ((status = HAL_UART_Receive(&huart1, (uint8_t*)&c, 1,  0)) == HAL_OK){   
        HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1, 100);
        if (c != '\r' && c != '\n'){
          rx_buffer[i++] = c;
          if(i >= sizeof(rx_buffer)-1) 
              i = sizeof(rx_buffer)-2;
          }
          else{
            rx_buffer[i] = '\0';
            if(strcmp(rx_buffer, "status") == 0){
              char msg[64];
              sprintf(msg,"SysTicks: %lu\r\nQueue: %d\r\nSemaphore: %d\r\n",os_now(),queue.count,sem.value);
              HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
            }
            else if(strcmp(rx_buffer, "tasks") == 0){
                  // char msg[] ="Tasks:\r\n""Heartbeat\r\n""Task 1\r\n""Task 2\r\n""UART_Task\r\n";
                  // HAL_UART_Transmit(&huart1,(uint8_t*)msg,sizeof(msg)-1,HAL_MAX_DELAY);
                  TCB *tasks = os_get_task_list();
                  int count = os_get_task_count();

                  for(int i = 0; i < count; i++)
                  {
                      char msg[64];
                      sprintf(msg,"Task %d : %s\r\n", i, tasks[i].name);
                      HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
                  }
            }
            else if(strcmp(rx_buffer, "state") == 0){
                  TCB *tasks = os_get_task_list();
                  int count = os_get_task_count();

                  for(int i = 0; i < count; i++)
                  {
                      char *state;
                      switch(tasks[i].state)
                      {
                          case READY:    state = "READY"; break;
                          case RUNNING:  state = "RUNNING"; break;
                          case SLEEPING: state = "SLEEPING"; break;
                          case BLOCKED:  state = "BLOCKED"; break;
                          default:       state = "UNKNOWN"; break;
                      }

                      char msg[80];
                      sprintf(msg,"%s : %s | Wakeup: %lu\r\n",tasks[i].name,state,(unsigned long)tasks[i].wakeup_time);
                      HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
                  }
            }
            else if(strcmp(rx_buffer, "help") == 0){
              char msg[] ="Commands:\r\n""help   - Show commands\r\n""status - OS status\r\n""tasks  - List tasks\r\n""state  - Task states\r\n""tick   - Current system tick\r\n""queue  - Queue information\r\n""about  - Project information\r\n";
              HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg) - 1, HAL_MAX_DELAY);
            }
            else if(strcmp(rx_buffer, "about") == 0){
              char msg[] ="Simple Cooperative RTOS\r""Priority Scheduler\r""Binary Semaphore + Queue\r\n";
              HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
            }
            else if(strcmp(rx_buffer, "tick") == 0){
              char msg[32];
              sprintf(msg,"Tick: %lu\r\n",(unsigned long)os_now());
              HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
            }
            else if(strcmp(rx_buffer, "queue") == 0){
              char msg[64];
              sprintf(msg,"Count:%d Head:%d Tail:%d\r\n",queue.count,queue.head,queue.tail);
              HAL_UART_Transmit(&huart1,(uint8_t*)msg,strlen(msg),HAL_MAX_DELAY);
            }
            i = 0;
          }    
      }
      if (status == HAL_TIMEOUT) {
        os_sleep(10000); 
        if(os_yield()) return;
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
  __HAL_AFIO_REMAP_SWJ_ENABLE();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // while (1)
  // {
  //   /* USER CODE END WHILE */

  //   /* USER CODE BEGIN 3 */
  // }
    os_init();

    queue_init(&queue);
    sem_init(&sem);

    os_task_create(heartbeat_task, 1, "Heartbeat");
    os_task_create(task_1, 2, "Task 1");
    os_task_create(task_2, 2, "Task 2");
    os_task_create(uart_task, 3, "UART_Task");

    os_start();
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
#ifdef USE_FULL_ASSERT
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
