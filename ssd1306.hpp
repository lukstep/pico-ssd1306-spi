#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <boards/pico_w.h>
#include <cstdint>
#include <pico/types.h>
#include <stddef.h>
#include <cstring>
#include <sys/cdefs.h>
#include <math.h>

#include "font5x7.hpp"

constexpr uint8_t SSD1306_COLUMNADDR = 0x21;
constexpr uint8_t SSD1306_PAGEADDR = 0x22;
constexpr uint8_t SSD1306_DISPLAYOFF = 0xAE;
constexpr uint8_t SSD1306_DISPLAYON = 0xAF;
constexpr uint8_t SSD1306_SETDISPLAYCLOCKDIV = 0xD5;
constexpr uint8_t SSD1306_SETMULTIPLEX = 0xA8;
constexpr uint8_t SSD1306_SETDISPLAYOFFSET = 0xD3;
constexpr uint8_t SSD1306_SETSTARTLINE = 0x40;
constexpr uint8_t SSD1306_CHARGEPUMP = 0x8D;
constexpr uint8_t SSD1306_MEMORYMODE = 0x20;
constexpr uint8_t SSD1306_SEGREMAP = 0xA0;
constexpr uint8_t SSD1306_COMSCANDEC = 0xC8;
constexpr uint8_t SSD1306_SETCOMPINS = 0xDA;
constexpr uint8_t SSD1306_SETCONTRAST = 0x81;
constexpr uint8_t SSD1306_SETPRECHARGE = 0xD9;
constexpr uint8_t SSD1306_SETVCOMDETECT = 0xDB;
constexpr uint8_t SSD1306_DISPLAYALLON_RESUME = 0xA4;
constexpr uint8_t SSD1306_NORMALDISPLAY = 0xA6;

template<int32_t WIDTH, int32_t HEIGHT>
class SSD1306
{
  private:
    static constexpr uint8_t FONT_WIDTH = 5;
    static constexpr uint8_t FONT_HEIGHT = 7;
    static constexpr uint8_t CHARACTER_SPACE = 1;

  public:
    SSD1306()
    {
        static_assert(WIDTH > 0 && WIDTH % 8 == 0, "Width must be a multiple of 8");
        static_assert(HEIGHT > 0 && HEIGHT % 8 == 0, "Height must be a multiple of 8");

        spiInitialization();
        clear();

        gpio_put(RST_PIN, 0);
        sleep_ms(10);
        gpio_put(RST_PIN, 1);
        sleep_ms(10);

        uint8_t commands[] = {SSD1306_DISPLAYOFF,
                              SSD1306_SETDISPLAYCLOCKDIV,
                              0x80,
                              SSD1306_SETMULTIPLEX,
                              static_cast<uint8_t>(HEIGHT - 1),
                              SSD1306_SETDISPLAYOFFSET,
                              0x00,
                              static_cast<uint8_t>(SSD1306_SETSTARTLINE | 0x00),
                              SSD1306_CHARGEPUMP,
                              0x14,
                              SSD1306_MEMORYMODE,
                              0x00,
                              static_cast<uint8_t>(SSD1306_SEGREMAP | 0x1),
                              SSD1306_COMSCANDEC,
                              SSD1306_SETCOMPINS,
                              0x12,
                              SSD1306_SETCONTRAST,
                              0xCF,
                              SSD1306_SETPRECHARGE,
                              0xF1,
                              SSD1306_SETVCOMDETECT,
                              0x40,
                              SSD1306_DISPLAYALLON_RESUME,
                              SSD1306_NORMALDISPLAY,
                              SSD1306_DISPLAYON};
        sendCommands(commands, sizeof(commands));
    }

    void clear()
    {
        memset(buffer, 0x00, sizeof(buffer));
    }

    // void display()
    // {
    //     for (uint8_t page = 0; page < (HEIGHT / 8); ++page) {
    //         uint8_t commands[] = {
    //             SSD1306_PAGEADDR, page, page,
    //             SSD1306_COLUMNADDR, 0x00, static_cast<uint8_t>(WIDTH - 1)
    //         };
    //         sendCommands(commands, sizeof(commands));

    //         sendDataBulk(&buffer[page * WIDTH], WIDTH);
    //     }
    // }

    void display()
    {
        uint8_t commands[] = {SSD1306_COLUMNADDR, 0x00, static_cast<uint8_t>(WIDTH - 1),
                              SSD1306_PAGEADDR,   0x00, static_cast<uint8_t>((HEIGHT / 8) - 1)};
        sendCommands(commands, sizeof(commands));
        sendDataBulk(buffer, sizeof(buffer));
    }

    constexpr int32_t bufferSize() const
    {
        return WIDTH * HEIGHT / 8;
    }

    __always_inline void drawPixel(int32_t x, int32_t y)
    {
        if(x >= WIDTH || y >= HEIGHT)
        {
            return;
        }
        buffer[x + ((y >> 3) * WIDTH)] |= 1 << (y & 7);
    }

    void drawChar(int32_t x, int32_t y, char c)
    {
        if(c < 0 || c > 255)
        {
            return;
        }

        uint16_t index = c * FONT_WIDTH;

        for(int32_t i = 0; i < FONT_WIDTH; ++i)
        {
            uint8_t col = ssd1306_font5x7[index + i];

            for(int32_t j = 0; j < 8; ++j)
            {
                if(col & (1 << j))
                {
                    drawPixel(x + i, y + j);
                }
            }
        }
    }

    void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
    {
        int32_t dx = abs(x1 - x0);
        int32_t sx = x0 < x1 ? 1 : -1;
        int32_t dy = -abs(y1 - y0);
        int32_t sy = y0 < y1 ? 1 : -1;
        int32_t err = dx + dy;

        while(true)
        {
            drawPixel(x0, y0);
            if(x0 == x1 && y0 == y1)
                break;

            int e2 = 2 * err;
            if(e2 >= dy)
            {
                err += dy;
                x0 += sx;
            }
            if(e2 <= dx)
            {
                err += dx;
                y0 += sy;
            }
        }
    }

    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h)
    {
        for(int32_t i = x; i < x + w; i++)
        {
            for(int32_t j = y; j < y + h; j++)
            {
                drawPixel(i, j);
            }
        }
    }

    void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        auto swap = [](int32_t& a, int32_t& b) {
            int32_t t = a;
            a = b;
            b = t;
        };

        if(y0 > y1)
        {
            swap(y0, y1);
            swap(x0, x1);
        }

        if(y1 > y2)
        {
            swap(y1, y2);
            swap(x1, x2);
        }

        if(y0 > y1)
        {
            swap(y0, y1);
            swap(x0, x1);
        }

        int32_t total_height = y2 - y0;

        for(int32_t i = 0; i < total_height; i++)
        {
            bool second_half = i > y1 - y0 || y1 == y0;
            int32_t segment_height = second_half ? y2 - y1 : y1 - y0;
            float alpha = (float)i / total_height;
            float beta = (float)(i - (second_half ? y1 - y0 : 0)) / segment_height;

            int32_t ax = x0 + (x2 - x0) * alpha;
            int32_t bx = second_half ? x1 + (x2 - x1) * beta : x0 + (x1 - x0) * beta;

            if(ax > bx)
            {
                swap(ax, bx);
            }
            for(int32_t j = ax; j <= bx; j++)
            {
                drawPixel(j, y0 + i);
            }
        }
    }

    void drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        drawLine(x0, y0, x1, y1);
        drawLine(x1, y1, x2, y2);
        drawLine(x2, y2, x0, y0);
    }

    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h)
    {
        drawLine(x, y, x + w - 1, y);
        drawLine(x, y + h - 1, x + w - 1, y + h - 1);
        drawLine(x, y, x, y + h - 1);
        drawLine(x + w - 1, y, x + w - 1, y + h - 1);
    }

    void drawCircle(int32_t x0, int32_t y0, int32_t radius)
    {
        int32_t x = radius;
        int32_t y = 0;
        int32_t err = 0;

        while(x >= y)
        {
            drawPixel(x0 + x, y0 + y);
            drawPixel(x0 + y, y0 + x);
            drawPixel(x0 - y, y0 + x);
            drawPixel(x0 - x, y0 + y);
            drawPixel(x0 - x, y0 - y);
            drawPixel(x0 - y, y0 - x);
            drawPixel(x0 + y, y0 - x);
            drawPixel(x0 + x, y0 - y);

            y++;
            if(err <= 0)
            {
                err += 2 * y + 1;
            }
            else
            {
                x--;
                err += 2 * (y - x) + 1;
            }
        }
    }

    void drawText(int32_t x, int32_t y, const char* text)
    {
        while(*text)
        {
            drawChar(x, y, *text++);
            x += FONT_WIDTH + CHARACTER_SPACE;
        }
    }

  private:
    static constexpr int32_t CS_PIN = PICO_DEFAULT_SPI_CSN_PIN;
    static constexpr int32_t CLK_PIN = PICO_DEFAULT_SPI_SCK_PIN;
    static constexpr int32_t SDI_PIN = PICO_DEFAULT_SPI_TX_PIN;
    static constexpr int32_t SDO_PIN = PICO_DEFAULT_SPI_RX_PIN;
    static constexpr int32_t DC_PIN = 20;
    static constexpr int32_t RST_PIN = 21;
    uint8_t buffer[WIDTH * HEIGHT / 8];

    void spiInitialization() const
    {
        gpio_init(CS_PIN);
        gpio_set_dir(CS_PIN, GPIO_OUT);
        csDeselect();

        spi_init(spi_default, 10'000'000);

        gpio_set_function(CLK_PIN, GPIO_FUNC_SPI);
        gpio_set_function(SDI_PIN, GPIO_FUNC_SPI);
        gpio_set_function(SDO_PIN, GPIO_FUNC_SPI);

        gpio_init(DC_PIN);
        gpio_set_dir(DC_PIN, GPIO_OUT);

        gpio_init(RST_PIN);
        gpio_set_dir(RST_PIN, GPIO_OUT);
    }

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

    inline void spiWrite(uint8_t* data) const
    {
        csSelect();
        spi_write_blocking(spi_default, data, 1);
        csDeselect();
    }

    inline void spiBulkWrite(uint8_t* data, size_t size) const
    {
        csSelect();
        spi_write_blocking(spi_default, data, size);
        csDeselect();
    }

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
};