#pragma once

#include <Adafruit_I2CDevice.h>
#include <Adafruit_SPIDevice.h>

class SC16IS7X0_I2C;
class SC16IS7X0_SPI;

class SC16IS7X0_BusIo
{
public:
    enum ioBus
    {
        SPI_BUS,
        I2C_BUS
    };

    virtual ~SC16IS7X0_BusIo(){};
    virtual bool read(uint8_t *buffer, size_t len) = 0;
    virtual bool write(const uint8_t *buffer, size_t len,
                       const uint8_t *prefix_buffer = nullptr, size_t prefix_len = 0) = 0;
    virtual bool write_then_read(const uint8_t *write_buffer, size_t write_len,
                                 uint8_t *read_buffer, size_t read_len) = 0;

    static SC16IS7X0_BusIo *buildSPI(int8_t cspin, uint32_t freq = 4000000,
                                      BusIOBitOrder dataOrder = SPI_BITORDER_MSBFIRST,
                                      uint8_t dataMode = SPI_MODE0, SPIClass *theSPI = &SPI);
    static SC16IS7X0_BusIo *buildI2C(uint8_t addr, TwoWire *theWire = &Wire);
};

class SC16IS7X0_I2C : public SC16IS7X0_BusIo
{
private:
    Adafruit_I2CDevice *i2c = nullptr;

    friend SC16IS7X0_BusIo;

    // to avoid direct instantiation
    SC16IS7X0_I2C(uint8_t addr, TwoWire *theWire);

public:
    ~SC16IS7X0_I2C();
    bool read(uint8_t *buffer, size_t len) override;
    bool write(const uint8_t *buffer, size_t len,
               const uint8_t *prefix_buffer = nullptr, size_t prefix_len = 0) override;
    bool write_then_read(const uint8_t *write_buffer, size_t write_len,
                         uint8_t *read_buffer, size_t read_len) override;
};

class SC16IS7X0_SPI : public SC16IS7X0_BusIo
{
private:
    Adafruit_SPIDevice *spi = nullptr;

    friend SC16IS7X0_BusIo;
    // to avoid direct instantiation
    SC16IS7X0_SPI(int8_t cspin, uint32_t freq,
                  BusIOBitOrder dataOrder,
                  uint8_t dataMode, SPIClass *theSPI);

public:
    ~SC16IS7X0_SPI();
    bool read(uint8_t *buffer, size_t len) override;
    bool write(const uint8_t *buffer, size_t len,
               const uint8_t *prefix_buffer = nullptr, size_t prefix_len = 0) override;
    bool write_then_read(const uint8_t *write_buffer, size_t write_len,
                         uint8_t *read_buffer, size_t read_len) override;
};