# gfx
## Copyright
Public domain, 2024

github.com/SirJonthe

## About
`gfx` is a collection of simple and customizable software graphics rendering routines.

## Design
The library is written in standard C++ and does not make references to operating system or platform-specific functionality. This essentially makes the library embarrassingly portable, but adds the responsibility of displaying graphics rendered by the library to its user.

Every function is designed to be able to be customizable, essentially allowing for custom encoding, decoding, indexing, sampling, and shading.

The library's performance is probably not the best due to the code needing to take a more general purpose approach in order to accomodate for custom code injection.

The library does not contain code to load or save images. Instead, the user is expected to provide such functionality. Conversely, the library also does not allocate any memory on the heap as the user is expected to inject pixel memory directly into the rendering routines in order to allow for the user to have full control over memory usage.

## Building
No special adjustments need to be made to build `gfx` except enabling C++11 compatibility or above. Simply include the relevant headers in your code and make sure the headers and source files are available in your compiler search paths. Using `g++` as an example, building is no harder than:

```
g++ -std=c++11 code.cpp gfx/gfx.cpp
```

...where `code.cpp` is an example source file containing some user code as well as the entry point for the program.

## Examples
### Creating an image
`gfx` does not allocate memory for the user. Instead, the user is expected to keep track of memory, which allows the user to have very tight control over memory.
```
uint8_t pixel_memory[CUSTOM_WIDTH*CUSTOM_HEIGHT*4]; // 4 = one byte per component in RGBA.
cc0::gfx::Image image = new_image(pixel_memory, CUSTOM_WIDTH, CUSTOM_HEIGHT, cc0::gfx::encode_rgba32, cc0::gfx::decode_rgba32, cc0::gfx::index_linear);
```
As noted, the `Image` struct does not manage the memory, so it will not delete memory when the structure falls outside of the scope. In the example above, the image is assumed to order its channels red-green-blue-alpha, each of which are 8 bits each, as evident by the provided encoder and decoder functions. `gfx` provides a few different encoders and decoders, but more advanced versions have to be implemented by the user.

In the above example a linear indexing function is used, meaning pixels are stored as rows of horizontal scanlines. Similar to encoders and decoders, `gfx` provides a few different indexing functions; linear and swizzled. Linear indexing is recommended for speed when the image is to be rendered on a scanline basis, for instance using `stretch_image`. However, when the access patters changes as a result of the orientation of the rendering primitive, such as when using `draw_triangle`, a swizzled indexing method may provide faster access.

### 

## TODO
- [ ] The stretch_image version using sampling probably needs to interpolate normalized texture coordinates because bilinear needs to interpolate 0 - width-1, while nearest needs to interpolate 0 - width, and the function itself does not know how to differentiate the bounds.