# Building Carbonsteel

## 1. Install dependencies

Git, Meson, Flex, Bison, GCC and CMake must be available on your system. The exact installation instructions are OS-dependent, but on Alpine Linux these can be installed with

`apk add git meson flex bison gcc musl-dev cmake`

## 2. Download sources

Clone the latest version of Carbonsteel source code

`git clone https://github.com/andersonarc/carbonsteel.git`

and enter the directory

`cd carbonsteel`

## 3. Build

Set up a build directory

`mkdir build`

and initialize Meson

`meson setup build .`

Then, compile the source code

`meson compile -C build`

If there are no errors, the compiled binary will be located at build/carbonsteel.