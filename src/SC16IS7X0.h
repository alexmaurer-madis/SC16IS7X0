#ifndef __ALEXMAURER_SC16IS7X0_H__
#define __ALEXMAURER_SC16IS7X0_H__

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_SPIDevice.h>
#include <Arduino.h>

#include "Stream.h"

// General register set

// Read, Receive Holding Register
#define SC16IS7X0_RHR 0x00
// Write, Transmit Holding Register
#define SC16IS7X0_THR 0x00
// R/W, Interrupt Enable Register
#define SC16IS7X0_IER 0x01
// Read, Interrupt Identification Register
#define SC16IS7X0_IIR 0x02
// Write, FIFO Control Register
#define SC16IS7X0_FCR 0x02
// R/W, Line Control Register
#define SC16IS7X0_LCR 0x03
// R/W, Modem Control Register
#define SC16IS7X0_MCR 0x04
// Read, Line Status Register
#define SC16IS7X0_LSR 0x05
// Read, Modem Status Register
#define SC16IS7X0_MSR 0x06
// R/W, Scratchpad register
#define SC16IS7X0_SPR 0x07
// R/W, Transmission Control Register (These registers are accessible only when
// MCR[2] = 1 and EFR[4] = 1)
#define SC16IS7X0_TCR 0x06
// R/W, Trigger Level Register (These registers are accessible only when MCR[2]
// = 1 and EFR[4] = 1)
#define SC16IS7X0_TLR 0x07
// Read, Transmit FIFO Level Register
#define SC16IS7X0_TXLVL 0x08
// Read, Receive FIFO Level Register
#define SC16IS7X0_RXLVL 0x09
// R/W, I/O pin Direction Register (Only available on the SC16IS750/SC16IS760)
#define SC16IS7X0_IODIR 0x0A
//!< R/W, I/O pin States Register (Only available on the SC16IS750/SC16IS760)
#define SC16IS7X0_IOSTATE 0x0B
// R/W, I/O Interrupt Enable Register (Only available on the
// SC16IS750/SC16IS760)
#define SC16IS7X0_IOINTENA 0x0C
// R/W, I/O pins Control Register (Only available on the SC16IS750/SC16IS760)
#define SC16IS7X0_IOCONTROL 0x0E
// R/W, Extra Features Register (Accessible only when LCR is set to 1011 1111b
// (0xBF))
#define SC16IS7X0_EFCR 0x0F

// Special Register Set (only accessible when LCR[7] is logic 1 and not 0xBF)

// R/W, Divisor Latch LSB
#define SC16IS7X0_DLL 0x00
// R/W, Divisor Latch MSB
#define SC16IS7X0_DLH 0x01

// Enhanced register set (only accessible when LCR = 0xBF)

// R/W, Enhanced Feature Register
#define SC16IS7X0_EFR 0x02
// R/W, XON1 word
#define SC16IS7X0_XON1 0x04
// R/W, XON2 word
#define SC16IS7X0_XON2 0x05
// R/W, XOFF1 word
#define SC16IS7X0_XOFF1 0x06
// R/W, XOFF2 word
#define SC16IS7X0_XOFF2 0x07

#define SC16IS7X0_READ_FLAG 0x80

class SC16IS7X0 : public Stream {
 public:
  SC16IS7X0(uint32_t crystalClock);
  virtual ~SC16IS7X0() {}

  bool begin_SPI(uint8_t cs_pin, SPIClass *theSPI = &SPI);

  void begin_UART(unsigned long baudrate) { begin_UART(baudrate, SERIAL_8N1); };
  void begin_UART(unsigned long baudrate, SerialConfig config);

  void updateBaudRate(unsigned long baudrate);

  int available(void) override;
  int peek(void) override { return -1; }
  int read(void) override;
  int read(uint8_t *buffer, size_t len) override;

  size_t write(uint8_t c) override;
  size_t write(const uint8_t *buffer, size_t size) override;

  using Print::write;  // Import other write() methods to support things like
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
  Adafruit_SPIDevice *spi_dev = NULL;  ///< Pointer to SPI bus interface
  enum Prescaler { DIVIDE_BY_1, DIVIDE_BY_4 };

 private:
  void writeDivisorAndPrescaler(uint32_t divisor, Prescaler prescaler);
  void enableEnhancedFunctions(void);
  void enableFIFO(void);
  void enableTCR_TLR(void);
  void disableTCR_TLR(void);
  uint8_t txlvl(void);

  uint8_t _mcr = 0x00;
  uint8_t _lcr = 0x03;
  uint8_t _efr = 0x00;
  uint32_t _xtalFreq;
  uint8_t _ioDir = 0x00;
  uint8_t _ioState = 0x00;
};

#endif