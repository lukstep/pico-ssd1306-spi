#pragma once

class FontBase
{
  public:
    virtual ~FontBase() = default;

    virtual uint8_t width() const = 0;
    virtual uint8_t height() const = 0;
    virtual uint8_t characterSpace() const = 0;
    virtual uint8_t characterOffset() const = 0;
    virtual const uint8_t* getFontData() const = 0;
};