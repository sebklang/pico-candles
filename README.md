##Building
First of all, follow the official [Getting Started with Raspberry Pi Pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) guide to install the Pico SDK and export it to your path. Then place your generated `pico_sdk_import.cmake` file in this root folder and build the executable like so:
```
mkdir build
cd build
cmake ..
make
```

After which only `make` is sufficient.
