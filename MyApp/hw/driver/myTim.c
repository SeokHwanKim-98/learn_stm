#include "myTim.h"
#include "tim.h"
#include <stdio.h>

/*
echo_start  → ECHO가 HIGH가 된 순간의 TIM2 값
echo_end    → ECHO가 LOW가 된 순간의 TIM2 값
echo_time   → ECHO가 HIGH였던 시간
distance    → 계산된 거리(cm)
capture_state → Rising/Falling 상태 구분
*/
volatile uint32_t echo_start = 0;
volatile uint32_t echo_end = 0;
volatile uint32_t echo_time = 0;

volatile uint8_t capture_state = 0;

volatile float distance = 0.0f;

void HCSR04_Trigger(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

    HAL_Delay(1);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

    for (volatile int i = 0; i < 100; i++);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}

void myTim_Init(void)
{
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        if (capture_state == 0)
        {
            // Rising Edge
            echo_start = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            capture_state = 1;

            // Falling Edge로 변경
            __HAL_TIM_SET_CAPTUREPOLARITY(
                htim,
                TIM_CHANNEL_1,
                TIM_INPUTCHANNELPOLARITY_FALLING
            );
        }
        else
        {
            // Falling Edge
            echo_end = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            if (echo_end >= echo_start)
            {
                echo_time = echo_end - echo_start;
            }
            else
            {
                // Timer overflow 발생
                echo_time = (65535 - echo_start) + echo_end + 1;
            }

            // 거리 계산
            distance = echo_time * 0.01715f;
            printf("Distance : %.2f cm\r\n", distance);


            capture_state = 0;

            // 다시 Rising Edge
            __HAL_TIM_SET_CAPTUREPOLARITY(
                htim,
                TIM_CHANNEL_1,
                TIM_INPUTCHANNELPOLARITY_RISING
            );
        }
    }
}