#include "myTim.h"
#include "tim.h"
#include <stdio.h>

/*
volatile 변수 설명: 다른곳에서 변경될 수 있는 변수이므로, 컴파일러 최적화 시 주의가 필요함
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

void HCSR04_Trigger(void) // 측정 시작 함수
{
    // pa1을 LOW로 1마이크로초 유지 후 HIGH로 10 마이크로초 유지 후 다시 LOW로
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
        if (capture_state == 0) // 초기상태
        {
            // Rising Edge
            // ECHO가 HIGH가 된 순간의 TIM2 값 저장
            echo_start = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            capture_state = 1;

            // Falling Edge로 변경

            // 이제 ch1가 low->high 변화가 아닌 high->low 변화가 발생하면 인터럽트가 발생하도록 설정
            __HAL_TIM_SET_CAPTUREPOLARITY(
                htim,
                TIM_CHANNEL_1,
                TIM_INPUTCHANNELPOLARITY_FALLING
            );
        }
        else
        {
            // Falling Edge
            // ECHO가 LOW가 된 순간의 TIM2 값 저장
            echo_end = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            if (echo_end >= echo_start)
            {
                echo_time = echo_end - echo_start;
            }
            else
            {
                // Timer overflow 발생
                // 끝값을 65535로 가정했으니 
                // 특정 상황에서 음수가 아닌 양수로 계산되도록 처리 
                // ex) start = 65500 end = 100 → 65535 - 65500 + 100 + 1 = 136
                echo_time = (65535 - echo_start) + echo_end + 1;
            }

            // 거리 계산
            distance = echo_time * 0.01715f;
            // printf("start=%lu end=%lu time=%lu distance=%.2f\r\n",
            //        echo_start,
            //        echo_end,
            //        echo_time,
            //        distance);

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

/*
tim2 -> Timer 2
tim2 ch1 -> input capture은 특정 핀의 신호가 변하는 순간 타이머 현재값 기록

input capture Polarity Rising Edge설정
0에서 1로 변하는 순간 타이머 현재값 기록
falling Edge는 코드로 설정

Prescaler 설정
빠른 속도로 들어오는 클럭 속도를 나눠서 너무 많은 숫자를 세지 않도록
83넣는 이유 = cpu클럭이 84MHz이므로 1MHz로 나누기 위해서
TIM2 clock = 84MHz / (Prescaler + 1) = 1MHz

counter period 설정
타이머가 0~65535까지 세고 다시 0으로 돌아가도록
65535인 이유 = 16bit이므로 2^16 - 1 = 65535

clock source 설정
Internal clock 사용 = 외부 클럭시노가 아닌 stm32 내부 클럭을 사용해서 타이머를 움직임

NVIC 설정 -> 이벤트가 발생하면 CPU에게 알려주는 역할
tim2 global interrupt -> Add 체크
*/ 
