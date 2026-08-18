#include "myUart.h"
#include <stdio.h>
#include <string.h>

/* Standard I/O Re-targeting for printf() */
#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
  /* Polling 방식으로 1바이트 전송 (전송 완료될 때까지 대기) */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

#define RX_BUF_SIZE 128
uint8_t rx_buf1[RX_BUF_SIZE];
uint8_t rx_buf2[RX_BUF_SIZE];
uint8_t rx_data2;
uint16_t rx2_idx;


void uartInit(void) {
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1,rx_buf1, RX_BUF_SIZE);
  HAL_UART_Receive_IT(&huart2, &rx_data2, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if(huart->Instance == USART2) {
    if(rx_data2 == '\b')
    {
      if (rx2_idx > 0) {
        rx2_idx--; // 버퍼 커서를 한 칸 뒤로
        rx_buf2[rx2_idx] = 0; // 지우기
        //나의 터미널에도 백스페이스 적용
        HAL_UART_Transmit(&huart2, (uint8_t*)"\b \b", 3, 10);
      }
    }
    else if(rx_data2 == '\r' || rx_data2 == '\n')
    {
      rx_buf2[rx2_idx] = '\r';
      rx2_idx++;
      rx_buf2[rx2_idx] = '\n';
      rx2_idx++;
      HAL_UART_Transmit(&huart1, rx_buf2,rx2_idx,100);
      //나의 터미널에 엔터 적용
      HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n",2,100);
      memset(rx_buf2,0,rx2_idx);
      rx2_idx = 0;
      HAL_UARTEx_ReceiveToIdle_DMA( &huart1, rx_buf1,RX_BUF_SIZE);
    }
    else{
      //나의 터미널에 문자 출력
      if (rx2_idx < RX_BUF_SIZE - 1) { // 버퍼 오버플로우 방지
        rx_buf2[rx2_idx++] = rx_data2;
        HAL_UART_Transmit(&huart2, &rx_data2, 1, 10);
      }
    }
    // 다음 1바이트 수신 대기
    HAL_UART_Receive_IT(&huart2, &rx_data2, 1);
  }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
  if(huart->Instance == USART1){
    HAL_UART_Transmit(&huart2, rx_buf1,Size,100);
    memset(rx_buf1,0,Size);
    HAL_UARTEx_ReceiveToIdle_DMA( &huart1, rx_buf1,RX_BUF_SIZE);

    
  }
  // if(huart->Instance == USART2){
  //   HAL_UART_Transmit(&huart1, rx_buf2,Size,100);
  //   HAL_UART_DMAStop(&huart2);
  //   memset(rx_buf2,0,Size);
  //   HAL_UARTEx_ReceiveToIdle_DMA( &huart2, rx_buf2,RX_BUF_SIZE);
  // }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
  HAL_UARTEx_ReceiveToIdle_DMA( &huart1, rx_buf1,RX_BUF_SIZE);
}