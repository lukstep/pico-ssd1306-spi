#include <cstdio>
#include <pico/stdio.h>
#include <pico/time.h>
#include "ssd1306.hpp"

int main()
{
    stdio_init_all();
    SSD1306::OledDisplay<128, 64, true> display;

    char text[20];
    while(true)
    {
        for(int i = 0; i < 255; i++)
        {
            display.clear();
            sprintf(&text[0], "%d:%c", i, i);
            display.drawText(5, 5, text);
            display.drawLine(0, 63, 127, 0);
            display.drawRect(15, 15, 10, 10);
            display.fillRect(60, 40, 10, 10);
            display.drawCircle(64, 32, 5);
            display.fillTriangle(70, 50, 80, 50, 75, 40);
            display.display();
            sleep_ms(500);
        }
    }
}