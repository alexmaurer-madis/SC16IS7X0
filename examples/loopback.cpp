#include <Arduino.h>
#include "SC16IS7X0.h"

//==========================================================
// short circuit the RX and TX pins of the SC16IS7X0 device
//==========================================================

constexpr uint32_t CRYSTAL_FREQ = 14745600;
constexpr uint8_t I2C_ADD = 0x90 >> 1;
constexpr uint32_t UART_BAUD = 115200;

SC16IS7X0 sc16is750(CRYSTAL_FREQ);

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("setup");

  if (sc16is750.begin_I2C(I2C_ADD))
    Serial.println("I2C initialized");
  sc16is750.begin_UART(UART_BAUD);
}

void loop()
{
  static uint32_t n=0;

  sc16is750.printf("loop %d\n", n++);
  while (sc16is750.available())
    Serial.write(sc16is750.read());

  delay(2000);
}