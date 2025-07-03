#pragma once
#include "font5x7.hpp"
#include "font5x8.hpp"
#include "font6x8.hpp"
#include "font8x8.hpp"

namespace Fonts
{

enum class FontType
{
    FONT5X7,
    FONT5X8,
    FONT6X8,
    FONT8X8
};

inline FontBase* getFont(FontType type)
{
    switch(type)
    {
        case FontType::FONT5X7:
            return &font5x7;
        case FontType::FONT5X8:
            return &font5x8;
        case FontType::FONT6X8:
            return &font6x8;
        case FontType::FONT8X8:
            return &font8x8;
        default:
            return nullptr;
    }
}
} // namespace Fonts