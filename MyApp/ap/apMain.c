#include "apMain.h"
#include "myAdc.h"
#include "myUart.h"
#include "myTim.h"
#include "myDht11.h"
#include "myI2c.h"
#include "myLcd1602.h"
#include "myMpu6050.h"
#include "mySsd1306.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"

#include <stdint.h>
#include <string.h>

extern ADC_HandleTypeDef hadc1;

int time = 0;

void apInit(void) { 
  // adcInit(); // 초음파 센서랑 포트가 겹쳐서 꺼놨음
  myTim_Init();
  dht11Init();
  lcd1602Init();
  mpu6050Init();
  ssd1306Init();
  
  i2cScan();
  uartInit();

}

float internal_temp =0;
dht11Data_t dht_data = {0};
static mpu6050Data_t mpu_data = {0};
bool dht_status = false;
bool mpu_status = false;



void apMain(void) {
  uint32_t tick_1000 = 0;
  uint32_t tick_250 = 0;
  uint32_t tick_100 = 0;
  uint32_t tick_50 = 0;
  uint32_t current_tick = 0;
  
  // lcdOpen();
  
  while (1) {
    current_tick = HAL_GetTick();
    
    // ssd1306Clear();
    // ssd1306DrawRect(0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, SSD1306_COLOR_WHITE);
    // ssd1306DrawString(8, 3, "STM32 MULTI-SENSOR", SSD1306_COLOR_WHITE);
    // ssd1306DrawLine(4, 13, 124, 13, SSD1306_COLOR_WHITE);
    // for (uint16_t i = 0; i < 10; i++) {
    //   ssd1306DrawIcon(3+i*6, 48, i, SSD1306_COLOR_WHITE);
    // }

    if (current_tick - tick_1000 >= 1000) {
      tick_1000 = current_tick;

    }

    if (current_tick - tick_250 >= 250) {
      tick_250 = current_tick;
      HCSR04_Trigger();

      dht_status = dht11Read(&dht_data);
      lcd1602Clear();
      // if (time < 10) {
      //   lcd1602Cursor(0, 0);
      //   lcd1602Printf("Temperature:%.1f",dht_data.temperature);
      // }
      // else {
      //   lcd1602Cursor(0, 1);
      //   lcd1602Printf("Humidity:%.2f",dht_data.humidity);
      //   if (time > 20) { time = 0; }
      // }
      // lcd1602Cursor(1, 1);
      // lcd1602Printf("Distance:%.2f",distance);
      // printf(">distance : %.2f\r\n", distance);
      // printf("distance : %.2f cm\r\n", distance);
      
    
      time++;
    }

    if (current_tick - tick_100 >= 100) {
      tick_100 = current_tick;

      if(mpu6050Read(&mpu_data)) {
        printf(">acc_x : %.3f\n>acc_y : %.3f\n>acc_z : %.3f\n>gyro_x : %.3f\n>gyro_y : %.3f\n>gyro_z : %.3f\n",
          mpu_data.accel_x,mpu_data.accel_y,mpu_data.accel_z,mpu_data.gyro_x,mpu_data.gyro_y,mpu_data.gyro_z
        );
      }
      
      // ssd1306Printf(3, 16, SSD1306_COLOR_WHITE, "acc_x : %.3f",mpu_data.accel_x);
      // ssd1306Printf(3, 24, SSD1306_COLOR_WHITE, "acc_y : %.3f",mpu_data.accel_y);
      // ssd1306Printf(3, 32, SSD1306_COLOR_WHITE, "acc_z : %.3f",mpu_data.accel_z);
      // ssd1306Printf(3, 40, SSD1306_COLOR_WHITE, "Distance : %.2f",distance);
      // ssd1306Update();
    }

    if (current_tick - tick_50 >= 50) {
      tick_50 = current_tick;

    }
    // adcUpdate();



    //badapple
    lcd1602Cursor(0, 0);
    lcd1602Printf("If it have Screen");
    lcd1602Cursor(1, 1);
    lcd1602Printf("Play  BadApple");
     /*
      * 정확히 1024바이트 수신 완료
      */
    if (frame_ready) {
      frame_ready = 0;
      /*
      * UART DMA로 받은 프레임을
      * SSD1306 화면 버퍼로 복사
      */
      ssd1306DrawFrame(frame_buf);

      /*
      * OLED에 실제 출력
      */
      ssd1306Update();

      /*
        * Python에게
        * "이 프레임 출력 끝났음"
        * ACK 전송
        */
      uint8_t ack = 'A';
      HAL_UART_Transmit( &huart2, &ack,1, 100 );

      /*
      * 다음 프레임 1024바이트 DMA 수신 시작
      */
      HAL_UART_Receive_DMA( &huart2, frame_buf, FRAME_SIZE );
    }
  }
}
