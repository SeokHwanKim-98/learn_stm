#include "myUart.h"

#include <stdint.h>
#include <stdio.h>


/* =========================================================
 * printf() 출력용
 *
 * USART1 → Teleplot
 * ========================================================= */

#ifdef __GNUC__

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

#else

#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#endif


PUTCHAR_PROTOTYPE
{
    /*
     * printf() 출력
     *
     * USART1 → PC → Teleplot
     */
    HAL_UART_Transmit(
        // &huart2, // Teleplot 에서 printf 보려면 이거 uart2
        &huart1, // Teleplot 안보고 ssd1306에서 BadApple 보려면 uart1
        (uint8_t *)&ch,
        1,
        HAL_MAX_DELAY
    );

    return ch;
}


/* =========================================================
 * Bad Apple UART2
 *
 * 128 x 64 / 8 = 1024 bytes
 * ========================================================= */


/* UART2 DMA로 받을 프레임 */
uint8_t frame_buf[FRAME_SIZE];


/* 1024바이트 수신 완료 플래그 */
volatile uint8_t frame_ready = 0;


/* =========================================================
 * UART 초기화
 * ========================================================= */

void uartInit(void)
{
    frame_ready = 0;

    /*
     * UART2로 정확히 1024바이트 DMA 수신 시작
     */
    HAL_UART_Receive_DMA(
        &huart2,
        frame_buf,
        FRAME_SIZE
    );
}


/* =========================================================
 * UART DMA 수신 완료 콜백
 *
 * 정확히 1024바이트를 모두 수신했을 때 호출
 * ========================================================= */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        frame_ready = 1;
    }
}


/* =========================================================
 * UART 오류 콜백
 * ========================================================= */

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        HAL_UART_AbortReceive(&huart2);

        frame_ready = 0;

        HAL_UART_Receive_DMA(
            &huart2,
            frame_buf,
            FRAME_SIZE
        );
    }
}