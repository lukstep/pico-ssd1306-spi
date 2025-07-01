#include <cstdio>
#include <pico/stdio.h>
#include <pico/time.h>
#include "ssd1306.hpp"

#include <string>

int32_t getFPS()
{
    static int32_t ms = 0;
    int32_t ms_last = ms;
    ms = us_to_ms(get_absolute_time());
    return round(1000.0f / (static_cast<float>(ms) - (static_cast<float>(ms_last))));
}

int main()
{
    stdio_init_all();

    SSD1306::OledDisplay<128, 64, true> display;
    int32_t fps = 0;

    while(true)
    {
        display.clear();
        fps = getFPS();
        std::string fps_text = "FPS: " + std::to_string(fps);
        display.drawText(0, 10, fps_text);
        display.drawText(100, 5, "XYY");
        display.drawText(5, 57, "YYY");
        display.drawLine(5, 63, 120, 5);
        display.drawRect(15, 50, 10, 10);
        display.fillRect(100, 32, 30, 10);
        display.drawCircle(64, 32, 5);
        display.fillTriangle(70, 50, 80, 50, 75, 40);
        display.display();
    }
}