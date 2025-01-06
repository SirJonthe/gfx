# gfx
## Copyright
Public domain, 2025

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
`gfx` does not allocate memory for the user. The `Image` structure is merely a container for the user's own pixel data coupled with some metadata relating to the image. Instead, the user is expected to keep track of memory, which allows the user to have very tight control over it.
```
uint8_t pixel_memory[CUSTOM_WIDTH*CUSTOM_HEIGHT*4]; // 4 = one byte per component in RGBA.
cc0::gfx::Image image = new_image(pixel_memory, CUSTOM_WIDTH, CUSTOM_HEIGHT, cc0::gfx::encode_rgba32, cc0::gfx::decode_rgba32, cc0::gfx::index_linear);
```
As noted, the `Image` struct does not manage the memory, so it will not delete memory when the structure falls outside of the scope. In the example above, the image is assumed to order its channels red-green-blue-alpha, each of which are 8 bits each, as evident by the provided encoder and decoder functions. `gfx` provides a few different encoders and decoders, but more advanced versions have to be implemented by the user.

In the above example a linear indexing function is used, meaning pixels are stored as rows of horizontal scanlines. Similar to encoders and decoders, `gfx` provides a few different indexing functions; linear and swizzled. Linear indexing is recommended for speed when the image is to be rendered on a scanline basis, for instance using `stretch_image`. However, when the access patters changes as a result of the orientation of the rendering primitive, such as when using `draw_triangle`, a swizzled indexing method may provide faster access.

### Filling a rectangle
Filling a rectangle is as simple as the following:
```
using cc0::gfx;

fill_rect(dst, Rect{ Point{ 0, 0 }, Point{ dst.width, dst.height } }, COLOR_BLACK);
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

fill_rect(dst, Rect{ Point{ 0, 0 }, Point{ dst.width, dst.height } }, COLOR_BLACK, shade_dstgray);
```

### Drawing lines

### Blitting images
"Blitting" refers to "bit block transfers", or just straight memory copies between regions of memory. Oftentimes this is the fastest method of drawing an image on top of another image (it should be noted that blitting in `gfx` is not actually a straight memory copy between surfaces as there is a decoding and encoding process that happens in case the source surface and the destination surface are not the same pixel format). Blitting does not support stretching images on the destination surface (see `stretch_image` instead), but does support axis mirrorring via the `src_rect` parameter which selects the region on the source surface to draw on top of the destination surface.
```
blit_image(dst, Point{ rand() % dst.width, rand() %  dst.height}, src, Rect{});
```

`blit_image` supports shaders.
```
```

### Stretching images

### Print text
`gfx` has one function to render text. This 

More advanced text rendering functions must be implemented by the user.

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
Depending on how the data is organized in the pixel memory, the final image could just take the unpartitioned `subimage_pixels` parameter and the full `WIDTH` and `HEIGHT` parameters rather than performing stitching on as a manual step on a separate surface.

The above method might prove to be slow. A faster way to do the same is using rendering masks. All rendering methods provide a `write_rect` parameter as a final parameter. This parameter ensures that all rendering outside of the specified area is discarded efficiently.
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
A word of warning regarding the render mask method; For image surfaces that are 1 bit per pixel, the rendering mask must cover whole bytes, i.e. normally be multiples of 8 (although the exact requirements will depend on the encoding, decoding, and indexing). Splitting a single byte using a rendering mask may lead to two threads writing to the same byte at the same time, and cause a program crash.

### Displaying graphics
`gfx` does not ship with the ability to display the graphics that are rendered to a surface. Instead, the user must either implement or provide the necessary API calls to output 

## TODO
- [ ] The stretch_image version using sampling probably needs to interpolate normalized texture coordinates because bilinear needs to interpolate 0 - width-1, while nearest needs to interpolate 0 - width, and the function itself does not know how to differentiate the bounds.