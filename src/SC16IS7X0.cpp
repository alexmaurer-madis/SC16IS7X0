/**
 * @file SC16IS7X0.h
 * @author Alexandre Maurer (alexmaurer@madis.ch)
 * @brief SC16IS740 / SC16IS750 / SC16IS760 library
 * @details Only SPI interface for the moment. Does currently not implement the
 usage of IRQ pin, IrDA, RS485 RTS control and 9-bit mode.
 *
 * @version 1.0.0
 * @date 2022-11-04
 *

 * @copyright MIT License
 */

#include "SC16IS7X0.h"

/**
 * @brief
 *
 * @param crystalClock Frequence in Hz of the XTAL1
 */
SC16IS7X0::SC16IS7X0(uint32_t xtalFreq) : _mcr(0x00),
                                          _lcr(0x03),
                                          _efr(0x00),
                                          _ioDir(0x00),
                                          _ioState(0x00),
                                          busIo(nullptr)
{
  assert(xtalFreq > 0);
  _xtalFreq = xtalFreq;
}

/**************************************************************************/
/*!
  @brief Initialize using hardware SPI.
  @param cs_pin Pin to use for SPI chip select
  @param theSPI Pointer to SPI instance
  @return true if initialization successful, otherwise false.
*/
/**************************************************************************/
bool SC16IS7X0::begin_SPI(uint8_t cs_pin, SPIClass *theSPI)
{
  return setBusIo(SC16IS7X0_BusIo::buildSPI(cs_pin, 4000000, SPI_BITORDER_MSBFIRST,
                                     SPI_MODE0, theSPI));
}

/**
 * @brief Initialize using of hardware I2C 
 * @param addr    Address assigned to the I2C device
 * @param theWire Pointer to I2C instance
 * @return true   Initialization was successful
 * @return false  Initialization failed
 * 
 * @warning The addresses showed in the table 32 of the 
 *          data-sheet must be right-shifted one bite 
 */
bool SC16IS7X0::begin_I2C(uint8_t addr, TwoWire *theWire)
{
  return setBusIo(SC16IS7X0_BusIo::buildI2C(addr, theWire));
}

/**
 * @brief Initialize baudrate generator and serial format
 *
 * @param baudrate Desired baudrate
 * @param config Serial configuration
 */
void SC16IS7X0::begin_UART(unsigned long baudrate, SerialConfig config)
{
  // Enable enhanced function to be able to change the clock prescaler
  enableEnhancedFunctions();
  // Enable embedded 64 bytes FIFO for RX and TX
  enableFIFO();
  // Enable Transmission Control Register to be able to use RTS (if desired).
  // Warning, Trigger Level Register is also enabled for interruption by this
  // function. As all bits in Trigger Level Register are cleared on POR, values
  // from FCR Register are used instead. Selectable RX and TX Trigger Level from
  // FCR Register are then by default of 8 characters
  enableTCR_TLR();

  // Read SerialConfig
  uint8_t wordLength = getWordLength(config);
  uint8_t stopBits = getStopBits(config);
  uint8_t parity = getParity(config);

  // Create LCR Register value
  _lcr = 0x00;
  _lcr = (parity | stopBits | wordLength);

  // Write Register
  uint8_t request[2] = {SC16IS7X0_LCR << 3, _lcr};
  busIo->write(request, 2);

  updateBaudRate(baudrate);
}

/**
 * @brief Update baudrate
 *
 * @param baudrate new baudrate value in Hz. max 5MHz
 */
void SC16IS7X0::updateBaudRate(unsigned long baudrate)
{
  assert(baudrate > 0 && baudrate <= 5000000);

  /**
   * @brief Divisor formula
   * Divisor = (XTAL1 freq / prescaler) / (Desired baudrate x 16)
   */

  uint32_t xtalFreqDivBy4 = _xtalFreq >> 2;
  uint32_t baudrateClock = baudrate << 4;

  // Calculate divisor and remainder for both prescaler
  uint32_t divisor1 = _xtalFreq / baudrateClock;
  uint32_t remainder1 = _xtalFreq % baudrateClock;
  uint32_t divisor4 = xtalFreqDivBy4 / baudrateClock;
  uint32_t remainder4 = xtalFreqDivBy4 % baudrateClock;

  // Serial.print("baudrate update ");
  // Serial.print(baudrate);
  // Serial.print(" ");
  // Serial.print(divisor1);
  // Serial.print(" ");
  // Serial.print(remainder1);
  // Serial.print(" ");
  // Serial.print(divisor4);
  // Serial.print(" ");
  // Serial.println(remainder4);

  uint8_t lastTry = false;
  if (divisor1 == 0 || divisor1 > 0xFFFF)
  {
    // Bad divisor
    lastTry = true;
  }
  else if (remainder1 == 0)
  {
    // Perfect match
    writeDivisorAndPrescaler(divisor1, DIVIDE_BY_1);
    return;
  }

  // Second try with /4 prescaler
  if (divisor4 == 0 || divisor4 > 0xFFFF)
  {
    // Bad divisor

    // No other possibility
    if (lastTry)
      return;

    writeDivisorAndPrescaler(divisor1, DIVIDE_BY_1);
    return;
  }

  // Here we are able to choose the divisor with the best remainder
  if (remainder4 < remainder1)
    writeDivisorAndPrescaler(divisor4, DIVIDE_BY_4);
  else
    writeDivisorAndPrescaler(divisor1, DIVIDE_BY_1);

  return;
}

/**
 * @brief Enable enhanced functions
 *
 */
void SC16IS7X0::enableEnhancedFunctions(void)
{
  uint8_t request[2];

  // Set LCR Register to 0xBF to access Enhanced register set
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = 0xBF;
  busIo->write(request, 2);

  // Write EFR Register to enable enhanced functions
  _efr |= (0x01 << 4);
  request[0] = SC16IS7X0_EFR << 3;
  request[1] = _efr;
  busIo->write(request, 2);

  // Set back LCR Register value
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = _lcr;
  busIo->write(request, 2);
}

/**
 * @brief Enable TX and RX 64 bytes FIFO
 *
 */
void SC16IS7X0::enableFIFO(void)
{
  uint8_t request[2];

  request[0] = SC16IS7X0_FCR << 3;
  request[1] = 0x01;
  busIo->write(request, 2);
}

/**
 * @brief Enable internal loopback mode
 *
 */
void SC16IS7X0::enableLoopback(void)
{
  uint8_t request[2];

  _mcr |= 0x01 << 4;

  // Write MCR Register
  request[0] = SC16IS7X0_MCR << 3;
  request[1] = _mcr;
  busIo->write(request, 2);
}

/**
 * @brief Disable internal loopback mode
 *
 */
void SC16IS7X0::disableLoopback(void)
{
  uint8_t request[2];

  _mcr &= ~(0x01 << 4);

  // Write MCR Register
  request[0] = SC16IS7X0_MCR << 3;
  request[1] = _mcr;
  busIo->write(request, 2);
}

/**
 * @brief Enable TCR+TLR register
 *
 */
void SC16IS7X0::enableTCR_TLR(void)
{
  uint8_t request[2];

  _mcr |= 0x01 << 2;

  // Write MCR Register
  request[0] = SC16IS7X0_MCR << 3;
  request[1] = _mcr;
  busIo->write(request, 2);
}

/**
 * @brief Disable TCR+TLR register
 *
 */
void SC16IS7X0::disableTCR_TLR(void)
{
  uint8_t request[2];

  _mcr &= ~(0x01 << 2);

  // Write MCR Register
  request[0] = SC16IS7X0_MCR << 3;
  request[1] = _mcr;
  busIo->write(request, 2);
}

/**
 * @brief Write Divisor Registers and select the desired prescaler
 *
 * @param divisor From 1 to 65535
 * @param prescaler DIVIDE_BY_1 or DIVIDE_BY_4
 */
void SC16IS7X0::writeDivisorAndPrescaler(uint32_t divisor,
                                         Prescaler prescaler)
{
  uint8_t request[2];

  // Change clock divisor bit
  if (prescaler == DIVIDE_BY_1)
  {
    _mcr &= ~(0b10000000);
  }
  else if (prescaler == DIVIDE_BY_4)
  {
    _mcr |= 0b10000000;
  }

  // Write MCR Register
  request[0] = SC16IS7X0_MCR << 3;
  request[1] = _mcr;
  busIo->write(request, 2);

  // Set LCR[7] bit to write to special registers LDD and LDH
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = _lcr | 0x80;
  busIo->write(request, 2);

  // Write the most significant part of the divisor
  request[0] = SC16IS7X0_DLH << 3;
  request[1] = (divisor >> 8) & 0xFF;
  busIo->write(request, 2);

  // Write the least significant part of the divisor
  request[0] = SC16IS7X0_DLL << 3;
  request[1] = divisor & 0xFF;
  busIo->write(request, 2);

  // Reset LCR[7] bit
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = _lcr;
  busIo->write(request, 2);
}

/**
 * @brief Write single byte into the TX FIFO
 *
 * @param c byte to send
 * @return size_t Return 1 if the byte has been pushed into the FIFO otherwise 0
 * if the FIFO was full.
 */
size_t SC16IS7X0::write(uint8_t c)
{
  uint8_t free = txlvl();
  if (free < 1)
    return 0;

  uint8_t request[2] = {(SC16IS7X0_THR << 3), c};
  busIo->write(request, 2);

  return 1;
}

/**
 * @brief Write one or more bytes into the TX FIFO
 *
 * @param buffer Buffer pointer
 * @param size size of data to transmit
 * @return size_t Return the number of bytes pushed into the FIFO. Can be less
 * than size.
 */
size_t SC16IS7X0::write(const uint8_t *buffer, size_t size)
{
  size_t free = (size_t)txlvl();
  if (free < size)
    size = free;

  if (size == 0)
    return 0;

  uint8_t request[1] = {SC16IS7X0_THR << 3};
  busIo->write(buffer, size, request, 1);
  return size;
}

/**
 * @brief Read TXLVL Register
 *
 * @return uint8_t
 */
uint8_t SC16IS7X0::txlvl(void)
{
  uint8_t request[1] = {(SC16IS7X0_TXLVL << 3) | SC16IS7X0_READ_FLAG};
  uint8_t txlvl;
  busIo->write_then_read(request, 1, &txlvl, 1);
  return txlvl;
}

bool SC16IS7X0::setBusIo(SC16IS7X0_BusIo *theBusIo)
{
  if (busIo)
    delete busIo; // delete old instance
  busIo = theBusIo;

  if (busIo)
    return true;
  else
    return false;
}

uint8_t SC16IS7X0::getWordLength(SerialConfig config)
{
  uint8_t wordLength;

  // Word Length
  switch (config & UART_NB_BIT_MASK)
  {
  case UART_NB_BIT_5:
    wordLength = 0x00;
    break;

  case UART_NB_BIT_6:
    wordLength = 0x01;
    break;

  case UART_NB_BIT_7:
    wordLength = 0x02;
    break;

  case UART_NB_BIT_8:
    wordLength = 0x03;
    break;
  }

  return wordLength;
}

uint8_t SC16IS7X0::getParity(SerialConfig config)
{
  uint8_t parity;

  switch (config & UART_PARITY_MASK)
  {
  case UART_PARITY_EVEN:
    parity = 0x18;
    break;

  case UART_PARITY_ODD:
    parity = 0x08;
    break;

  case UART_PARITY_NONE:
    parity = 0x00;
    break;
  }

  return parity;
}

uint8_t SC16IS7X0::getStopBits(SerialConfig config)
{
  uint8_t stopBits = 0x00;

  // Stop bits
  switch (config & UART_NB_STOP_BIT_MASK)
  {
#ifdef ESP8266
  case UART_NB_STOP_BIT_0:
    // Not applicable
    break;

  case UART_NB_STOP_BIT_15:
    // Work only with word length 5
    stopBits = 0x04;
    break;
#endif
  case UART_NB_STOP_BIT_1:
    // Work only with word length 5, 6, 7, 8
    stopBits = 0x00;
    break;

  case UART_NB_STOP_BIT_2:
    // Work only with word length 6, 7, 8
    stopBits = 0x04;
    break;
  }

  return stopBits;
}

/**
 * @brief Return the available char in RX FIFO
 *
 * @return int
 */
int SC16IS7X0::available(void)
{
  uint8_t request[1] = {(SC16IS7X0_RXLVL << 3) | SC16IS7X0_READ_FLAG};
  uint8_t rxlvl;
  busIo->write_then_read(request, 1, &rxlvl, 1);
  return (int)rxlvl;
}

/**
 * @brief Return a single character
 *
 * @return int
 */
int SC16IS7X0::read(void)
{
  if (available() == 0)
    return -1;

  uint8_t request[1] = {(SC16IS7X0_RHR << 3) | SC16IS7X0_READ_FLAG};
  uint8_t val;
  busIo->write_then_read(request, 1, &val, 1);
  return (int)val;
}

/**
 * @brief Read one or more bytes from the RX FIFO
 *
 * @param buffer Destination pointer to transfert RX bytes
 * @param len Number of bytes to read
 * @return int Return the real size of data that has been read
 */
#ifndef ESP32
int SC16IS7X0::read(uint8_t *buffer, size_t len)
#else
size_t SC16IS7X0::readBytes(uint8_t *buffer, size_t len)
#endif
{
  size_t a = (size_t)available();
  if (a < len)
    len = a;

  if (len == 0)
    return 0;

  uint8_t request[1] = {(SC16IS7X0_RHR << 3) | SC16IS7X0_READ_FLAG};
  busIo->write_then_read(request, 1, buffer, len);
  return (int)len;
}

/**
 * @brief Check for overrun
 *
 * @return true overrun has occured
 * @return false no overrun
 */
bool SC16IS7X0::hasOverrun(void)
{
  uint8_t request[1] = {(SC16IS7X0_LSR << 3) | SC16IS7X0_READ_FLAG};
  uint8_t lsr;
  busIo->write_then_read(request, 1, &lsr, 1);

  return (lsr & 0x02) ? true : false;
}

/**
 * @brief Check for error on reception
 *
 * @return true At least one parity error, framing error, or break indication is
 * in the receiver FIFO
 * @return false No error in FIFO
 */
bool SC16IS7X0::hasRxError(void)
{
  uint8_t request[1] = {(SC16IS7X0_LSR << 3) | SC16IS7X0_READ_FLAG};
  uint8_t lsr;
  busIo->write_then_read(request, 1, &lsr, 1);

  return (lsr & 0x80) ? true : false;
}

/**
 * @brief Enable Hardware CTS Flow control
 * Transmission will stop when a high signal is detected on the CTS pin
 *
 */
void SC16IS7X0::enableHardwareCTS(void)
{
  uint8_t request[2];

  // Set LCR Register to 0xBF to access Enhanced register set
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = 0xBF;
  busIo->write(request, 2);

  // Set EFR[7] to enable Hardware CTS
  _efr |= (0x01 << 7);
  request[0] = SC16IS7X0_EFR << 3;
  request[1] = _efr;
  busIo->write(request, 2);

  // Set back LCR Register value
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = _lcr;
  busIo->write(request, 2);
}

/**
 * @brief Disable Hardware CTS Flow Control
 *
 */
void SC16IS7X0::disableHardwareCTS(void)
{
  uint8_t request[2];

  // Set LCR Register to 0xBF to access Enhanced register set
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = 0xBF;
  busIo->write(request, 2);

  // Reset EFR[7] to enable Hardware CTS
  _efr &= ~(0x01 << 7);
  request[0] = SC16IS7X0_EFR << 3;
  request[1] = _efr;
  busIo->write(request, 2);

  // Set back LCR Register value
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = _lcr;
  busIo->write(request, 2);
}

/**
 * @brief Enable Hardware RTS Flow control
 * RTS pin goes high when the receiver FIFO 'halt trigger level' is reached
 * RTS pin goes low when the 'resume transmission trigger level' is reached
 *
 */
void SC16IS7X0::enableHardwareRTS(void)
{
  uint8_t request[2];

  // TCR[7:4] Trigger level to resume transmission (set to 2h = 2x4 -> 8
  // characters) TCR[3:0] Trigger level to halt transmission (set to Ah = 10x4
  // -> 40 characters) TCR[3:0] must be > TCR[7:4]
  request[0] = SC16IS7X0_TCR << 3;
  request[1] = 0x2A;
  busIo->write(request, 2);

  // Set LCR Register to 0xBF to access Enhanced register set
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = 0xBF;
  busIo->write(request, 2);

  // Set EFR[6] to enable Hardware RTS
  _efr |= (0x01 << 6);
  request[0] = SC16IS7X0_EFR << 3;
  request[1] = _efr;
  busIo->write(request, 2);

  // Set back LCR Register value
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = _lcr;
  busIo->write(request, 2);
}

/**
 * @brief Disable Hardware RTS Flow Control
 *
 */
void SC16IS7X0::disableHardwareRTS(void)
{
  uint8_t request[2];

  // Set LCR Register to 0xBF to access Enhanced register set
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = 0xBF;
  busIo->write(request, 2);

  // Reset EFR[6] to enable Hardware RTS
  _efr &= ~(0x01 << 6);
  request[0] = SC16IS7X0_EFR << 3;
  request[1] = _efr;
  busIo->write(request, 2);

  // Set back LCR Register value
  request[0] = SC16IS7X0_LCR << 3;
  request[1] = _lcr;
  busIo->write(request, 2);
}

/**
 * @brief Set pin as Input or Output
 *
 * @param pin pin number from 0 to 7
 * @param mode INPUT or OUTPUT
 */
void SC16IS7X0::pinMode(uint8_t pin, uint8_t mode)
{
  assert(pin <= 7);
  assert(mode == INPUT || mode == OUTPUT);

  switch (mode)
  {
  case OUTPUT:
    _ioDir |= 0x01 << pin;
    break;

  case INPUT:
    _ioDir &= ~(0x01 << pin);
    break;

  default:
    return;
  }

  // Write IoDir Register
  const uint8_t request[2] = {(SC16IS7X0_IODIR << 3), _ioDir};
  busIo->write(request, 2);
}

/**
 * @brief Write state of output pin
 *
 * @param pin Pin number from 0 to 7
 * @param val 0=low, other value = high
 */
void SC16IS7X0::digitalWrite(uint8_t pin, uint8_t val)
{
  assert(pin <= 7);

  if (val != 0)
    _ioState |= 0x01 << pin;
  else
    _ioState &= ~(0x01 << pin);

  // Write IoState Register
  const uint8_t request[2] = {(SC16IS7X0_IOSTATE << 3), _ioState};
  busIo->write(request, 2);
}

/**
 * @brief Read digital input
 *
 * @param pin Pin number from 0 to 7
 * @return int 0=input low, 1=input high
 */
int SC16IS7X0::digitalRead(uint8_t pin)
{
  assert(pin <= 7);

  const uint8_t request[2] = {(SC16IS7X0_IOSTATE << 3) | SC16IS7X0_READ_FLAG};
  uint8_t data;
  busIo->write_then_read(request, 1, &data, 1);
  return data & (0x01 << pin) ? 1 : 0;
}