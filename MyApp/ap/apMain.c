#include "apMain.h"
#include "myAdc.h"
#include "myUart.h"
#include "myTim.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"


#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern ADC_HandleTypeDef hadc1;


void apInit(void) { 
  // uartInit();
  // adcInit();
  myTim_Init();
}

float internal_temp =0;

void apMain(void) {

  while (1) {
    HCSR04_Trigger();

    HAL_Delay(60);

    printf(">distance : %.2f\r\n", distance);
    printf("distance : %.2f cm\r\n", distance);

    // HAL_Delay(1000);
  }
}
