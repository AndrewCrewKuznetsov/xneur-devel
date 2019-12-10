# Description
X Neural Switcher (http://www.xneur.ru)

It's program like Punto Switcher, but has other functionality
and it's really better with some various functions.

This utility is made for X Window System.

It is developed in Linux for now, but we're thinking it will
work on any *nix system with X11R6 & gcc3.

Latest Release and other info you can read at: http://www.xneur.ru

## Building with CMake
On Ubuntu first you need to install some dependencies:
```sh
sudo apt-get install libgstreamer1.0-dev libnotify-dev libenchant-dev libxi-dev libxtst-dev
```

For CMake 3.13 or later, [build with this commands](https://cmake.org/cmake/help/v3.13/manual/cmake.1.html):
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

For older CMake, use following commands:
```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

xneur will appear in the `build` directory.
