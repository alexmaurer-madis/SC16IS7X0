#ifndef __ALEXMAURER_SC16IS7X0_H__
#define __ALEXMAURER_SC16IS7X0_H__

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_SPIDevice.h>
#include <Arduino.h>

#include "Stream.h"
#include "SC16IS7X0_defines.h"
#include "SC16IS7X0_BusIo.h"

class SC16IS7X0 : public Stream
{
public:
  SC16IS7X0(uint32_t crystalClock);
  virtual ~SC16IS7X0() {}

  bool begin_SPI(uint8_t cs_pin, SPIClass *theSPI = &SPI);
  bool begin_I2C(uint8_t addr, TwoWire *theWire = &Wire);
  void begin_UART(unsigned long baudrate, SerialConfig config = SERIAL_8N1);

  void updateBaudRate(unsigned long baudrate);

  int available(void) override;
  int peek(void) override { return -1; }
  int read(void) override;
  void flush() override{/*TODO*/};

#ifdef ESP8266
  int read(uint8_t *buffer, size_t len) override;
#else
  size_t readBytes(uint8_t *buffer, size_t len) override;
#endif

  size_t write(uint8_t c) override;
  size_t write(const uint8_t *buffer, size_t size) override;

  using Print::write; // Import other write() methods to support things like
                      // write(0) properly

  bool hasOverrun(void);
  bool hasRxError(void);

  void enableHardwareCTS(void);
  void disableHardwareCTS(void);

  void enableHardwareRTS(void);
  void disableHardwareRTS(void);

  void enableLoopback(void);
  void disableLoopback(void);

  void pinMode(uint8_t pin, uint8_t mode);
  void digitalWrite(uint8_t pin, uint8_t val);
  int digitalRead(uint8_t pin);

protected:
  Adafruit_SPIDevice *spi_dev = NULL; ///< Pointer to SPI bus interface
  enum Prescaler
  {
    DIVIDE_BY_1,
    DIVIDE_BY_4
  };

private:
  void writeDivisorAndPrescaler(uint32_t divisor, Prescaler prescaler);
  void enableEnhancedFunctions(void);
  void enableFIFO(void);
  void enableTCR_TLR(void);
  void disableTCR_TLR(void);
  uint8_t txlvl(void);
  bool setBusIo(SC16IS7X0_BusIo *theBusIo);

  static uint8_t getWordLength(SerialConfig config);
  static uint8_t getParity(SerialConfig config);
  static uint8_t getStopBits(SerialConfig config);

  uint8_t _mcr;
  uint8_t _lcr;
  uint8_t _efr;
  uint32_t _xtalFreq;
  uint8_t _ioDir;
  uint8_t _ioState;
  SC16IS7X0_BusIo *busIo;
};

#endif