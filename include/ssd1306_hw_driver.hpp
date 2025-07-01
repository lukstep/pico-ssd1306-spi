#pragma once

#include <pico/stdlib.h>
#include <pico/types.h>
#include <hardware/gpio.h>
#include <hardware/spi.h>

namespace SSD1306
{
class HardwareInterfaceBase
{
  public:
    HardwareInterfaceBase() = default;
    ~HardwareInterfaceBase() = default;

    virtual void initialize() = 0;
    virtual void sendCommand(uint8_t command) const = 0;
    virtual void sendCommands(uint8_t* commands, size_t size) const = 0;
    virtual void sendData(uint8_t data) const = 0;
    virtual void sendDataBulk(uint8_t* data, size_t size) const = 0;
    virtual void reset() const = 0;
};

class SPIInterface : public HardwareInterfaceBase
{
  public:
    void initialize() override;

    inline void sendCommand(uint8_t command) const
    {
        commandTransfer();
        spiWrite(&command);
    }

    inline void sendCommands(uint8_t* commands, size_t size) const
    {
        commandTransfer();
        spiBulkWrite(commands, size);
    }

    inline void sendData(uint8_t data) const
    {
        dataTransfer();
        spiWrite(&data);
    }

    inline void sendDataBulk(uint8_t* data, size_t size) const
    {
        dataTransfer();
        spiBulkWrite(data, size);
    }

    inline void reset() const
    {
        gpio_put(RST_PIN, 0);
        sleep_ms(10);
        gpio_put(RST_PIN, 1);
        sleep_ms(10);
    }

  private:
    static constexpr int32_t CS_PIN = PICO_DEFAULT_SPI_CSN_PIN;
    static constexpr int32_t CLK_PIN = PICO_DEFAULT_SPI_SCK_PIN;
    static constexpr int32_t SDI_PIN = PICO_DEFAULT_SPI_TX_PIN;
    static constexpr int32_t SDO_PIN = PICO_DEFAULT_SPI_RX_PIN;
    static constexpr int32_t DC_PIN = 20;
    static constexpr int32_t RST_PIN = 21;

    inline void csSelect() const
    {
        asm volatile("nop \n nop \n nop \n nop");
        gpio_put(CS_PIN, 0); // Active low
        asm volatile("nop \n nop \n nop \n nop");
    }

    inline void csDeselect() const
    {
        asm volatile("nop \n nop \n nop \n nop");
        gpio_put(CS_PIN, 1);
        asm volatile("nop \n nop \n nop \n nop");
    }

    inline void spiWrite(uint8_t* data) const
    {
        csSelect();
        spi_write_blocking(spi_default, data, 1);
        csDeselect();
    }

    inline void dataTransfer() const
    {
        gpio_put(DC_PIN, 1);
        sleep_us(1);
    }

    inline void commandTransfer() const
    {
        gpio_put(DC_PIN, 0);
        sleep_us(1);
    }

    inline void spiBulkWrite(uint8_t* data, size_t size) const
    {
        csSelect();
        spi_write_blocking(spi_default, data, size);
        csDeselect();
    }
};
} // namespace SSD1306