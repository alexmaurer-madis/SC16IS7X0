#include "SC16IS7X0_BusIo.h"

SC16IS7X0_I2C::SC16IS7X0_I2C(uint8_t addr, TwoWire *theWire)
{
    i2c = new Adafruit_I2CDevice(addr, theWire);
    i2c->begin(false);
}

SC16IS7X0_I2C::~SC16IS7X0_I2C()
{
    delete i2c;
}

bool SC16IS7X0_I2C::read(uint8_t *buffer, size_t len)
{
    if (!i2c)
        return false;
    return i2c->read(buffer, len);
}

bool SC16IS7X0_I2C::write(const uint8_t *buffer,
                          size_t len,
                          const uint8_t *prefix_buffer,
                          size_t prefix_len)
{
    if (!i2c)
        return false;
    return i2c->write(buffer, len, true, prefix_buffer, prefix_len);
}

bool SC16IS7X0_I2C::write_then_read(const uint8_t *write_buffer,
                                    size_t write_len,
                                    uint8_t *read_buffer,
                                    size_t read_len)
{
    if (!i2c)
        return false;
    return i2c->write_then_read(write_buffer, write_len, read_buffer, read_len);
}

SC16IS7X0_SPI::SC16IS7X0_SPI(int8_t cspin,
                             uint32_t freq,
                             BusIOBitOrder dataOrder,
                             uint8_t dataMode,
                             SPIClass *theSPI)
{
    spi = new Adafruit_SPIDevice(cspin, freq, dataOrder, dataMode, theSPI);
    spi->begin();
}

SC16IS7X0_SPI::~SC16IS7X0_SPI()
{
    delete spi;
}

bool SC16IS7X0_SPI::read(uint8_t *buffer, size_t len)
{
    if (!spi)
        return false;
    return spi->read(buffer, len);
}

bool SC16IS7X0_SPI::write(const uint8_t *buffer,
                          size_t len,
                          const uint8_t *prefix_buffer,
                          size_t prefix_len)
{
    if (!spi)
        return false;
    return spi->write(buffer, len, prefix_buffer, prefix_len);
}

bool SC16IS7X0_SPI::write_then_read(const uint8_t *write_buffer,
                                    size_t write_len,
                                    uint8_t *read_buffer,
                                    size_t read_len)
{
    if (!spi)
        return false;
    return spi->write_then_read(write_buffer, write_len, read_buffer, read_len);
}

SC16IS7X0_BusIo *SC16IS7X0_BusIo::buildSPI(int8_t cspin, uint32_t freq, BusIOBitOrder dataOrder, uint8_t dataMode, SPIClass *theSPI)
{
    return new SC16IS7X0_SPI(cspin, freq, dataOrder, dataMode, theSPI);
}

SC16IS7X0_BusIo *SC16IS7X0_BusIo::buildI2C(uint8_t addr, TwoWire *theWire)
{
    return new SC16IS7X0_I2C(addr, theWire);
}
