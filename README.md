# SSD1306 SPI OLED Display Library for Raspberry Pi Pico 1/2

This is a C++ library for controlling SSD1306-based OLED displays over SPI with the Raspberry Pi Pico and Pico W.

## Features

- Easy-to-use C++ API
- Supports multiple display sizes (e.g., 128x64, 128x32)
- Hardware SPI interface for fast updates
- Drawing functions: pixels, lines, rectangles, text, bitmaps
- Example projects included


## How to build

To build the examples along with the library, run the following commands to configure the build system and enable example builds:

```sh
cmake -B build -DBUILD_EXAMPLES=ON
cmake --build build
```

After the build completes, you will find the compiled example binaries in the `build/examples` directory.

You can then flash these example binaries to your Raspberry Pi Pico to test the library functionality.

If you only want to build the library (without the examples), use the following commands:

```sh
cmake -B build
cmake --build build
```

This will generate the library files in the `build` directory, which you can then link to your own projects.


## How to use in your project

1. **Add the library as a submodule**

    In your project's root directory, add this repository as a submodule:

    ```sh
    git submodule add https://github.com/lukstep/pico-ssd1306-spi.git
    ```

2. **Initialize and update submodules**

    If you haven't already, initialize and update all submodules:

    ```sh
    git submodule update --init --recursive
    ```

3. **Include the library in your CMake project**

    In your project's `CMakeLists.txt`, add the following lines to include and link the library:

    ```cmake
    add_subdirectory(pico-ssd1306-spi)
    target_link_libraries(your_target ssd1306)
    ```

    Replace `your_target` with the name of your executable or library.

4. **Include the header in your source code**

    In your C/C++ source files, include the main header:

    ```cpp
    #include "ssd1306.hpp"
    ```

5. **Build your project**

    Configure and build your project as usual:

    ```sh
    cmake -B build
    cmake --build build
    ```

6. **Flash and test**

    Flash the resulting binary to your Raspberry Pi Pico and test the OLED display functionality.

For more detailed usage examples, refer to the `examples/` directory in the repository.

