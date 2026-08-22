#pragma once

#include "main.h"
#include "usart.h"

#include <stdint.h>
#include <stdio.h>

#define FRAME_SIZE 1024

/* Bad Apple UART2 DMA 수신 초기화 */
void uartInit(void);

/* UART2로 수신한 Bad Apple 프레임 */
extern uint8_t frame_buf[FRAME_SIZE];

/* 1024바이트 프레임 수신 완료 */
extern volatile uint8_t frame_ready;