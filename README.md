# Add an additional hardware UART to your Arduino project.

DEV BRANCH OF LIBRARY

| Feature           | Tested ESP8266 | Tested ESP32 | Tester |
| :---------------- | :------------- | :----------- | :----- |
| I2C communication | -              | -            | -      |


# Contributors

- The dev branch was created and committed with the fork of asantiagod https://github.com/asantiagod/SC16IS7X0 


# Compatibility and dependencies
- SC16IS740_750_760 Library (MIT License)
- Written for Arduino Framework.
- Work on platform ESP8266 / ESP32
- Depends on [Adafruit_BusIO Library](https://github.com/adafruit/Adafruit_BusIO) (MIT License)

# Programmed features
- This library inherits from Stream class
- SPI communication
- 64 bytes FIFO (TX & RX)
- Hardware CTS / RTS Flow Control
- SC16IS750 and SC16IS760 provids you with 8 additional programmable I/O pins

# Not programmed yet
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

