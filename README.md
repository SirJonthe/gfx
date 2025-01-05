# gfx
## Copyright
Public domain, 2024

github.com/SirJonthe

## About
`gfx` is a collection of simple and customizable software graphics rendering routines.

## Design
The library is written in standard C++ and does not make references to operating system or platform-specific functionality. This essentially makes the library embarrassingly portable, but adds the responsibility of displaying graphics rendered by the library to its user.

Every function is designed to be able to be customizable, essentially allowing for custom shading.

Performance is probably not the best due to the code needing to take a more general purpose approach in order to accomodate for custom code injection.

## Building
No special adjustments need to be made to build `gfx` except enabling C++11 compatibility or above. Simply include the relevant headers in your code and make sure the headers and source files are available in your compiler search paths. Using `g++` as an example, building is no harder than:

```
g++ -std=c++11 code.cpp gfx/gfx.cpp
```

...where `code.cpp` is an example source file containing some user code as well as the entry point for the program.