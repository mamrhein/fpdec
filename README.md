# fpdec

Fixed-point decimal arithmetic

## Building From Source

First make sure that you have [CMake](http://www.cmake.org/) and an C/C++ compiler environment installed.

Then open a terminal, go to the source directory and type the following commands:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

## Running unit tests

After building this project you may run its unit tests by using these commands:

    $ make test  # To run all tests via CTest
    $ make catch # Run all tests directly, showing more details to you

#### Documentation

For more details see the documentation provided with the source distribution
or [here](https://libfpdec.readthedocs.io/en/latest).
