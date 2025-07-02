# Displaying Bitmaps

This library allows you to display bitmap images on your screen using the `drawBitmap` function.

## How to Use `drawBitmap`

The `drawBitmap` function requires the following arguments:
- **x** and **y**: The position on the screen where the bitmap will be drawn.
- **bitmap pointer**: A pointer to the bitmap data.
- **width** and **height**: The size of the bitmap.

## Preparing Your Bitmap

To display an image, you first need to convert it to a format compatible with the library. You can use the free online tool [image2cpp](https://javl.github.io/image2cpp/) for this purpose.

### Steps to Convert and Display an Image

1. **Open [image2cpp](https://javl.github.io/image2cpp/).**
2. **Upload your image** (for example, the GitHub logo).
3. **Set the output format** to "Arduino code" or "C array".
4. **Adjust the image size** to fit your display.
5. **Important! Set "Draw mode"** to "Vertical - 1 bit per pixel"** 
6. **Copy the generated array** into your code.

7. **Call `drawBitmap`** with the correct parameters.

#### Example

```cpp
// Example bitmap array (replace with your own)
const uint8_t github_logo [] = {
    // ... bitmap data ...
};

display.drawBitmap(0, 0, github_logo, width, height);
```

Now, your image should appear on the display!