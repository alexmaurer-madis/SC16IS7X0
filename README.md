# Add an additional hardware UART to your Arduino project.


**THIS README IS NOT YET FINISHED**

Library still in development.  
Some examples will be soon published.

# Compatibility and dependencies
- SC16IS740_750_760 Library (MIT License)
- Written for Arduino Framework.
- Work on platform esp8266
- Depends on [Adafruit_BusIO Library](https://github.com/adafruit/Adafruit_BusIO) (MIT License)

# Programmed features
- This library inherits from Stream class
- SPI communication
- 64 bytes FIFO (TX & RX)
- Hardware CTS / RTS Flow Control
- SC16IS750 and SC16IS760 provids you with 8 additional programmable I/O pins

# Not programmed yet
- I2C communication
- Usage of IRQ pin
- IrDA
- RS485 RTS control and 9-bit mode / multidrop

# Speed and timing
- SPI max clock frequency :
  - 4MHz for SC16IS750
  - 15MHz for SC16IS760
- Crystal oscillator :
  - 24MHz max
- External clock frequency :
  - 48MHz max @ 2.5V
  - 80MHz max @ 3.3V


# Code snippets

## Library initialization

```
#include "SC16IS7X0.h"

#define SPI_CS_GPIO 2

SC16IS7X0 sc16is750 = SC16IS7X0(1843200);

void setup() {
  Serial.begin(9600);

  sc16is750.begin_SPI(SPI_CS_GPIO);
  sc16is750.begin_UART(115200, SERIAL_8N1);
}


void loop() {
}
```

