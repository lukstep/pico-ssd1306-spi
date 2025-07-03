#include <cstdio>
#include <cstring>
#include <pico/stdio.h>
#include <pico/time.h>
#include "ssd1306.hpp"

int32_t getFPS()
{
    static int32_t ms = 0;
    int32_t ms_last = ms;
    ms = us_to_ms(get_absolute_time());
    return round(1000.0f / (static_cast<float>(ms) - (static_cast<float>(ms_last))));
}

template<int32_t X, int32_t Y, int32_t BAR_HEIGHT = 20>
void drawProgressBar(SSD1306::OledDisplay<128, 64, true>& display, int32_t proc)
{
    constexpr int32_t CHARACTER_SPACE = 2;
    int32_t barWidth = display.width() - 2 * (X + CHARACTER_SPACE * 2);
    int32_t barPorcWidth = static_cast<int32_t>((barWidth * proc) * 0.01f);

    display.drawRect(X, Y, display.width() - 2 * X, BAR_HEIGHT);
    display.fillRect(X + CHARACTER_SPACE, Y + CHARACTER_SPACE, barPorcWidth,
                     BAR_HEIGHT - 2 * CHARACTER_SPACE);
}

int main()
{
    stdio_init_all();

    SSD1306::OledDisplay<128, 64, true> display;
    int32_t fps = 0;

    int32_t proc = 0;
    char text[20];

    memset(text, 0, sizeof(text));
    while(true)
    {
        display.clear();
        fps = getFPS();

        display.drawText(40, 5, "Loading...", Fonts::FontType::FONT5X8);
        drawProgressBar<10, 20>(display, proc);

        snprintf(text, sizeof(text), "%d%%   ", proc);
        display.drawText(56, 42, text, Fonts::FontType::FONT5X8);

        proc += 1;
        if(proc > 100)
        {
            proc = 0;
        }

        sleep_us(100000);
        display.display();
    }
}