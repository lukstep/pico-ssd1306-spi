#pragma once

#include <boards/pico_w.h>
#include <cstdint>
#include <pico/types.h>
#include <stddef.h>
#include <cstring>
#include <sys/cdefs.h>
#include <math.h>
#include <string>

#include "fonts.hpp"

#include "ssd1306_hw_driver.hpp"

namespace SSD1306
{

template<int32_t WIDTH, int32_t HEIGHT, bool FLIP_DIRECTION = false, bool INVERTED = false>
class OledDisplay
{
  private:
    static constexpr uint8_t SSD1306_COLUMNADDR = 0x21;
    static constexpr uint8_t SSD1306_PAGEADDR = 0x22;
    static constexpr uint8_t SSD1306_DISPLAYOFF = 0xAE;
    static constexpr uint8_t SSD1306_DISPLAYON = 0xAF;
    static constexpr uint8_t SSD1306_SETDISPLAYCLOCKDIV = 0xD5;
    static constexpr uint8_t SSD1306_SETMULTIPLEX = 0xA8;
    static constexpr uint8_t SSD1306_SETDISPLAYOFFSET = 0xD3;
    static constexpr uint8_t SSD1306_SETSTARTLINE = 0x40;
    static constexpr uint8_t SSD1306_CHARGEPUMP = 0x8D;
    static constexpr uint8_t SSD1306_MEMORYMODE = 0x20;
    static constexpr uint8_t SSD1306_SEGREMAP = 0xA0;
    static constexpr uint8_t SSD1306_COMSCANDEC = 0xC0;
    static constexpr uint8_t SSD1306_SETCOMPINS = 0xDA;
    static constexpr uint8_t SSD1306_SETCONTRAST = 0x81;
    static constexpr uint8_t SSD1306_SETPRECHARGE = 0xD9;
    static constexpr uint8_t SSD1306_SETVCOMDETECT = 0xDB;
    static constexpr uint8_t SSD1306_DISPLAYALLON_RESUME = 0xA4;
    static constexpr uint8_t SSD1306_NORMALDISPLAY = 0xA6;
    static constexpr uint8_t SSD1306_INVERTDISPLAY = 0xA7;

    enum class MEMORY_ADDRESSING_MODE
    {
        HORIZONTAL = 0x00,
        VERTICAL = 0x01,
        PAGE = 0x02
    };

    enum class SCAN_DIRECTION
    {
        NORMAL = 0x00,
        FLIPPED = 0x08
    };

    void drawChar(int32_t x, int32_t y, char c, FontBase* fontData)
    {
        if(c < 0 || c > 255)
        {
            return;
        }

        uint16_t index = (c - fontData->characterOffset()) * fontData->width();

        for(int32_t i = 0; i < fontData->width(); ++i)
        {
            uint8_t col = fontData->getFontData()[index + i];

            for(int32_t j = 0; j < fontData->height(); ++j)
            {
                if(col & (1 << j))
                {
                    drawPixel(x + i, y + j);
                }
            }
        }
    }

  public:
    OledDisplay()
    {
        static_assert(WIDTH > 0 && WIDTH % 8 == 0, "Width must be a multiple of 8");
        static_assert(HEIGHT > 0 && HEIGHT % 8 == 0, "Height must be a multiple of 8");

        hwInterface.initialize();
        clear();

        hwInterface.reset();

        uint8_t mode = INVERTED ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY;

        uint8_t segremapValue = 0x00;
        uint8_t scanDirection = 0x00;
        if constexpr(FLIP_DIRECTION)
        {
            segremapValue = SSD1306_SEGREMAP | 0x0;
            scanDirection = SSD1306_COMSCANDEC | static_cast<uint8_t>(SCAN_DIRECTION::NORMAL);
        }
        else
        {
            segremapValue = SSD1306_SEGREMAP | 0x1;
            scanDirection = SSD1306_COMSCANDEC | static_cast<uint8_t>(SCAN_DIRECTION::FLIPPED);
        }

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
                              static_cast<uint8_t>(MEMORY_ADDRESSING_MODE::HORIZONTAL),
                              segremapValue,
                              scanDirection,
                              SSD1306_SETCOMPINS,
                              0x12,
                              SSD1306_SETCONTRAST,
                              0x00,
                              SSD1306_SETPRECHARGE,
                              0xF1,
                              SSD1306_SETVCOMDETECT,
                              0x40,
                              SSD1306_DISPLAYALLON_RESUME,
                              mode,
                              SSD1306_DISPLAYON};
        hwInterface.sendCommands(commands, sizeof(commands));
    }

    constexpr int32_t width() const
    {
        return WIDTH;
    }

    constexpr int32_t height() const
    {
        return HEIGHT;
    }

    void clear()
    {
        memset(buffer, 0x00, sizeof(buffer));
    }

    void display()
    {
        uint8_t commands[] = {SSD1306_COLUMNADDR, 0x00, static_cast<uint8_t>(WIDTH - 1),
                              SSD1306_PAGEADDR,   0x00, static_cast<uint8_t>((HEIGHT / 8) - 1)};
        hwInterface.sendCommands(commands, sizeof(commands));
        hwInterface.sendDataBulk(buffer, sizeof(buffer));
    }

    __always_inline void drawPixel(int32_t x, int32_t y)
    {
        if(x >= WIDTH || y >= HEIGHT)
        {
            return;
        }
        buffer[x + ((y >> 3) * WIDTH)] |= 1 << (y & 7);
    }

    void drawChar(int32_t x, int32_t y, char c, Fonts::FontType font = Fonts::FontType::FONT5X8)
    {
        auto fontData = getFont(font);
        drawChar(x, y, c, fontData);
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

    template<typename StringType>
    void drawText(int32_t x, int32_t y, const StringType& text,
                  Fonts::FontType font = Fonts::FontType::FONT5X8)
    {
        auto fontData = getFont(font);
        for(auto c: text)
        {
            if(c == '\0')
            {
                break;
            }
            drawChar(x, y, c, fontData);
            x += fontData->width() + fontData->characterSpace();
        }
    }

    template<typename StringType>
    void drawTextWithWrap(int32_t x, int32_t y, const StringType& text,
                          Fonts::FontType font = Fonts::FontType::FONT5X8)
    {
        auto fontData = getFont(font);
        for(auto c: text)
        {
            if(c == '\0')
            {
                break;
            }
            drawChar(x, y, c, fontData);
            x += fontData->width() + fontData->characterSpace();
            if(x + fontData->width() > WIDTH)
            {
                x = 0;
                y += fontData->height() + 1;
            }
        }
    }

    void drawBitmap(int x, int y, const uint8_t* bitmap, int w, int h)
    {
        for(int j = 0; j < h; j++)
        {
            for(int i = 0; i < w; i++)
            {
                int byteIndex = i + (j / 8) * w;
                if(bitmap[byteIndex] & (1 << (j % 8)))
                {
                    drawPixel(x + i, y + j);
                }
            }
        }
    }

    void drawBitmapHorizontal(int x0, int y0, const uint8_t* bitmap, int width, int height)
    {
        int bytesPerRow = (width + 7) / 8;

        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                int byteIndex = y * bytesPerRow + (x / 8);
                uint8_t bit = bitmap[byteIndex] & (0x80 >> (x % 8));

                if(bit)
                    drawPixel(x0 + x, y0 + y);
            }
        }
    }

  private:
    SSD1306::HardwareInterfaceBase& hwInterface = *new SSD1306::SPIInterface();
    uint8_t buffer[WIDTH * HEIGHT / 8];
};
} // namespace SSD1306
