#ifndef CC0_GFX_H_INCLUDED__
#define CC0_GFX_H_INCLUDED__

#include <limits.h>
#include <math.h>
#include <cstdint>

namespace cc0
{
	namespace gfx
	{
		/// @brief Namespace for internal functions. Do not use these.
		namespace internal
		{
			template < uint32_t bits > struct intinfo {};

			template <>
			struct intinfo<8>
			{
				typedef int8_t      int_t;
				typedef uint8_t     uint_t;
				typedef intinfo<8>  prev;
				typedef intinfo<16> next;
			};

			template <>
			struct intinfo<16>
			{
				typedef int16_t     int_t;
				typedef uint16_t    uint_t;
				typedef intinfo<8>  prev;
				typedef intinfo<32> next;
			};

			template <>
			struct intinfo<32>
			{
				typedef int32_t     int_t;
				typedef uint32_t    uint_t;
				typedef intinfo<16> prev;
				typedef intinfo<64> next;
			};

			template <>
			struct intinfo<64>
			{
				typedef int64_t     int_t;
				typedef uint64_t    uint_t;
				typedef intinfo<32> prev;
				typedef intinfo<64> next;
			};
		}

		template < uint32_t bits, uint32_t precision >
		struct fixed
		{
			typename internal::intinfo<bits>::int_t x;

			fixed() = default;
			fixed(const fixed&) = default;
			fixed &operator=(const fixed&) = default;

			fixed(typename internal::intinfo<bits>::int_t n) : x(n << precision) {}
			operator typename internal::intinfo<bits>::int_t( void ) { return x >> precision; }

			fixed &operator+=(fixed r) { x += r.x; return *this; }
			fixed &operator-=(fixed r) { x -= r.x; return *this; }
			fixed &operator*=(fixed r) {
				typename internal::intinfo<bits>::next::int_t n = typename internal::intinfo<bits>::next::int_t(x) * r.x;
				x = (n.x >> precision);
				return *this;
			}
			fixed &operator/=(fixed r) {
				x = (typename internal::intinfo<bits>::next::int_t(x) << precision) / r.x;
				return *this;
			}
		};

		typedef fixed<32,15> fixed32_t;

		/// @brief Color separated as channels.
		struct RGBA32
		{
			uint8_t red;   // The red component.
			uint8_t green; // The green component.
			uint8_t blue;  // The blue component.
			uint8_t alpha; // The alpha component.
		};

		static const RGBA32 COLOR_WHITE   = { 255, 255, 255, 255 }; // White.
		static const RGBA32 COLOR_BLACK   = { 0, 0, 0, 255 };       // Black.
		static const RGBA32 COLOR_RED     = { 255, 0, 0, 255 };     // Red.
		static const RGBA32 COLOR_GREEN   = { 0, 255, 0, 255 };     // Green.
		static const RGBA32 COLOR_BLUE    = { 0, 0, 255, 255 };     // Blue.
		static const RGBA32 COLOR_MAGENTA = { 255, 0, 255, 255 };   // Magenta.
		static const RGBA32 COLOR_CYAN    = { 0, 255, 255, 255 };   // Cyan.
		static const RGBA32 COLOR_YELLOW  = { 255, 255, 0, 255 };   // Yellow.
		static const RGBA32 COLOR_GRAY    = { 127, 127, 127, 255 }; // Gray.

		/// @brief A point with integer coordinates.
		struct Point
		{
			int32_t x; // The X coordinate.
			int32_t y; // The Y coordinate.
		};

		/// @brief A point with real coordinates.
		struct Coord
		{
			fixed32_t x; // The X coordinate.
			fixed32_t y; // The Y coordinate.
		};

		/// @brief A rectangle.
		struct Rect
		{
			Point a; // A point.
			Point b; // The diagonally adjacent point.
		};

		/// @brief A span.
		struct Span
		{
			int32_t a;    // An endpoint on the span.
			int32_t b;    // An endpoint on the span.
			int32_t axis; // The axis. 0 for X, 1 for Y.
		};

		/// @brief A line.
		struct Line
		{
			Coord a; // An endpoint on the line.
			Coord b; // An endpoint on the line.
		};
		
		struct Image;

		/// @brief Maps a 2D coordinate to a 1D pixel coordinate.
		typedef int32_t (*Indexer)(const Image&, Point);

		/// @brief Converts 2D coordinates into a 1D pixel coordinate. Assumes the pixels in a source image are stored left to right, and in such that the last pixel of the previous row directly precedes the first pixel in the current row.
		/// @param src The source image.
		/// @param p The X,Y coordinate.
		/// @return The 1D index accessing the requested pixel at the given coordinates in the source image.
		/// @note The output coordinate is the pixel coordinate, not the index of a byte in the pixel array of the source image. The difference is that under this scheme you can address individual bits in the pixel array for times where pixels do not neatly line up with byte boundaries, for instance when pixels are less than one byte large.
		int32_t index_linear(const Image &src, Point p);

		/// @brief Converts 2D coordinates into a 1D pixel coordinate. Assumes the pixels in a source image are stored left to right, and in such that the last pixel of the previous row directly precedes the first pixel in the current row. When accessing outside the area of the source image, the coordinates wrap around.
		/// @param src The source image.
		/// @param p The X,Y coordinate.
		/// @return The 1D index accessing the requested pixel at the given coordinates in the source image.
		/// @note The output coordinate is the pixel coordinate, not the index of a byte in the pixel array of the source image. The difference is that under this scheme you can address individual bits in the pixel array for times where pixels do not neatly line up with byte boundaries, for instance when pixels are less than one byte large.
		int32_t index_repeat_linear(const Image &src, Point p);

		/// @brief Converts 2D coordinates into a 1D pixel coordinate. Assumes the pixels in a source image are stored in Z/Morton order (a recursive Z pattern).
		/// @param p The X,Y coordinate.
		/// @return The 1D index accessing the requested pixel at the given coordinates in the source image.
		/// @note The output coordinate is the pixel coordinate, not the index of a byte in the pixel array of the source image. The difference is that under this scheme you can address individual bits in the pixel array for times where pixels do not neatly line up with byte boundaries, for instance when pixels are less than one byte large.
		int32_t index_z(const Image&, Point p);

		/// @brief Converts 2D coordinates into a 1D pixel coordinate. Assumes the pixels in a source image are stored in Z/Morton order (a recursive Z pattern). When accessing outside the area of the source image, the coordinates wrap around.
		/// @param p The X,Y coordinate.
		/// @return The 1D index accessing the requested pixel at the given coordinates in the source image.
		/// @note The output coordinate is the pixel coordinate, not the index of a byte in the pixel array of the source image. The difference is that under this scheme you can address individual bits in the pixel array for times where pixels do not neatly line up with byte boundaries, for instance when pixels are less than one byte large.
		int32_t index_repeat_z(const Image&, Point p);

		/// @brief Decodes a single pixel in RGBA16 format into RGBA32 format.
		/// @param pixel The pixel pointer pointing to the start of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_RGBA16(const void *pixel);

		/// @brief Decodes a single pixel in RGB24 format into RGBA32 format.
		/// @param pixel The pixel pointer pointing to the start of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_RGB24(const void *pixel);

		/// @brief Decodes a single pixel in RGBA32 format into RGBA32 format.
		/// @param pixel The pixel pointer pointing to the start of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_RGBA32(const void *pixel);

		/// @brief Decodes a single pixel in BGRA16 format into RGBA32 format.
		/// @param pixel The pixel pointer pointing to the start of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_BGRA16(const void *pixel);

		/// @brief Decodes a single pixel in BGR24 format into RGBA32 format.
		/// @param pixel The pixel pointer pointing to the start of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_BGR24(const void *pixel);

		/// @brief Decodes a single pixel in BGRA32 format into RGBA32 format.
		/// @param pixel The pixel pointer pointing to the start of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_BGRA32(const void *pixel);

		/// @brief Decodes a single pixel in ARGB16 format into RGBA32 format.
		/// @param pixel The pixel pointer pointing to the start of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_ARGB16(const void *pixel);

		/// @brief Decodes a single pixel in ARGB32 format into RGBA32 format.
		/// @param pixel The pixel pointer pointing to the start of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_ARGB32(const void *pixel);

		/// @brief Decodes a single pixel in ABGR16 format into RGBA32 format.
		/// @param pixel The pixel pointer pointing to the start of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_ABGR16(const void *pixel);

		/// @brief Decodes a single pixel in ABGR32 format into RGBA32 format.
		/// @param pixel The pixel pointer pointing to the start of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_ABGR32(const void *pixel);

		/// @brief Decodes a single pixel in 1bpp format into RGBA32 format.
		/// @param pixel The pixel pointer pointing to the start of the pixel.
		/// @return The decoded RGBA32.
		uint8_t decode_1bpp(uint8_t pixelx8, int32_t i);

		/// @brief Encodes a RGBA32 color format into RGBA16 format.
		/// @param color The color to encode.
		/// @param out The byte index to store the encoded pixel.
		void encode_RGBA16(RGBA32 color, void *out);

		/// @brief Encodes a RGBA32 color format into RGB24 format.
		/// @param color The color to encode.
		/// @param out The byte index to store the encoded pixel.
		void encode_RGB24(RGBA32 color, void *out);

		/// @brief Encodes a RGBA32 color format into RGBA32 format.
		/// @param color The color to encode.
		/// @param out The byte index to store the encoded pixel.
		void encode_RGBA32(RGBA32 color, void *out);

		/// @brief Encodes a RGBA32 color format into BGRA16 format.
		/// @param color The color to encode.
		/// @param out The byte index to store the encoded pixel.
		void encode_BGRA16(RGBA32 color, void *out);

		/// @brief Encodes a RGBA32 color format into BGR24 format.
		/// @param color The color to encode.
		/// @param out The byte index to store the encoded pixel.
		void encode_BGR24(RGBA32 color, void *out);

		/// @brief Encodes a RGBA32 color format into BGRA32 format.
		/// @param color The color to encode.
		/// @param out The byte index to store the encoded pixel.
		void encode_BGRA32(RGBA32 color, void *out);

		/// @brief Encodes a RGBA32 color format into ARGB16 format.
		/// @param color The color to encode.
		/// @param out The byte index to store the encoded pixel.
		void encode_ARGB16(RGBA32 color, void *out);

		/// @brief Encodes a RGBA32 color format into ARGB32 format.
		/// @param color The color to encode.
		/// @param out The byte index to store the encoded pixel.
		void encode_ARGB32(RGBA32 color, void *out);

		/// @brief Encodes a RGBA32 color format into ABGR16 format.
		/// @param color The color to encode.
		/// @param out The byte index to store the encoded pixel.
		void encode_ABGR16(RGBA32 color, void *out);

		/// @brief Encodes a RGBA32 color format into ABGR32 format.
		/// @param color The color to encode.
		/// @param out The byte index to store the encoded pixel.
		void encode_ABGR32(RGBA32 color, void *out);

		/// @brief Encodes a RGBA32 color format into 1bpp format.
		/// @param color The color to encode.
		/// @param out The byte index to store the encoded pixel.
		void encode_1bpp(uint8_t color, void *out, int32_t i);

		/// @brief Decodes a pixel pointer parameter into a color and returns it.
		typedef RGBA32 (*Decoder)(const Image&, Point);

		/// @brief Decodes a single pixel in RGBA16 format into RGBA32 format.
		/// @param src The source image to pick a pixel from.
		/// @param p The X,Y coordinate of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_RGBA16(const Image &src, Point p);

		/// @brief Decodes a single pixel in RGB24 format into RGBA32 format.
		/// @param src The source image to pick a pixel from.
		/// @param p The X,Y coordinate of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_RGB24(const Image &src, Point p);

		/// @brief Decodes a single pixel in RGBA32 format into RGBA32 format.
		/// @param src The source image to pick a pixel from.
		/// @param p The X,Y coordinate of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_RGBA32(const Image &src, Point p);

		/// @brief Decodes a single pixel in BGRA16 format into RGBA32 format.
		/// @param src The source image to pick a pixel from.
		/// @param p The X,Y coordinate of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_BGRA16(const Image &src, Point p);

		/// @brief Decodes a single pixel in BGR24 format into RGBA32 format.
		/// @param src The source image to pick a pixel from.
		/// @param p The X,Y coordinate of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_BGR24(const Image &src, Point p);

		/// @brief Decodes a single pixel in BGRA32 format into RGBA32 format.
		/// @param src The source image to pick a pixel from.
		/// @param p The X,Y coordinate of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_BGRA32(const Image &src, Point p);

		/// @brief Decodes a single pixel in ARGB16 format into RGBA32 format.
		/// @param src The source image to pick a pixel from.
		/// @param p The X,Y coordinate of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_ARGB16(const Image &src, Point p);

		/// @brief Decodes a single pixel in ARGB32 format into RGBA32 format.
		/// @param src The source image to pick a pixel from.
		/// @param p The X,Y coordinate of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_ARGB32(const Image &src, Point p);

		/// @brief Decodes a single pixel in ABGR16 format into RGBA32 format.
		/// @param src The source image to pick a pixel from.
		/// @param p The X,Y coordinate of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_ABGR16(const Image &src, Point p);

		/// @brief Decodes a single pixel in ABGR32 format into RGBA32 format.
		/// @param src The source image to pick a pixel from.
		/// @param p The X,Y coordinate of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_ABGR32(const Image &src, Point p);

		/// @brief Decodes a single pixel in 1bpp format into RGBA32 format.
		/// @param src The source image to pick a pixel from.
		/// @param p The X,Y coordinate of the pixel.
		/// @return The decoded RGBA32.
		RGBA32 decode_1bpp(const Image &src, Point p);

		/// @brief Encodes a color into the pixel pointer parameter.
		typedef void (*Encoder)(Image&, Point, RGBA32);

		/// @brief Encodes a RGBA32 color format into RGBA16 format and stores it in an image.
		/// @param dst The destination image to store the encoded pixel in.
		/// @param p The X,Y coordinate of the pixel.
		/// @param color The color to encode.
		void encode_RGBA16(Image &dst, Point p, RGBA32 color);

		/// @brief Encodes a RGBA32 color format into RGB24 format and stores it in an image.
		/// @param dst The destination image to store the encoded pixel in.
		/// @param p The X,Y coordinate of the pixel.
		/// @param color The color to encode.
		void encode_RGB24(Image &dst, Point p, RGBA32 color);

		/// @brief Encodes a RGBA32 color format into RGBA32 format and stores it in an image.
		/// @param dst The destination image to store the encoded pixel in.
		/// @param p The X,Y coordinate of the pixel.
		/// @param color The color to encode.
		void encode_RGBA32(Image &dst, Point p, RGBA32 color);

		/// @brief Encodes a RGBA32 color format into BGRA16 format and stores it in an image.
		/// @param dst The destination image to store the encoded pixel in.
		/// @param p The X,Y coordinate of the pixel.
		/// @param color The color to encode.
		void encode_BGRA16(Image &dst, Point p, RGBA32 color);

		/// @brief Encodes a RGBA32 color format into RGB24 format and stores it in an image.
		/// @param dst The destination image to store the encoded pixel in.
		/// @param p The X,Y coordinate of the pixel.
		/// @param color The color to encode.
		void encode_BGR24(Image &dst, Point p, RGBA32 color);

		/// @brief Encodes a RGBA32 color format into BGRA32 format and stores it in an image.
		/// @param dst The destination image to store the encoded pixel in.
		/// @param p The X,Y coordinate of the pixel.
		/// @param color The color to encode.
		void encode_BGRA32(Image &dst, Point p, RGBA32 color);

		/// @brief Encodes a RGBA32 color format into ARGB16 format and stores it in an image.
		/// @param dst The destination image to store the encoded pixel in.
		/// @param p The X,Y coordinate of the pixel.
		/// @param color The color to encode.
		void encode_ARGB16(Image &dst, Point p, RGBA32 color);

		/// @brief Encodes a RGBA32 color format into ARGB32 format and stores it in an image.
		/// @param dst The destination image to store the encoded pixel in.
		/// @param p The X,Y coordinate of the pixel.
		/// @param color The color to encode.
		void encode_ARGB32(Image &dst, Point p, RGBA32 color);

		/// @brief Encodes a RGBA32 color format into ABGR16 format and stores it in an image.
		/// @param dst The destination image to store the encoded pixel in.
		/// @param p The X,Y coordinate of the pixel.
		/// @param color The color to encode.
		void encode_ABGR16(Image &dst, Point p, RGBA32 color);

		/// @brief Encodes a RGBA32 color format into ABGR32 format and stores it in an image.
		/// @param dst The destination image to store the encoded pixel in.
		/// @param p The X,Y coordinate of the pixel.
		/// @param color The color to encode.
		void encode_ABGR32(Image &dst, Point p, RGBA32 color);

		/// @brief Encodes a RGBA32 color format into 1bpp format and stores it in an image.
		/// @param dst The destination image to store the encoded pixel in.
		/// @param p The X,Y coordinate of the pixel.
		/// @param color The color to encode.
		void encode_1bpp(Image &dst, Point p, RGBA32 color);

		/// @brief Blends a source color and a destination color and returns the result.
		typedef RGBA32 (*Shader)(RGBA32, RGBA32);

		/// @brief Simply returns the source color. The destination color is always discarded.
		/// @param dst The destination color in the location to be written.
		/// @param src The source color in the location being read from.
		/// @return The final color.
		RGBA32 shade_set(RGBA32 dst, RGBA32 src);

		/// @brief Blends the source and destination color based on the source alpha component via linear interpolation.
		/// @param dst The destination color in the location to be written.
		/// @param src The source color in the location being read from.
		/// @return The final color.
		RGBA32 shade_alpha(RGBA32 dst, RGBA32 src);

		/// @brief A shader that returns the source color if it is not the same as a given color key. Returns the destination color otherwise.
		/// @param dst The destination color in the location to be written.
		/// @param src The source color in the location being read from.
		/// @return The final color.
		RGBA32 shade_colorkey(RGBA32 dst, RGBA32 src);

		/// @brief Returns a grayscale variant of the source color. The destination color is always discarded.
		/// @param dst The destination color in the location to be written.
		/// @param src The source color in the location being read from.
		/// @return The final color.
		RGBA32 shade_gray(RGBA32 dst, RGBA32 src);

		/// @brief Returns a grayscale variant of the source color, blended with the destination color based on the source alpha component via linear interpolation.
		/// @param dst The destination color in the location to be written.
		/// @param src The source color in the location being read from.
		/// @return The final color.
		RGBA32 shade_grayalpha(RGBA32 dst, RGBA32 src);

		/// @brief A shader that returns the grayscale source color if it is not the same as a given color key prior to grayscale conversion. Returns the destination color otherwise.
		/// @param dst The destination color in the location to be written.
		/// @param src The source color in the location being read from.
		/// @return The final color.
		RGBA32 shade_graycolorkey(RGBA32 dst, RGBA32 src);

		/// @brief A shader that sets the destination color if its alpha is greater than 0, and the source color otherwise.
		/// @param dst The destination color in the location to be written.
		/// @param src The source color in the location being read from.
		/// @return The final color.
		RGBA32 shade_stencil(RGBA32 dst, RGBA32 src);

		/// @brief Uses image coordinates (16.15 fixed point) to sample one or several colors from an image and return some blended result.
		typedef RGBA32 (*Sampler)(const Image&, int32_t, int32_t);

		/// @brief Samples the nearest color. Looks pixelated when stretched out, and looks aliased when compressed to a smaller size than normal.
		/// @param src The source image to sample from.
		/// @param u The U coordinate to sample from (16.15 fixed-point format).
		/// @param v The V coordinate to sample from (16.15 fixed-point format).
		/// @return The final sampled color.
		RGBA32 sample_nearest(const Image &src, int32_t u, int32_t v);

		/// @brief Samples the four closest colors and interpolates between the colors to get the most accurate color.
		/// @param src The source image to sample from.
		/// @param u The U coordinate to sample from (16.15 fixed-point format).
		/// @param v The V coordinate to sample from (16.15 fixed-point format).
		/// @return The final sampled color.
		RGBA32 sample_bilinear(const Image &src, int32_t u, int32_t v);

		/// @brief An image container. The container is mostly a convenience, does not own its own memory, and is unaware how to decode and encode pixels as well as how to index tbe pixel data given 2D coordinates.
		struct Image
		{
			uint8_t *pixels; // A pixel array containing the color information of the image. May also contain other types of data (such as RLE header) as long as the encoder, decoder, and indexer can traverse the data.
			int32_t  width;  // The width, in pixels, of the image.
			int32_t  height; // The height, in pixels, of the image.
			Encoder  encode; // The function used to encode a RGB32 into a native pixel format.
			Decoder  decode; // The function used to decode the native pixel format into a RGB32.
			Indexer  index;  // The function used to map a 2D coordinate to a singular pixel.

			static constexpr int32_t MAX_DIMENSION = USHRT_MAX; // The maximally supported single dimension of an image.
		};

		/// @brief Creates a new image with the provided data.
		/// @param pixels The pointer to the image data, mainly containing colors, but could contain any image data.
		/// @param width The width, in pixels, of the image.
		/// @param height The height, in pixels, of the image.
		/// @param encoder The encoder function that transforms a color from RGBA32 into the image's native format.
		/// @param decoder The decoder function that transforms a color from the image's native format into RGBA32.
		/// @param indexer The indexer function that transforms a 2D coordinate into a 1D pixel coordinate.
		/// @return A new image.
		Image new_image(void *pixels, int32_t width, int32_t height, Encoder encoder, Decoder decoder, Indexer indexer);

		/// @brief Returns a color at the given coordinates from an image.
		/// @param src The image to get the color from.
		/// @param p The X,Y coordinate to get the color from.
		/// @return The color at the given coordinate.
		RGBA32 get_color(const Image &src, Point p);

		/// @brief Sets the color of a pixel at a given integer coordinate of an image.
		/// @param dst The image to set the color to.
		/// @param p The X,Y coordinate of the color to set.
		/// @param c The color.
		void set_color(Image &dst, Point p, RGBA32 c);

		const Rect FULL_RECT = Rect{ Point{0,0}, Point{Image::MAX_DIMENSION, Image::MAX_DIMENSION} }; // The maximum (unsigned) rectangle allowed. Can often be used to indicate the maximally allowed region of an image.

		const Span FULL_SPAN = Span{ 0, Image::MAX_DIMENSION }; // The maximum (unsigned) span allowed. Can often be used to indicate the maximally allowed region of an image.

		/// @brief Fills the specified area with the specified color using the specified shader (normal assignment is default).
		/// @param dst The destination image.
		/// @param dst_rect The region to fill.
		/// @param color The input color.
		/// @param shader The shader to use to blend colors.
		/// @param write_rect The area on the destination image that is writeable. All rendering outside this area is discarded. The entire destination image is selected by default. The region is automatically clipped to the maximally allowed dimensions on the destination image.
		void fill_rect(Image &dst, Rect dst_rect, RGBA32 color, Shader shader = shade_set, Rect write_rect = FULL_RECT);

		/// @brief Draws a line between the two specified points using the two specified colors and the specified predicate (normal assignment is default).
		/// @param pDst The destination image.
		/// @param pX1 The first X coordinate.
		/// @param pY1 The first Y coordinate.
		/// @param pColor1 The first color.
		/// @param pX2 The second X coordinate.
		/// @param pY2 The second Y coordinate.
		/// @param pColor2 The second color.
		/// @param shader The shader to use to blend colors.
		void draw_line(Image &pDst, int32_t pX1, int32_t pY1, RGBA32 pColor1, int32_t pX2, int32_t pY2, RGBA32 pColor2, Shader shader);

		/// @brief Stretches a portion of a source image over the portion of a destination image.
		/// @param dst The destination image.
		/// @param dst_rect The area on the destination image over which the selected source region will be stretched. The region is automatically clipped to the maximally allowed dimensions on the destination image.
		/// @param src The source image.
		/// @param src_rect The area on the source image to stretch over the selected destination region. The entire source image is selected by default. The region is automatically clipped to the maximally allowed dimensions on the source image.
		/// @param write_rect The area on the destination image that is writeable. All rendering outside this area is discarded. The entire destination image is selected by default. The region is automatically clipped to the maximally allowed dimensions on the destination image.
		void stretch_image(Image &dst, Rect dst_rect, const Image &src, Rect src_rect = FULL_RECT, Rect write_rect = FULL_RECT);

		/// @brief Stretches a portion of a source image over the portion of a destination image.
		/// @param dst The destination image.
		/// @param dst_rect The area on the destination image over which the selected source region will be stretched. The region is automatically clipped to the maximally allowed dimensions on the destination image.
		/// @param src The source image.
		/// @param shader The shader to use to blend colors.
		/// @param sampler The sampler to use on the source image to sample colors.
		/// @param src_rect The area on the source image to stretch over the selected destination region. The entire source image is selected by default. The region is automatically clipped to the maximally allowed dimensions on the source image.
		/// @param write_rect The area on the destination image that is writeable. All rendering outside this area is discarded. The entire destination image is selected by default. The region is automatically clipped to the maximally allowed dimensions on the destination image.
		void stretch_image(Image &dst, Rect dst_rect, const Image &src, Shader shader, Sampler sampler, Rect src_rect = FULL_RECT, Rect write_rect = FULL_RECT);

		/// @brief Draws an axis-aligned span on the destination image.
		/// @param dst The destination image.
		/// @param dst_axis The fixed axis of the span. The axis is determined by the inverse of the destination span settings.
		/// @param dst_span The span of the destination image. The axis is determined by the span settings.
		/// @param color The color.
		/// @param shader The shader to use to blend colors (defaults to assignment).
		/// @param write_rect The area on the destination image that is writeable. All rendering outside this area is discarded. The entire destination image is selected by default. The region is automatically clipped to the maximally allowed dimensions on the destination image.
		void fill_span(Image &dst, int32_t dst_axis, Span dst_span, RGBA32 color, Shader shader = shade_set, Rect write_rect = FULL_RECT);

		/// @brief Draws an axis-aligned span on the destination image by stretching a region of the source image over a region of the destination image.
		/// @param dst The destination image.
		/// @param dst_axis The fixed-axis coordinate to sample the span from on the destination image. The axis is determined by the inverse of the destination span settings.
		/// @param dst_span The span of the destination image. The axis is determined by the span settings.
		/// @param src The source image.
		/// @param src_axis The fixed-axis coordinate to sample the span from on the destination image. The axis is determined by the inverse of the source span settings.
		/// @param src_span The span of the source image to stretch over the span on the destination image. The axis is determined by the span settings.
		/// @param shader The shader to use.
		/// @param sampler The sampler to use on the source image.
		/// @param write_rect The area on the destination image that is writeable. All rendering outside this area is discarded. The entire destination image is selected by default. The region is automatically clipped to the maximally allowed dimensions on the destination image.
		void stretch_span(Image &dst, int32_t dst_axis, Span dst_span, const Image &src, Shader shader, Sampler sampler, int32_t src_axis, Span src_span = FULL_SPAN, Rect write_rect = FULL_RECT);

		/// @brief Draws an axis-aligned span on the destination image by stretching a region of the source image over a region of the destination image.
		/// @param dst The destination image.
		/// @param dst_axis The fixed-axis coordinate to sample the span from on the destination image. The axis is determined by the inverse of the destination span settings.
		/// @param dst_span The span of the destination image. The axis is determined by the span settings.
		/// @param src The source image.
		/// @param src_axis The fixed-axis coordinate to sample the span from on the destination image. The axis is determined by the inverse of the source span settings.
		/// @param src_line Two points on the source image making up a 2D span to stretch over the destination span.
		/// @param shader The shader to use.
		/// @param sampler The sampler to use on the source image.
		/// @param write_rect The area on the destination image that is writeable. All rendering outside this area is discarded. The entire destination image is selected by default. The region is automatically clipped to the maximally allowed dimensions on the destination image.
		void stretch_span(Image &dst, int32_t dst_x, Span dst_span, const Image &src, Shader shader, Sampler sampler, Line src_line, Rect write_rect = FULL_RECT);

		/// @brief Draws text using the built-in font on the 
		/// @param dst The target image.
		/// @param x The X coordinate of the first character of text.
		/// @param y The Y coordinate of the first character of text.
		/// @param text The text.
		/// @param text_len The number of characters in the text.
		/// @param color The color to render the text in.
		/// @param scale The scale of the text.
		/// @param write_rect The area on the destination image that is writeable. All rendering outside this area is discarded. The entire destination image is selected by default. The region is automatically clipped to the maximally allowed dimensions on the destination image.
		/// @return The X coordinate past the last character of the input string.
		/// @note This uses only a built-in font, but the effect can be replicated using any bitfont using the draw_image function and the 1bpp encoder/decoder functions.
		int32_t print_text(Image &dst, Point p, const char *text, int32_t text_len, RGBA32 color, int32_t scale = 1, Rect write_rect = FULL_RECT);

		/// @brief Fills the specified triangle area with the specified color using the specified shader (normal assignment is default).
		/// @param dst The destination image.
		/// @param a A point of the triangle.
		/// @param b A point of the triangle.
		/// @param c A point of the triangle.
		/// @param color The input color.
		/// @param shader The shader to use to blend colors (assignment is default).
		/// @param write_rect The area on the destination image that is writeable. All rendering outside this area is discarded. The entire destination image is selected by default. The region is automatically clipped to the maximally allowed dimensions on the destination image.
		void fill_tri(Image &dst, Point a, Point b, Point c, RGBA32 color, Shader shader = shade_set, Rect write_rect = FULL_RECT);

		/// @brief An array of attributes that can be interpolated across a primitive.
		/// @tparam attr_type The base type of the attributes.
		/// @tparam attr_size The number of attributes.
		template < typename attr_type, uint32_t attr_size >
		using Attributes = attr_type[attr_size];

		/// @brief A shader that takes a set of input attributes and returns a resulting color.
		/// @tparam attr_type The base type of the attributes.
		/// @tparam var_attr_size The number of elements in the varying attribute array.
		/// @tparam const_attr_size The number of elements in the constant attribute array.
		template < typename attr_type, uint32_t var_attr_size, typename const_attr_t >
		using AttributeShader = RGBA32 (*)(RGBA32, const Attributes<attr_type,var_attr_size>&, const const_attr_t&);

		/// @brief Interpolates a set of attributes across a triangle and inputs them into a custom shader.
		/// @tparam attr_type The base type of the attributes.
		/// @tparam var_attr_size The number of elements in the varying attribute array.
		/// @tparam const_attr_size The number of elements in the constant attribute array.
		/// @param dst The destination surface.
		/// @param a A point of the triangle.
		/// @param b A point of the triangle.
		/// @param c A point of the triangle.
		/// @param a_var The attributes to interpolate at the A point of the triangle.
		/// @param b_var The attributes to interpolate at the B point of the triangle.
		/// @param c_var The attributes to interpolate at the C point of the triangle.
		/// @param consts A set of attributes that remains constant over the entire primitive.
		/// @param shader The shader tp use to blend colors.
		/// @param write_rect The area on the destination image that is writeable. All rendering outside this area is discarded. The entire destination image is selected by default. The region is automatically clipped to the maximally allowed dimensions on the destination image.
		template < typename attr_type, uint32_t var_attr_size, typename const_attr_t >
		void draw_tri(Image &dst, Point a, Point b, Point c, const Attributes<attr_type, var_attr_size> &a_var, const Attributes<attr_type, var_attr_size> &b_var, const Attributes<attr_type, var_attr_size> &c_var, const const_attr_t &consts, AttributeShader<attr_type, var_attr_size, const_attr_t> shader, Rect write_rect = FULL_RECT);

		/// @brief Namespace for internal functions. Do not use these.
		namespace internal
		{
			template < typename type_t >
			void swap(type_t &a, type_t &b);

			template < typename type_t >
			type_t min(type_t a, type_t b);

			template < typename type_t >
			type_t min(type_t a, type_t b, type_t c);

			template < typename type_t >
			type_t max(type_t a, type_t b);

			template < typename type_t >
			type_t max(type_t a, type_t b, type_t c);

			template < typename type_t >
			type_t clamp(type_t min, type_t x, type_t max);

			cc0::gfx::Rect order(cc0::gfx::Rect r);

			cc0::gfx::Rect clip(cc0::gfx::Rect a, cc0::gfx::Rect b);

			uint64_t determine_halfspace(cc0::gfx::Point a, cc0::gfx::Point b, cc0::gfx::Point point);
			
			bool is_top_left(cc0::gfx::Point a, cc0::gfx::Point b);
		}
	}
}

template < uint32_t bits, uint32_t precision > cc0::gfx::fixed<bits,precision> operator+(cc0::gfx::fixed<bits,precision> l, cc0::gfx::fixed<bits,precision> r) { return l += r; }
template < uint32_t bits, uint32_t precision > cc0::gfx::fixed<bits,precision> operator-(cc0::gfx::fixed<bits,precision> l, cc0::gfx::fixed<bits,precision> r) { return l -= r; }
template < uint32_t bits, uint32_t precision > cc0::gfx::fixed<bits,precision> operator*(cc0::gfx::fixed<bits,precision> l, cc0::gfx::fixed<bits,precision> r) { return l *= r; }
template < uint32_t bits, uint32_t precision > cc0::gfx::fixed<bits,precision> operator/(cc0::gfx::fixed<bits,precision> l, cc0::gfx::fixed<bits,precision> r) { return l /= r; }

template < typename attr_type, uint32_t var_attr_size, typename const_attr_t >
void cc0::gfx::draw_tri(cc0::gfx::Image &dst, cc0::gfx::Point a, cc0::gfx::Point b, cc0::gfx::Point c, const cc0::gfx::Attributes<attr_type, var_attr_size> &a_var, const cc0::gfx::Attributes<attr_type, var_attr_size> &b_var, const cc0::gfx::Attributes<attr_type, var_attr_size> &c_var, const const_attr_t &consts, cc0::gfx::AttributeShader<attr_type, var_attr_size, const_attr_t> shader, cc0::gfx::Rect write_rect)
{
	cc0::gfx::Attributes<attr_type, var_attr_size> var;

	write_rect = internal::clip(internal::order(write_rect), Rect{ Point{ 0, 0 }, Point{ dst.width, dst.height } });

	// AABB Clipping
	const int32_t min_y = internal::max(internal::min(a.y, b.y, c.y), write_rect.a.y);
	const int32_t max_y = internal::min(internal::max(a.y, b.y, c.y), write_rect.b.y - 1);
	if (max_y - min_y <= 0) { return; }
	const int32_t min_x = internal::max(internal::min(a.x, b.x, c.x), write_rect.a.x);
	const int32_t max_x = internal::min(internal::max(a.x, b.x, c.x), write_rect.b.x - 1);
	if (max_x - min_x <= 0) { return; }

	// Triangle setup
	Point    p    = { min_x, min_y };
	uint64_t w0_y = internal::determine_halfspace(b, c, p);
	uint64_t w1_y = internal::determine_halfspace(c, a, p);
	uint64_t w2_y = internal::determine_halfspace(a, b, p);

	// Interpolation/triangle setup
	const int64_t w2_x_inc = a.y - b.y;
	const int64_t w2_y_inc = b.x - a.x;
	const int64_t w0_x_inc = b.y - c.y;
	const int64_t w0_y_inc = c.x - b.x;
	const int64_t w1_x_inc = c.y - a.y;
	const int64_t w1_y_inc = a.x - c.x;

	const int64_t sum_inv_area_x2 = (1ULL<<47) / (w0_y + w1_y + w2_y);
	int64_t       l0_y            =     w0_y * sum_inv_area_x2;
	int64_t       l1_y            =     w1_y * sum_inv_area_x2;
	int64_t       l2_y            =     w2_y * sum_inv_area_x2;
	const int64_t l0_x_inc        = w0_x_inc * sum_inv_area_x2;
	const int64_t l1_x_inc        = w1_x_inc * sum_inv_area_x2;
	const int64_t l2_x_inc        = w2_x_inc * sum_inv_area_x2;
	const int64_t l0_y_inc        = w0_y_inc * sum_inv_area_x2;
	const int64_t l1_y_inc        = w1_y_inc * sum_inv_area_x2;
	const int64_t l2_y_inc        = w2_y_inc * sum_inv_area_x2;

	w0_y += internal::is_top_left(b, c) ? 0 : -1;
	w1_y += internal::is_top_left(c, a) ? 0 : -1;
	w2_y += internal::is_top_left(a, b) ? 0 : -1;

	for (p.y = min_y; p.y <= max_y; ++p.y) {

		int64_t w0 = int64_t(w0_y);
		int64_t w1 = int64_t(w1_y);
		int64_t w2 = int64_t(w2_y);

		int64_t l0 = int64_t(l0_y);
		int64_t l1 = int64_t(l1_y);
		int64_t l2 = int64_t(l2_y);

		for (p.x = min_x; p.x <= max_x; ++p.x) {

			if ((w0 | w1 | w2) >= 0) {
				for (int32_t i = 0; i < var_attr_size; ++i) {
					var[i] = attr_type((l0 * a_var[i] + l1 * b_var[i] + l2 * c_var[i]) >> 32);
				}
				set_color(dst, p, shader(get_color(dst, p), var, consts));
			}

			w0 += w0_x_inc;
			w1 += w1_x_inc;
			w2 += w2_x_inc;

			l0 += l0_x_inc;
			l1 += l1_x_inc;
			l2 += l2_x_inc;
		}

		w0_y += w0_y_inc;
		w1_y += w1_y_inc;
		w2_y += w2_y_inc;

		l0_y += l0_y_inc;
		l1_y += l1_y_inc;
		l2_y += l2_y_inc;
	}
}

template < typename type_t >
void cc0::gfx::internal::swap(type_t &a, type_t &b)
{
	type_t t = a;
	a = b;
	b = t;
}

template < typename type_t >
type_t cc0::gfx::internal::min(type_t a, type_t b)
{
	return a < b ? a : b;
}

template < typename type_t >
type_t cc0::gfx::internal::min(type_t a, type_t b, type_t c)
{
	return cc0::gfx::internal::min(a, cc0::gfx::internal::min(b, c));
}

template < typename type_t >
type_t cc0::gfx::internal::max(type_t a, type_t b)
{
	return a > b ? a : b;
}

template < typename type_t >
type_t cc0::gfx::internal::max(type_t a, type_t b, type_t c)
{
	return cc0::gfx::internal::max(a, cc0::gfx::internal::max(b, c));
}

template < typename type_t >
type_t cc0::gfx::internal::clamp(type_t min, type_t x, type_t max)
{
	return cc0::gfx::internal::max(min, cc0::gfx::internal::min(x, max));
}

#endif
