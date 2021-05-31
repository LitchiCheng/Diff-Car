#include <drv_encoder.h>
#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>

#include "stm32f1xx_hal.h"

TIM_HandleTypeDef htim1;

void MX_TIM1_Init()
{
    /* USER CODE BEGIN TIM1_Init 0 */

      /* USER CODE END TIM1_Init 0 */

      TIM_Encoder_InitTypeDef sConfig = {0};
      TIM_MasterConfigTypeDef sMasterConfig = {0};

      /* USER CODE BEGIN TIM1_Init 1 */

      /* USER CODE END TIM1_Init 1 */
      htim1.Instance = TIM1;
      htim1.Init.Prescaler = 0;
      htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
      htim1.Init.Period = 65535;
      htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      htim1.Init.RepetitionCounter = 0;
      htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
      sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
      sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
      sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
      sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
      sConfig.IC1Filter = 0;
      sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
      sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
      sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
      sConfig.IC2Filter = 0;
      if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
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
      HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
      /* USER CODE END TIM1_Init 2 */

}

/**
* @brief TIM_Encoder MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_encoder: TIM_Encoder handle pointer
* @retval None
*/
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_encoder)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim_encoder->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PE9     ------> TIM1_CH1
    PE11     ------> TIM1_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_TIM1_ENABLE();

  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }

}

/**
* @brief TIM_Encoder MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_encoder: TIM_Encoder handle pointer
* @retval None
*/
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef* htim_encoder)
{
  if(htim_encoder->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();

    /**TIM1 GPIO Configuration
    PE9     ------> TIM1_CH1
    PE11     ------> TIM1_CH2
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_9|GPIO_PIN_11);

  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }

}

void clearCounter(void)
{
    __HAL_TIM_SET_COUNTER(&htim1, 0);
}

void getCounter(rt_int16_t *speed)
{
    *speed = __HAL_TIM_GET_COUNTER(&htim1);
}

int hw_ENCODER_init(void)
{
    MX_TIM1_Init();
    return RT_EOK;
}

INIT_BOARD_EXPORT(hw_ENCODER_init);
