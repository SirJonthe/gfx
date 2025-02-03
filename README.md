# gfx
## Copyright
Public domain, 2025

github.com/SirJonthe

## About
`gfx` is a collection of simple and customizable software graphics rendering routines.

## Design
The library is written in standard C++ and does not make references to operating system or platform-specific functionality. This essentially makes the library embarrassingly portable, but adds the responsibility of displaying graphics rendered by the library to its user.

Every function is designed to be able to be customizable, essentially allowing for custom encoding, decoding, indexing, sampling, and shading. The library's performance is probably not the best due to the code needing to take a more general purpose approach in order to accomodate for custom code injection.

The library does not contain code to load or save images. Instead, the user is expected to provide such functionality. Conversely, the library also does not allocate any memory on the heap as the user is expected to inject pixel memory directly into the rendering routines in order to allow for the user to have full control over memory usage.

## Building
No special adjustments need to be made to build `gfx` except enabling C++11 compatibility or above. Simply include the relevant headers in your code and make sure the headers and source files are available in your compiler search paths. Using `g++` as an example, building is no harder than:

```
g++ -std=c++11 code.cpp gfx/gfx.cpp
```

...where `code.cpp` is an example source file containing some user code as well as the entry point for the program.

## Examples
### Creating an image
`gfx` does not allocate memory for the user. The `Image` structure is merely a container for the user's own pixel data coupled with some metadata relating to the image. Instead, the user is expected to keep track of memory, which allows the user to have very tight control over it.
```
using namespace cc0::gfx;

const int32_t WIDTH           = 512; // Just some arbitrary width.
const int32_t HEIGHT          = 512; // Just some arbitrary height.
const int32_t BYTES_PER_PIXEL = 4;   // One byte per component in RGBA.
uint8_t pixel_memory[WIDTH*HEIGHT*BYTES_PER_PIXEL];
Image image = new_image(
	pixel_memory,  // The pixel memory.
	WIDTH,         // The number of pixels in width.
	HEIGHT,        // The number of pixels in height.
	encode_rgba32, // The pixel encoder.
	decode_rgba32, // The pixel decoder.
	index_linear   // The pixel indexer.
);
```
As noted, the `Image` struct does not manage the memory, so it will not delete memory when the structure falls outside of the scope. In the example above, the image is assumed to order its channels red-green-blue-alpha, each of which are 8 bits each, as evident by the provided encoder and decoder functions. `gfx` provides a few different encoders and decoders, but more advanced versions have to be implemented by the user.

In the above example a linear indexing function is used, meaning pixels are stored as rows of horizontal scanlines. Similar to encoders and decoders, `gfx` provides a few different indexing functions; linear and swizzled. Linear indexing is recommended for speed when the image is to be rendered on a scanline basis, for instance using `stretch_image`. However, when the access patters changes as a result of the orientation of the rendering primitive, such as when using `draw_triangle`, a swizzled indexing method may provide faster access.

### Loading an image format
`gfx` does not provide code to load any given image format from disk into the application. This is a separate domain that should be handled by other libraries.

### Getting a color
Once the user has set up an image object with the proper encoder, decoder, and indexer a color from that image can be retrieved using the `get_color` function. `get_color` will return a pixel-format agnostic `RGBA32` color format:
```
using namespace cc0::gfx;

RGBA32 color = get_color(img, Point{ 0, 0 }); // Retrieves the pixel color at coordinate 0, 0 from the image.
```

### Setting a color
Once the user has set up an image object with the proper encoder, decoder, and indexer a color from that image can be modified using the `set_color` function:
```
using namespace cc0::gfx;

set_color(img, Point{ 0, 0 }, COLOR_RED); // Modifies the color at coordinate 0, 0 on the image and sets it to red.
```

`gfx` has a number of predefined color constants like `COLOR_BLACK`, `COLOR_CYAN`, etc.

### Filling a rectangle
Filling a rectangle is as simple as the following:
```
using cc0::gfx;

fill_rect(
	dst,                               // The destination image.
	Rect{                              // The fill rendering rectangle.
		Point{ 0, 0 },                 // The upper-left point of the rectangle.
		Point{ dst.width, dst.height } // The lower-right point of the rectangle.
	},
	COLOR_BLACK                        // The color to fill with.
);
```
The second parameter specifies the region to be filled. Note that `dst` is assumed to be a properly set up destination surface. 

`fill_rect` supports shaders. Shading can be used to create more elaborate effects than just setting the destination surface pixels to a set color, and can, for instance, be used to alter the existing colors on the destination surface like converting it to grayscale, adding, multiplying, subtracting the input color, etc.
```
RGBA32 shade_dstgray(RGBA32 dst, RGBA32)
{
	const uint8_t GRAY = (uint8_t)(
		(float)dst.red   * 0.30f +
		(float)dst.green * 0.59f +
		(float)dst.blue  * 0.11f
	);
	return RGBA32{ GRAY, GRAY, GRAY, dst.alpha };
}

fill_rect(
	dst,                               // The destination image.
	Rect{                              // The fill rendering rectangle.
		Point{ 0, 0 },                 // The upper-left point of the rectangle.
		Point{ dst.width, dst.height } // The lower-right point of the rectangle.
	},
	COLOR_BLACK,                       // The color to fill with.
	shade_dstgray                      // The shader to use.
);
```

### Drawing lines

### Blitting images
"Blitting" refers to "bit block transfers", or just straight memory copies between regions of memory. Oftentimes this is the fastest method of drawing an image on top of another image (it should be noted that blitting in `gfx` is not actually a straight memory copy between surfaces as there is a decoding and encoding process that happens in case the source surface and the destination surface are not the same pixel format). Blitting does not support stretching images on the destination surface (see `stretch_image` instead), but does support axis mirrorring via the `src_rect` parameter which selects the region on the source surface to draw on top of the destination surface.
```
using namespace cc0::gfx;

blit_image(
	dst,                                // The destination surface.
	Point{ 0, 0},                       // The point on the destination surface to anchor the upper left part of the source image. 
	src,                                // The source image.
	Rect{                               // The region of the source image to draw. This can be any portion of the source image.
		Point{ 0, 0 },                  // The upper-left point of the rendering rectangle.
		Point{ src.width, src.height }  // The lower-right point of the rendering rectangle.
	}
);
```
The points of the `src_rect` parameter can correspond to any corner in the rendering rectangle. Depending on which corner the points correspond to the rendering result may be mirrorred along an axis. When the first point corresponds to the upper-left corner rendering is not mirrorred.

`blit_image` supports shaders.
```
using namespace cc0::gfx;

blit_image(
	dst,                                // The destination surface.
	Point{ 0, 0},                       // The point on the destination surface to anchor the upper left part of the source image. 
	src,                                // The source image.
	Rect{                               // The region of the source image to draw. This can be any portion of the source image.
		Point{ 0, 0 },                  // The upper-left point of the rendering rectangle.
		Point{ src.width, src.height }  // The lower-right point of the rendering rectangle.
	}
	shade_alpha                         // The shader (here an alpha shader for transparency). 
);
```
The above example uses the built-in shader `shade_alpha` which will blend the source and destination colors depending on the alpha component in the source colors. The user can provide their own shader function for custom results.

It is safe to specify rendering coordinates which would render the source image wholly or partially off-screen on the destination buffer.

### Stretching images
Images can be arbitrarily stretched when drawing them to another surface:
```
using namespace cc0::gfx;

stretch_image(
	dst,                               // The destination surface.
	Rect{                              // The rectangle to stretch the source region over on the destination image.
		Point{ 0, 0 },                 // The first point of the destination rectangle, corresponding to the upper-left corner.
		Point{ dst.width, dst.height } // The second point of the destination rectancle, corresponding to the lower-left corner.
	},
	src,                               // The source image.
	Rect{                              // The region of the source image to stretch over the destination rectangle.
		Point{ 0, 0 },                 // The first point of the source rectangle, corresponding to the upper-left corner.
		Point{ src.width, src.height } // The second point of the source rectancle, corresponding to the lower-left corner.
	}
);
```
Both the destination and source rectangles support flipping rendering axis by letting the points making up the rectangles lie on different corners of the rectangles. For instance, if the first point on the destination rectangle lies on the upper-left corner of the rectangle then the source image is rendered unmirrorred (unless the source rectangle is flipped as well), while if it lies on the lower-right corner then the source image is flipped in both X and Y axis. Similarly, the source image rectangle can also be flipped in the same way, but be conscious that double-flipping with cancel out as no-flipping.

`stretch_image` supports shaders and samplers:
```
using namespace cc0::gfx;

stretch_image(
	dst,                                   // The destination surface.
	Rect{                                  // The rectangle to stretch the source region over on the destination image.
		Point{ 0, 0 },                     // The first point of the destination rectangle, corresponding to the upper-left corner.
		Point{ dst.width, dst.height }     // The second point of the destination rectancle, corresponding to the lower-left corner.
	},
	src,                                   // The source image.
	Rect{                                  // The region of the source image to stretch over the destination rectangle.
		Point{ 0, 0 },                     // The first point of the source rectangle, corresponding to the upper-left corner.
		Point{ src.width-1, src.height-1 } // The second point of the source rectancle, corresponding to the lower-left corner.
	},
	sample_bilinear,
	shade_alpha
);
```
In the example above a bilinear sampler is used to  for an anti-aliased look. Note then that the end-point of the source rectangle needs to be one less than the requested dimensions to prevent accessing out of bounds. An alpha shader is used to blend the colors of the destination and source colors depending on the source alpha.

### Printing text
`gfx` has one function to render text using a built-in 8x8 bitfont. The function is simply a wrapper around existing blitting and stretching functions using a 1 bit per pixel encoder and decoder.
```
using namespace cc0::gfx;

int32_t x = 0; // The X coordinate
x = print_text(
	dst,                          // The destination surface.
	Point{ x, 0 },                // The coordinate of the upper-left corner of the first character in the sequence.
	"Hello, World!", 13,          // The text to render, as well as the maximum length of the text.
	RGBA32{ 255, 255, 255, 255 }, // The color of the text.
	5                             // The integer scale of the text.
);
```
The function returns the X coordinate past the last character. This X coordinate can be supplied to a subsequent call to `print_text` so that printing starts where it last left off.

`print_text` uses upper bounds for the text string it is printing to avoid run-away code execution. It still exits the rendering function if a null terminating character is encountered before reaching the upper bounds.

The built-in bitfont only supports non-whitespace ASCII characters.

More advanced text rendering functions must be implemented by the user, including printing of numbers and text newline wrapping and tabulation.

### Notes on rendering rectangles
Rendering rectangles, `Rect`s, consist of two points that are diagonally adjacent called `a` and `b`. `dst_rect` and `src_rect` rendering rectangle parameters can be ordered in any way, i.e. either of the two points can be the upper-left, upper-right, lower-left, or lower-right point of the rectangle. For these parameters, rendering can be mirrored along an axis depending on what corner the points correspond to. When `a` corresponds to the upper-left point rendering is normal, upper-right rendering is mirrored in X axis, lower-left rendering is mirrorred in Y axis, and lower-right rendering is mirrorred in both X and Y axis. It is also important to note that the `a` point is inclusive, while the `b` point is exclusive in terms of rendering; The coordinate specified by `a` will be included in the rendering result, while the coordinate specified by `b` will not.

`write_rect` works slightly differently. While both `a` and `b` points can correspond to any corder on the rectangle, the rendering function will always reorder the rectangle such that `a` is the upper-left corner, and `b` is the lower-right corner. After reordering `a` is inclusive and `b` is not, i.e. the coordinate specified by `a` will be included in the rendering result, while the coordinate specified by `b` will not.

### Thread-safety
The basic rendering routines are single-threaded. However, with the exception of `get_color` and `set_color`, they are all embarassingly parallel. This means that users will see a proportional performance boost to the number of cores added to the rendering duties. Adding cores to rendering duties requires manual intervention, and can be achieved in a few different ways.

One way to implementing multi-threading is to render to separate surfaces and stitch them together as a final step.
```
#include <thread>
#include <cstdint>

using namespace cc0::gfx;

int main()
{
	const int32_t WIDTH = 512;
	const int32_t HEIGHT = 512;
	const int32_t BYTES_PER_PIXEL = 4;
	const int32_t NUM_THREADS = 2;
	uint8_t subimage_pixels[WIDTH*HEIGHT*BYTES_PER_PIXEL];
	Image image[NUM_THREADS] = {
		new_image(subimage_pixels, WIDTH, HEIGHT / NUM_THREADS, encode_rgba32, decode_rgba32, index_linear),
		new_image(subimage_pixels + (WIDTH * HEIGHT * BYTES_PER_PIXEL) / NUM_THREADS, WIDTH, HEIGHT / NUM_THREADS, encode_rgba32, decode_rgba32, index_linear),
	};

	std::thread threads[NUM_THREADS] = {
		std::thread(fill_rect, image[0], Rect{ Point{ 0, 0 }, Point{ image[0].width, image[0].height } }, shade_set),
		std::thread(fill_rect, image[1], Rect{ Point{ 0, 0 }, Point{ image[1].width, image[1].height } }, shade_set)
	}
	for (int32_t i = 0; i < NUM_THREADS; ++i) {
		threads[i].join();
	}

	uint8_t final_pixels[WIDTH*HEIGHT*BYTES_PER_PIXEL]
	Image final_image = new_image(final_pixels, WIDTH, HEIGHT, encode_rgba32, decode_rgba32, index_linear);

	blit_image(
		final_image,
		Point{ 0, 0 },
		image[0],
		Rect{ Point{ 0, 0 }, Point{ image[0].width, image[0].height } }
	);
	blit_image(
		final_image,
		Point{ 0, image[0].height },
		image[1],
		Rect{ Point{ 0, 0 }, Point{ image[1].width, image[1].height } }
	);

	return 0;
}
```
Depending on how the data is organized in the pixel memory, the final image could just take the unpartitioned `subimage_pixels` parameter and the full `WIDTH` and `HEIGHT` parameters rather than performing stitching as a manual step on a separate surface.

The above method might prove to be slow, especially if stitching is required. A faster way to do the same is using rendering masks. All rendering methods provide a `write_rect` parameter as a final parameter. This parameter ensures that all rendering outside of the specified area is discarded efficiently.
```
#include <thread>
#include <cstdint>

using namespace cc0::gfx;

int main()
{
	const int32_t WIDTH = 512;
	const int32_t HEIGHT = 512;
	const int32_t BYTES_PER_PIXEL = 4;
	uint8_t pixels[WIDTH*HEIGHT*BYTES_PER_PIXEL];
	Image image = new_image(pixels, WIDTH, HEIGHT, encode_rgba32, decode_rgba32, index_linear);
	
	const int32_t NUM_THREADS = 2;

	std::thread threads[NUM_THREADS] = {
		std::thread(fill_rect, image, Rect{ Point{ 0, 0 }, Point{ image.width, image.height } }, shade_set, Rect{ Point{ 0, 0 }, Point{ image.width, image.height / 2 } }), // The final parameter is the rendering mask.
		std::thread(fill_rect, image, Rect{ Point{ 0, 0 }, Point{ image.width, image.height } }, shade_set, Rect{ Point{ 0, image.height / 2 }, Point{ image.width, image.height } }) // The final parameter is the rendering mask.
	}
	for (int32_t i = 0; i < NUM_THREADS; ++i) {
		threads[i].join();
	}

	return 0;
}
```
Using render masks may be more limited than rendering on separate surfaces when separate pixels share the same byte. For instance, when rendering to image surfaces that are 1 bit per pixel the rendering mask must cover whole bytes, i.e. normally be multiples of 8 (although the exact requirements will depend on the encoding, decoding, and indexing) in order to prevent two or more threads attempting to modify the same byte.

### Displaying graphics
`gfx` does not ship with the ability to display the graphics that are rendered to a surface for reasons of complete platform agnosticism and portability. Instead, the user must either implement or provide the necessary API calls to output the rendering results themselves.

## TODO
- [ ] blit_image implementation - more lightweight than stretch_image
- [ ] Replace scaled integers with fixed in samplers, and shaders
- [ ] Add special functionality when interpolating float, fixed, and int
- [ ] Perspective correct triangle
- [ ] shade_rect interpolates custom attributes and has a constant structure argument
- [ ] Maybe remove all sampler + shader functions and replace with shade_rect/shade_tri which interpolates attributes