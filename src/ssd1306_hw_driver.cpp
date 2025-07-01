#include "ssd1306_hw_driver.hpp"

namespace SSD1306
{
void SPIInterface::initialize()
{
    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    csDeselect();

    constexpr int32_t SPI_BAUDRATE = 10'000'000;
    spi_init(spi_default, SPI_BAUDRATE);

    gpio_set_function(CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SDI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SDO_PIN, GPIO_FUNC_SPI);

    gpio_init(DC_PIN);
    gpio_set_dir(DC_PIN, GPIO_OUT);

    gpio_init(RST_PIN);
    gpio_set_dir(RST_PIN, GPIO_OUT);
}
} // namespace SSD1306