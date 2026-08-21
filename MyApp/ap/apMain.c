#include "apMain.h"
#include "myAdc.h"
#include "myUart.h"
#include "myTim.h"
#include "myDht11.h"
#include "myI2c.h"
#include "myLcd1602.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"


#include <stdint.h>
#include <string.h>

extern ADC_HandleTypeDef hadc1;

int time = 0;

void apInit(void) { 
  uartInit();
  // adcInit();
  myTim_Init();
  dht11Init();
  i2cScan();
  lcd1602Init();

}

float internal_temp =0;
dht11Data_t dht_data = {0};
bool dht_status = false;

void apMain(void) {
  lcdOpen();

  while (1) {
    // adcUpdate();
    
    dht_status = dht11Read(&dht_data);
    
    HCSR04_Trigger();

    lcd1602Clear();
    if (time < 10) {
      lcd1602Cursor(0, 0);
      lcd1602Printf("Temperature:%.1f",dht_data.temperature);
    }
    else {
      lcd1602Cursor(0, 1);
      lcd1602Printf("Humidity:%.2f",dht_data.humidity);
      if (time > 20) { time = 0; }
    }
    lcd1602Cursor(1, 1);
    lcd1602Printf("Distance:%.2f",distance);
    printf(">distance : %.2f\r\n", distance);
    // printf("distance : %.2f cm\r\n", distance);
    
    time++;
    HAL_Delay(350);
  }
}
