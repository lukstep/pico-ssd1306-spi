#include <cstdio>
#include <cstring>
#include <pico/stdio.h>
#include <pico/time.h>
#include <vector>
#include "ssd1306.hpp"

int32_t getFPS()
{
    static int32_t ms = 0;
    int32_t ms_last = ms;
    ms = us_to_ms(get_absolute_time());
    return round(1000.0f / (static_cast<float>(ms) - (static_cast<float>(ms_last))));
}

std::string lorenIpsum =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut "
    "labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco "
    "laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in "
    "voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat "
    "cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

int main()
{
    stdio_init_all();

    SSD1306::OledDisplay<128, 64, true> display;
    int32_t fps = 0;

    char i = 0;
    std::vector<Fonts::FontType> fonts = {Fonts::FontType::FONT5X7, Fonts::FontType::FONT5X8,
                                          Fonts::FontType::FONT6X8, Fonts::FontType::FONT8X8};

    while(true)
    {
        display.clear();
        fps = getFPS();

        auto font = fonts[i % fonts.size()];
        i++;
        display.drawTextWithWrap(0, 0, lorenIpsum, font);

        sleep_us(1000000);
        display.display();
    }
}