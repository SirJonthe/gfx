#include "gfx.h"

#define FONT_ATLAS_CHAR_WIDTH_COUNT   1 // The number of glyphs in the atlas in the X axis.
#define FONT_ATLAS_CHAR_HEIGHT_COUNT 95 // The number of glyphs in the atlas in the Y axis.
#define FONT_CHAR_PX_WIDTH            6 // The number of pixels in the glyph in the X axis.
#define FONT_CHAR_PX_HEIGHT           8 // The number of pixels in the glyph in the Y axis.
#define FONT_CELL_PX_WIDTH            8 // The number of pixels in the glyph (including unused pixels) in the X axis.
#define FONT_CELL_PX_HEIGHT           8 // The number of pixels in the glyph (including unused pixels) in the Y axis.
#define FONT_CHAR_ASCII_START        33
#define FONT_CHAR_ASCII_END         126

static uint8_t FONT_ATLAS[] = { // The font bits as 1 bit per pixel.
	0x00, 0x00, 0x04, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x14, 0x14,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x3e, 0x14, 0x3e, 0x14, 0x00,
	0x00, 0x00, 0x3c, 0x16, 0x3e, 0x34, 0x1e, 0x00, 0x00, 0x00, 0x22, 0x10,
	0x08, 0x04, 0x22, 0x00, 0x00, 0x00, 0x0e, 0x04, 0x2a, 0x12, 0x2c, 0x00,
	0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x04,
	0x04, 0x04, 0x08, 0x00, 0x00, 0x00, 0x04, 0x08, 0x08, 0x08, 0x04, 0x00,
	0x00, 0x00, 0x00, 0x14, 0x08, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
	0x1c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02,
	0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00,
	0x00, 0x00, 0x1c, 0x22, 0x2a, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x08, 0x0c,
	0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, 0x1e, 0x20, 0x1c, 0x02, 0x3e, 0x00,
	0x00, 0x00, 0x1e, 0x20, 0x1c, 0x20, 0x1e, 0x00, 0x00, 0x00, 0x22, 0x22,
	0x3e, 0x20, 0x20, 0x00, 0x00, 0x00, 0x3e, 0x02, 0x1e, 0x20, 0x1e, 0x00,
	0x00, 0x00, 0x1c, 0x02, 0x1e, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x3e, 0x20,
	0x30, 0x20, 0x20, 0x00, 0x00, 0x00, 0x1c, 0x22, 0x1c, 0x22, 0x1c, 0x00,
	0x00, 0x00, 0x3c, 0x22, 0x3c, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x04,
	0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04, 0x02, 0x00,
	0x00, 0x00, 0x08, 0x04, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x1c,
	0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x08, 0x04, 0x02, 0x00,
	0x00, 0x00, 0x1e, 0x20, 0x1e, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1c, 0x32,
	0x2a, 0x32, 0x04, 0x00, 0x00, 0x00, 0x08, 0x14, 0x22, 0x3e, 0x22, 0x00,
	0x00, 0x00, 0x0e, 0x12, 0x1e, 0x22, 0x1e, 0x00, 0x00, 0x00, 0x3c, 0x02,
	0x02, 0x02, 0x3c, 0x00, 0x00, 0x00, 0x1e, 0x22, 0x22, 0x22, 0x1e, 0x00,
	0x00, 0x00, 0x3e, 0x02, 0x1e, 0x02, 0x3e, 0x00, 0x00, 0x00, 0x3e, 0x02,
	0x1e, 0x02, 0x02, 0x00, 0x00, 0x00, 0x3c, 0x02, 0x32, 0x22, 0x3c, 0x00,
	0x00, 0x00, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x00, 0x00, 0x00, 0x1c, 0x08,
	0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, 0x3e, 0x20, 0x20, 0x22, 0x1c, 0x00,
	0x00, 0x00, 0x22, 0x22, 0x1e, 0x22, 0x22, 0x00, 0x00, 0x00, 0x02, 0x02,
	0x02, 0x02, 0x3e, 0x00, 0x00, 0x00, 0x36, 0x2a, 0x22, 0x22, 0x22, 0x00,
	0x00, 0x00, 0x22, 0x26, 0x2a, 0x32, 0x22, 0x00, 0x00, 0x00, 0x1c, 0x22,
	0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x1e, 0x22, 0x1e, 0x02, 0x02, 0x00,
	0x00, 0x00, 0x1c, 0x22, 0x22, 0x32, 0x3c, 0x00, 0x00, 0x00, 0x1e, 0x22,
	0x3e, 0x12, 0x22, 0x00, 0x00, 0x00, 0x3c, 0x02, 0x1c, 0x20, 0x1e, 0x00,
	0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x22, 0x22,
	0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x08, 0x00,
	0x00, 0x00, 0x22, 0x22, 0x22, 0x2a, 0x36, 0x00, 0x00, 0x00, 0x22, 0x22,
	0x1c, 0x22, 0x22, 0x00, 0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x08, 0x00,
	0x00, 0x00, 0x3e, 0x20, 0x1c, 0x02, 0x3e, 0x00, 0x00, 0x00, 0x0c, 0x04,
	0x04, 0x04, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00,
	0x00, 0x00, 0x0c, 0x08, 0x08, 0x08, 0x0c, 0x00, 0x00, 0x00, 0x08, 0x14,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00,
	0x00, 0x00, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c,
	0x22, 0x22, 0x3c, 0x00, 0x00, 0x00, 0x02, 0x02, 0x1e, 0x22, 0x1e, 0x00,
	0x00, 0x00, 0x00, 0x3c, 0x02, 0x02, 0x3c, 0x00, 0x00, 0x00, 0x20, 0x20,
	0x3c, 0x22, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x3e, 0x02, 0x3c, 0x00,
	0x00, 0x00, 0x18, 0x04, 0x0c, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x3c,
	0x22, 0x3c, 0x20, 0x1c, 0x00, 0x00, 0x02, 0x02, 0x1e, 0x22, 0x22, 0x00,
	0x00, 0x00, 0x08, 0x00, 0x0c, 0x08, 0x1c, 0x00, 0x00, 0x00, 0x20, 0x00,
	0x20, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x02, 0x22, 0x1e, 0x22, 0x22, 0x00,
	0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x18, 0x00, 0x00, 0x00, 0x00, 0x1e,
	0x2a, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x22, 0x22, 0x22, 0x00,
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x1e,
	0x22, 0x1e, 0x02, 0x02, 0x00, 0x00, 0x00, 0x3c, 0x22, 0x3c, 0x20, 0x20,
	0x00, 0x00, 0x00, 0x3a, 0x06, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x3c,
	0x06, 0x38, 0x1e, 0x00, 0x00, 0x00, 0x04, 0x0c, 0x04, 0x04, 0x18, 0x00,
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x22,
	0x22, 0x14, 0x08, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x2a, 0x14, 0x00,
	0x00, 0x00, 0x00, 0x22, 0x1c, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x22,
	0x22, 0x3c, 0x20, 0x1c, 0x00, 0x00, 0x00, 0x3e, 0x10, 0x0c, 0x3e, 0x00,
	0x00, 0x00, 0x18, 0x08, 0x04, 0x08, 0x18, 0x00, 0x00, 0x00, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x0c, 0x08, 0x10, 0x08, 0x0c, 0x00,
	0x00, 0x00, 0x14, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3e,
	0x3e, 0x3e, 0x3e, 0x00
};

cc0::gfx::Rect<int32_t> cc0::gfx::internal::order(cc0::gfx::Rect<int32_t> r)
{
	if (r.a.x > r.b.x) { cc0::gfx::internal::swap(r.a.x, r.b.x); }
	if (r.a.y > r.b.y) { cc0::gfx::internal::swap(r.a.y, r.b.x); }
	return r;
}

cc0::gfx::Rect<int32_t> cc0::gfx::internal::clip(cc0::gfx::Rect<int32_t> a, cc0::gfx::Rect<int32_t> b)
{
	return cc0::gfx::Rect<int32_t>{
		cc0::gfx::Point<int32_t>{ cc0::gfx::internal::max(a.a.x, b.a.x), cc0::gfx::internal::max(a.a.y, b.a.y) },
		cc0::gfx::Point<int32_t>{ cc0::gfx::internal::min(a.b.x, b.b.x), cc0::gfx::internal::min(a.b.y, b.b.y) }
	};
}

uint64_t cc0::gfx::internal::determine_halfspace(cc0::gfx::Point<int32_t> a, cc0::gfx::Point<int32_t> b, cc0::gfx::Point<int32_t> point)
{
	return uint64_t(b.x - a.x) * uint64_t(point.y - a.y) - uint64_t(b.y - a.y) * uint64_t(point.x - a.x);
}

bool cc0::gfx::internal::is_top_left(cc0::gfx::Point<int32_t> a, cc0::gfx::Point<int32_t> b)
{
	// strictly connected to winding order
	return (a.x < b.x && b.y == a.y) || (a.y > b.y);
}

int32_t cc0::gfx::index_linear(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return (src.width * p.y) + p.x;
}

int32_t cc0::gfx::index_repeat_linear(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return index_linear(src, Point<int32_t>{ p.x % src.width, p.y % src.height });
}

int32_t cc0::gfx::index_z(const cc0::gfx::Image&, cc0::gfx::Point<int32_t> p)
{
	p.x = (p.x | (p.x << 8)) & 0x00FF00FF;
	p.x = (p.x | (p.x << 4)) & 0x0F0F0F0F;
	p.x = (p.x | (p.x << 2)) & 0x33333333;
	p.x = (p.x | (p.x << 1)) & 0x55555555;

	p.y = (p.y | (p.y << 8)) & 0x00FF00FF;
	p.y = (p.y | (p.y << 4)) & 0x0F0F0F0F;
	p.y = (p.y | (p.y << 2)) & 0x33333333;
	p.y = (p.y | (p.y << 1)) & 0x55555555;

	return p.x | (p.y << 1);
}

int32_t cc0::gfx::index_repeat_z(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return index_z(src, Point<int32_t>{ p.x % src.width, p.y % src.height });
}

cc0::gfx::RGBA32 cc0::gfx::decode_RGBA16(const void *pixel)
{
	// bits = RRRRR GGGGG BBBBB A
	constexpr uint32_t FIX_SCALAR = (256 << 8) / 31;
	const uint16_t     p16        = *((const uint16_t*)pixel);
	const uint32_t     p32        = uint32_t(p16);
	return RGBA32{
		uint8_t(((p32 & 0x7800) * FIX_SCALAR) >> 19),
		uint8_t(((p32 & 0x07C0) * FIX_SCALAR) >> 14),
		uint8_t(((p32 & 0x003E) * FIX_SCALAR) >> 9),
		uint8_t((p32 & 0x0001) ? 255 : 0)
	};
}

cc0::gfx::RGBA32 cc0::gfx::decode_RGB24(const void *pixel)
{
	return RGBA32{
		((uint8_t*)pixel)[0],
		((uint8_t*)pixel)[1],
		((uint8_t*)pixel)[2],
		255
	};
}

cc0::gfx::RGBA32 cc0::gfx::decode_RGBA32(const void *pixel)
{
	return RGBA32{
		((uint8_t*)pixel)[0],
		((uint8_t*)pixel)[1],
		((uint8_t*)pixel)[2],
		((uint8_t*)pixel)[3],
	};
}

cc0::gfx::RGBA32 cc0::gfx::decode_BGRA16(const void *pixel)
{
	// bits = BBBBB GGGGG RRRRR A
	constexpr uint32_t FIX_SCALAR = (256 << 8) / 31;
	const uint16_t     p16        = *((const uint16_t*)pixel);
	const uint32_t     p32        = uint32_t(p16);
	return RGBA32{
		uint8_t(((p32 & 0x003E) * FIX_SCALAR) >> 9),
		uint8_t(((p32 & 0x07C0) * FIX_SCALAR) >> 14),
		uint8_t(((p32 & 0x7800) * FIX_SCALAR) >> 19),
		uint8_t((p32 & 0x0001) ? 255 : 0)
	};
}

cc0::gfx::RGBA32 cc0::gfx::decode_BGR24(const void *pixel)
{
	return RGBA32{
		((uint8_t*)pixel)[2],
		((uint8_t*)pixel)[1],
		((uint8_t*)pixel)[0],
		255
	};
}

cc0::gfx::RGBA32 cc0::gfx::decode_BGRA32(const void *pixel)
{
	return RGBA32{
		((uint8_t*)pixel)[2],
		((uint8_t*)pixel)[1],
		((uint8_t*)pixel)[0],
		((uint8_t*)pixel)[3],
	};
}

cc0::gfx::RGBA32 cc0::gfx::decode_ARGB16(const void *pixel)
{
	// bits = A RRRRR GGGGG BBBBB
	constexpr uint32_t FIX_SCALAR = (256 << 8) / 31;
	const uint16_t     p16        = *((const uint16_t*)pixel);
	const uint32_t     p32        = uint32_t(p16);
	return RGBA32{
		uint8_t(((p32 & 0x7C00) * FIX_SCALAR) >> 18),
		uint8_t(((p32 & 0x03E0) * FIX_SCALAR) >> 13),
		uint8_t(((p32 & 0x001F) * FIX_SCALAR) >> 8),
		uint8_t((p32 & 0x8000) ? 255 : 0)
	};
}

cc0::gfx::RGBA32 cc0::gfx::decode_ARGB32(const void *pixel)
{
	return RGBA32{
		((uint8_t*)pixel)[1],
		((uint8_t*)pixel)[2],
		((uint8_t*)pixel)[3],
		((uint8_t*)pixel)[0],
	};
}

cc0::gfx::RGBA32 cc0::gfx::decode_ABGR16(const void *pixel)
{
	// bits = A BBBBB GGGGG RRRRR
	constexpr uint32_t FIX_SCALAR = (256 << 8) / 31;
	const uint16_t     p16        = *((const uint16_t*)pixel);
	const uint32_t     p32        = uint32_t(p16);
	return RGBA32{
		uint8_t(((p32 & 0x001F) * FIX_SCALAR) >> 8),
		uint8_t(((p32 & 0x03E0) * FIX_SCALAR) >> 13),
		uint8_t(((p32 & 0x7C00) * FIX_SCALAR) >> 18),
		uint8_t((p32 & 0x8000) ? 255 : 0)
	};
}

cc0::gfx::RGBA32 cc0::gfx::decode_ABGR32(const void *pixel)
{
	return RGBA32{
		((uint8_t*)pixel)[3],
		((uint8_t*)pixel)[2],
		((uint8_t*)pixel)[1],
		((uint8_t*)pixel)[0],
	};
}

uint8_t cc0::gfx::decode_1bpp(uint8_t pixelx8, int32_t i)
{
	return (pixelx8 & (1 << i)) > 0 ? 255 : 0;
}

void cc0::gfx::encode_RGBA16(cc0::gfx::RGBA32 color, void *out)
{
	// bits =  RRRRR GGGGG BBBBB A
	constexpr uint32_t FIX_SCALAR = (32 << 8) / 255;
	const uint32_t     r          = (uint32_t(color.red)   * FIX_SCALAR) >> 8;
	const uint32_t     g          = (uint32_t(color.green) * FIX_SCALAR) >> 8;
	const uint32_t     b          = (uint32_t(color.blue)  * FIX_SCALAR) >> 8;
	const uint32_t     a          = uint32_t(color.alpha > 0);
	const uint16_t     pixel      = uint16_t(a) | uint16_t(r << 11) | uint16_t(g << 6) | uint16_t(b << 1);
	*((uint16_t*)out) = pixel;
}

void cc0::gfx::encode_RGB24(cc0::gfx::RGBA32 color, void *out)
{
	((uint8_t*)out)[0] = color.red;
	((uint8_t*)out)[1] = color.green;
	((uint8_t*)out)[2] = color.blue;
}

void cc0::gfx::encode_RGBA32(cc0::gfx::RGBA32 color, void *out)
{
	((uint8_t*)out)[0] = color.red;
	((uint8_t*)out)[1] = color.green;
	((uint8_t*)out)[2] = color.blue;
	((uint8_t*)out)[3] = color.alpha;
}

void cc0::gfx::encode_BGRA16(cc0::gfx::RGBA32 color, void *out)
{
	// bits = BBBBB GGGGG RRRRR A
	constexpr uint32_t FIX_SCALAR = (32 << 8) / 255;
	const uint32_t     r          = (uint32_t(color.red)   * FIX_SCALAR) >> 8;
	const uint32_t     g          = (uint32_t(color.green) * FIX_SCALAR) >> 8;
	const uint32_t     b          = (uint32_t(color.blue)  * FIX_SCALAR) >> 8;
	const uint32_t     a          = uint32_t(color.alpha > 0);
	const uint16_t     pixel      = uint16_t(a) | uint16_t(b << 11) | uint16_t(g << 6) | uint16_t(r << 1);
	*((uint16_t*)out) = pixel;
}

void cc0::gfx::encode_BGR24(cc0::gfx::RGBA32 color, void *out)
{
	((uint8_t*)out)[0] = color.blue;
	((uint8_t*)out)[1] = color.green;
	((uint8_t*)out)[2] = color.red;
}

void cc0::gfx::encode_BGRA32(cc0::gfx::RGBA32 color, void *out)
{
	((uint8_t*)out)[0] = color.blue;
	((uint8_t*)out)[1] = color.green;
	((uint8_t*)out)[2] = color.red;
	((uint8_t*)out)[3] = color.alpha;
}

void cc0::gfx::encode_ARGB16(cc0::gfx::RGBA32 color, void *out)
{
	// bits = A RRRRR GGGGG BBBBB
	constexpr uint32_t FIX_SCALAR = (32 << 8) / 255;
	const uint32_t     r          = (uint32_t(color.red)   * FIX_SCALAR) >> 8;
	const uint32_t     g          = (uint32_t(color.green) * FIX_SCALAR) >> 8;
	const uint32_t     b          = (uint32_t(color.blue)  * FIX_SCALAR) >> 8;
	const uint32_t     a          = uint32_t(color.alpha > 0) << 15;
	const uint16_t     pixel      = uint16_t(a) | uint16_t(r << 10) | uint16_t(g << 5) | uint16_t(b);
	*((uint16_t*)out) = pixel;
}

void cc0::gfx::encode_ARGB32(cc0::gfx::RGBA32 color, void *out)
{
	((uint8_t*)out)[0] = color.alpha;
	((uint8_t*)out)[1] = color.red;
	((uint8_t*)out)[2] = color.green;
	((uint8_t*)out)[3] = color.blue;
}

void cc0::gfx::encode_ABGR16(cc0::gfx::RGBA32 color, void *out)
{
	// bits = A BBBBB GGGGG RRRRR
	constexpr uint32_t FIX_SCALAR = (32 << 8) / 255;
	const uint32_t     r          = (uint32_t(color.red)   * FIX_SCALAR) >> 8;
	const uint32_t     g          = (uint32_t(color.green) * FIX_SCALAR) >> 8;
	const uint32_t     b          = (uint32_t(color.blue)  * FIX_SCALAR) >> 8;
	const uint32_t     a          = uint32_t(color.alpha > 0) << 15;
	const uint16_t     pixel      = uint16_t(a) | uint16_t(b << 10) | uint16_t(g << 5) | uint16_t(r);
	*((uint16_t*)out) = pixel;
}

void cc0::gfx::encode_ABGR32(cc0::gfx::RGBA32 color, void *out)
{
	((uint8_t*)out)[0] = color.alpha;
	((uint8_t*)out)[1] = color.blue;
	((uint8_t*)out)[2] = color.green;
	((uint8_t*)out)[3] = color.red;
}

void cc0::gfx::encode_1bpp(uint8_t color, void *out, int32_t i)
{
	const uint8_t index = 1 << i;
	const uint8_t mask = *(uint8_t*)out;
	*(uint8_t*)out = (color > 0) ? (mask | index) : (mask & (~index));
}

cc0::gfx::RGBA32 cc0::gfx::decode_RGBA16(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return decode_RGBA16(src.pixels + src.index(src, p) * 2);
}

cc0::gfx::RGBA32 cc0::gfx::decode_RGB24(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return decode_RGB24(src.pixels + src.index(src, p) * 3);
}

cc0::gfx::RGBA32 cc0::gfx::decode_RGBA32(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return decode_RGBA32(src.pixels + src.index(src, p) * 3);
}

cc0::gfx::RGBA32 cc0::gfx::decode_BGRA16(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return decode_BGRA16(src.pixels + src.index(src, p) * 2);
}

cc0::gfx::RGBA32 cc0::gfx::decode_BGR24 (const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return decode_BGR24(src.pixels + src.index(src, p) * 3);
}

cc0::gfx::RGBA32 cc0::gfx::decode_BGRA32(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return decode_BGRA32(src.pixels + src.index(src, p) * 4);
}

cc0::gfx::RGBA32 cc0::gfx::decode_ARGB16(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return decode_ARGB16(src.pixels + src.index(src, p) * 2);
}

cc0::gfx::RGBA32 cc0::gfx::decode_ARGB32(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return decode_ARGB32(src.pixels + src.index(src, p) * 4);
}

cc0::gfx::RGBA32 cc0::gfx::decode_ABGR16(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return decode_ABGR16(src.pixels + src.index(src, p) * 2);
}

cc0::gfx::RGBA32 cc0::gfx::decode_ABGR32(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return decode_ABGR32(src.pixels + src.index(src, p) * 4);
}

cc0::gfx::RGBA32 cc0::gfx::decode_1bpp(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	const uint32_t INDEX = src.index(src, p);
	const uint8_t  MASK = decode_1bpp(((const uint8_t*)src.pixels)[INDEX >> 3], INDEX & 7);
	return RGBA32{ MASK, MASK, MASK, MASK };
}

void cc0::gfx::encode_RGBA16(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 color)
{
	encode_RGBA16(color, dst.pixels + dst.index(dst, p) * 2);
}

void cc0::gfx::encode_RGB24(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 color)
{
	encode_RGB24(color, dst.pixels + dst.index(dst, p) * 3);
}

void cc0::gfx::encode_RGBA32(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 color)
{
	encode_RGBA32(color, dst.pixels + dst.index(dst, p) * 4);
}

void cc0::gfx::encode_BGRA16(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 color)
{
	encode_BGRA16(color, dst.pixels + dst.index(dst, p) * 2);
}

void cc0::gfx::encode_BGR24(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 color)
{
	encode_BGR24(color, dst.pixels + dst.index(dst, p) * 3);
}

void cc0::gfx::encode_BGRA32(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 color)
{
	encode_BGRA32(color, dst.pixels + dst.index(dst, p) * 4);
}

void cc0::gfx::encode_ARGB16(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 color)
{
	encode_ARGB16(color, dst.pixels + dst.index(dst, p) * 2);
}

void cc0::gfx::encode_ARGB32(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 color)
{
	encode_ARGB32(color, dst.pixels + dst.index(dst, p) * 4);
}

void cc0::gfx::encode_ABGR16(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 color)
{
	encode_ABGR16(color, dst.pixels + dst.index(dst, p) * 2);
}

void cc0::gfx::encode_ABGR32(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 color)
{
	encode_ABGR32(color, dst.pixels + dst.index(dst, p) * 4);
}

void cc0::gfx::encode_1bpp(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 color)
{
	const uint32_t INDEX = dst.index(dst, p);
	encode_1bpp(color.alpha > 0, ((uint8_t*)dst.pixels) + (INDEX >> 3), INDEX & 7);
}

cc0::gfx::RGBA32 cc0::gfx::shade_set(cc0::gfx::RGBA32, cc0::gfx::RGBA32 src)
{
	return src;
}

cc0::gfx::RGBA32 cc0::gfx::shade_alpha(cc0::gfx::RGBA32 dst, cc0::gfx::RGBA32 src)
{
	dst.red   += (src.alpha * (src.red   - dst.red))   >> CHAR_BIT;
	dst.green += (src.alpha * (src.green - dst.green)) >> CHAR_BIT;
	dst.blue  += (src.alpha * (src.blue  - dst.blue))  >> CHAR_BIT;
	dst.alpha += (src.alpha * (src.alpha - dst.alpha)) >> CHAR_BIT;
	return dst;
}

cc0::gfx::RGBA32 cc0::gfx::shade_colorkey(cc0::gfx::RGBA32 pDst, cc0::gfx::RGBA32 pSrc)
{
	static const RGBA32   COLORKEY  = cc0::gfx::RGBA32{ 255, 0, 255, 0 };
	static const uint32_t KEY       = *(uint32_t*)(&COLORKEY);
	static const RGBA32   ALPHAMASK = cc0::gfx::RGBA32{ 255, 255, 255, 0 };
	static const uint32_t MASK      = *(uint32_t*)(&ALPHAMASK);
	return (((*(uint32_t*)(&pSrc)) & MASK) == KEY) ? pDst : pSrc;
}

cc0::gfx::RGBA32 cc0::gfx::shade_gray(cc0::gfx::RGBA32, cc0::gfx::RGBA32 src)
{
	// r = ~30%, g = ~60%, b = ~10%
	const uint32_t r = uint32_t(src.red)   * 3;
	const uint32_t g = uint32_t(src.green) * 6;
	const uint32_t b = uint32_t(src.blue);
	const uint8_t  GRAY = (r + g + b) / 10;
	return RGBA32{ GRAY, GRAY, GRAY, src.alpha };
}

cc0::gfx::RGBA32 cc0::gfx::shade_grayalpha(cc0::gfx::RGBA32 dst, cc0::gfx::RGBA32 src)
{
	return shade_alpha(dst, shade_gray(dst, src));
}

cc0::gfx::RGBA32 cc0::gfx::shade_graycolorkey(cc0::gfx::RGBA32 dst, cc0::gfx::RGBA32 src)
{
	static const RGBA32   COLORKEY  = cc0::gfx::RGBA32{ 255, 0, 255, 0 };
	static const uint32_t KEY       = *(uint32_t*)(&COLORKEY);
	static const RGBA32   ALPHAMASK = cc0::gfx::RGBA32{ 255, 255, 255, 0 };
	static const uint32_t MASK      = *(uint32_t*)(&ALPHAMASK);
	return (((*(uint32_t*)(&src)) & MASK) == KEY) ? dst : shade_gray(dst, src);
}

cc0::gfx::RGBA32 cc0::gfx::shade_stencil(cc0::gfx::RGBA32 dst, cc0::gfx::RGBA32 src)
{
	return src.alpha > 0 ? src : dst;
}

template < typename type_t >
type_t lerp(type_t a, type_t b, int32_t x)
{
	return a + (((b - a) * x) >> 15);
}

template < typename type_t >
type_t bilerp(type_t p00, type_t p10, type_t p01, type_t p11, int32_t x, int32_t y)
{
	return lerp(lerp(p00, p01, y), lerp(p10, p11, y), x);
}

cc0::gfx::RGBA32 rgba_bilerp(cc0::gfx::RGBA32 p00, cc0::gfx::RGBA32 p10, cc0::gfx::RGBA32 p01, cc0::gfx::RGBA32 p11, int32_t x, int32_t y)
{
	return cc0::gfx::RGBA32{
		(uint8_t)bilerp((int32_t)p00.red,   (int32_t)p10.red,   (int32_t)p01.red,   (int32_t)p11.red,   x, y),
		(uint8_t)bilerp((int32_t)p00.green, (int32_t)p10.green, (int32_t)p01.green, (int32_t)p11.green, x, y),
		(uint8_t)bilerp((int32_t)p00.blue,  (int32_t)p10.blue,  (int32_t)p01.blue,  (int32_t)p11.blue,  x, y),
		(uint8_t)bilerp((int32_t)p00.alpha, (int32_t)p10.alpha, (int32_t)p01.alpha, (int32_t)p11.alpha, x, y)
	}; 
}

cc0::gfx::RGBA32 cc0::gfx::sample_nearest(const cc0::gfx::Image &pImage, int32_t pU, int32_t pV)
{
	return get_color(pImage, Point<int32_t>{ pU >> 15, pV >> 15 });
}

cc0::gfx::RGBA32 cc0::gfx::sample_bilinear(const cc0::gfx::Image &pImage, int32_t pU, int32_t pV)
{
	const int32_t x = pU >> 15;
	const int32_t y = pV >> 15;
	const RGBA32 c00 = get_color(pImage, Point<int32_t>{ x,     y });
	const RGBA32 c01 = get_color(pImage, Point<int32_t>{ x,     y + 1 });
	const RGBA32 c10 = get_color(pImage, Point<int32_t>{ x + 1, y });
	const RGBA32 c11 = get_color(pImage, Point<int32_t>{ x + 1, y + 1 });
	return rgba_bilerp(c00, c10, c01, c11, pU & 0x7fff, pV & 0x7fff);
}

cc0::gfx::Image cc0::gfx::new_image(void *pixels, int32_t width, int32_t height, cc0::gfx::Encoder encoder, cc0::gfx::Decoder decoder, cc0::gfx::Indexer indexer)
{
	return Image{
		(uint8_t*)pixels,
		width, height,
		encoder, decoder, indexer
	};
}

cc0::gfx::RGBA32 cc0::gfx::get_color(const cc0::gfx::Image &src, cc0::gfx::Point<int32_t> p)
{
	return src.decode(src, p);
}

void cc0::gfx::set_color(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, cc0::gfx::RGBA32 c)
{
	dst.encode(dst, p, c);
}

void cc0::gfx::fill_rect(cc0::gfx::Image &dst, cc0::gfx::Rect<int32_t> dst_rect, cc0::gfx::RGBA32 color, cc0::gfx::Shader shader, Rect<int32_t> write_rect)
{
	write_rect = internal::clip(internal::order(write_rect), Rect<int32_t>{ Point<int32_t>{ 0, 0 }, Point<int32_t>{ dst.width, dst.height } });
	dst_rect = internal::clip(internal::order(dst_rect), write_rect);
	for (int32_t y = dst_rect.a.y; y < dst_rect.b.y; ++y) {
		for (int32_t x = dst_rect.a.x; x < dst_rect.b.x; ++x) {
			const Point<int32_t> p = { x, y };
			set_color(dst, p, shader(get_color(dst, p), color));
		}
	}
}

void cc0::gfx::draw_line(cc0::gfx::Image &pDst, int32_t pX1, int32_t pY1, cc0::gfx::RGBA32 pColor1, int32_t pX2, int32_t pY2, cc0::gfx::RGBA32 pColor2, cc0::gfx::Shader shader)
{
	const float r1 = pColor1.red;
	const float g1 = pColor1.green;
	const float b1 = pColor1.blue;
	const float a1 = pColor1.alpha;
	const float r2 = pColor2.red;
	const float g2 = pColor2.green;
	const float b2 = pColor2.blue;
	const float a2 = pColor2.alpha;
	
	const float xdiff = (float)(pX2 - pX1);
	const float ydiff = (float)(pY2 - pY1);
	
	if (xdiff == 0.0f && ydiff == 0.0f) {
		if (pX1 >= 0 && pX1 < pDst.width && pY1 >= 0 && pY1 < pDst.height) {
			RGBA32 color = { (uint8_t)r1, (uint8_t)g1, (uint8_t)b1, (uint8_t)a1 };
			set_color(pDst, Point<int32_t>{ pX1, pY1 }, shader(get_color(pDst, Point<int32_t>{ pX1, pY1 }), color));
		}
		return;
	}
	
	if (fabs(xdiff) > fabs(ydiff)) {
		float xmin, xmax;
		
		// set xmin to the lower x value given
		// and xmax to the higher value
		if (pX1 < pX2) {
			xmin = (float)pX1;
			xmax = (float)pX2;
		} else {
			xmin = (float)pX2;
			xmax = (float)pX1;
		}
		xmin = xmin > 0.0f ? xmin : 0.0f; // clipping
		xmax = xmax < (pDst.width - 1) ? xmax : (pDst.width - 1); // clipping
		
		// draw line in terms of y slope
		const float slope = ydiff / xdiff;
		for (float x = xmin; x <= xmax; x += 1.0f) {
			const float y = pY1 + ((x - pX1) * slope);
			if (y < 0.0f || y >= pDst.height) {
				continue; // clipping
			}
			const RGBA32 color = {
				(uint8_t)(r1 + ((r2 - r1) * ((x - pX1) / xdiff))),
				(uint8_t)(g1 + ((g2 - g1) * ((x - pX1) / xdiff))),
				(uint8_t)(b1 + ((b2 - b1) * ((x - pX1) / xdiff))),
				(uint8_t)(a1 + ((a2 - a1) * ((x - pX1) / xdiff)))
			};
			const Point<int32_t> p = { (int32_t)x, (int32_t)y };
			set_color(pDst, p, shader(get_color(pDst, p), color));
		}
	} else {
		float ymin, ymax;
		
		// set ymin to the lower y value given
		// and ymax to the higher value
		if (pY1 < pY2) {
			ymin = (float)pY1;
			ymax = (float)pY2;
		} else {
			ymin = (float)pY2;
			ymax = (float)pY1;
		}
		ymin = ymin > 0.0f              ? ymin : 0.0f;              // clipping
		ymax = ymax < (pDst.height - 1) ? ymax : (pDst.height - 1); // clipping
		
		// draw line in terms of x slope
		const float slope = xdiff / ydiff;
		for (float y = ymin; y <= ymax; y += 1.0f) {
			const float x = pX1 + ((y - pY1) * slope);
			if (x < 0.0f || x >= pDst.width) {
				continue; // clipping
			}
			const RGBA32 color = {
				(uint8_t)(r1 + ((r2 - r1) * ((y - pY1) / ydiff))),
				(uint8_t)(g1 + ((g2 - g1) * ((y - pY1) / ydiff))),
				(uint8_t)(b1 + ((b2 - b1) * ((y - pY1) / ydiff))),
				(uint8_t)(a1 + ((a2 - a1) * ((y - pY1) / ydiff)))
			};
			const Point<int32_t> p = { (int32_t)x, (int32_t)y };
			set_color(pDst, p, shader(get_color(pDst, p), color));
		}
	}
}

void cc0::gfx::stretch_image(cc0::gfx::Image &dst, cc0::gfx::Rect<int32_t> dst_rect, const cc0::gfx::Image &src, cc0::gfx::Rect<int32_t> src_rect, cc0::gfx::Rect<int32_t> write_rect)
{
	write_rect = internal::clip(internal::order(write_rect), Rect<int32_t>{ Point<int32_t>{ 0, 0 }, Point<int32_t>{ dst.width, dst.height } });

	if (src_rect.a.x == src_rect.b.x) { return; }
	if (src_rect.a.y == src_rect.b.y) { return; }
	if (dst_rect.a.x == dst_rect.b.x) { return; }
	if (dst_rect.a.y == dst_rect.b.y) { return; }

	src_rect = internal::clip(src_rect, Rect<int32_t>{ Point<int32_t>{ 0, 0 }, Point<int32_t>{ src.width, src.height } });

	if (dst_rect.a.x > dst_rect.b.x) {
		internal::swap(dst_rect.a.x, dst_rect.b.x);
		internal::swap(src_rect.a.x, src_rect.b.x);
	}
	if (dst_rect.a.y > dst_rect.b.y) {
		internal::swap(dst_rect.a.y, dst_rect.b.y);
		internal::swap(src_rect.a.y, src_rect.b.y);
	}

	if (dst_rect.b.x < write_rect.a.x || dst_rect.a.x >= write_rect.b.x) { return; }
	if (dst_rect.b.y < write_rect.a.x || dst_rect.a.y >= write_rect.b.y) { return; }

	const int32_t dsx = ((src_rect.b.x - src_rect.a.x) << 15) / (dst_rect.b.x - dst_rect.a.x);
	const int32_t dsy = ((src_rect.b.y - src_rect.a.y) << 15) / (dst_rect.b.y - dst_rect.a.y);

	int32_t ssx = dsx >= 0 ? (internal::min(src_rect.a.x, src_rect.b.x) << 15) : ((internal::max(src_rect.a.x, src_rect.b.x) << 15) + dsx);
	if (dst_rect.a.x < write_rect.a.x) {
		ssx += dsx * (write_rect.a.x - dst_rect.a.x);
		dst_rect.a.x = write_rect.a.x;
	}
	if (dst_rect.b.x >= write_rect.b.x) {
		dst_rect.b.x = write_rect.b.x;
	}

	int32_t ssy = dsy >= 0 ? (internal::min(src_rect.a.y, src_rect.b.y) << 15) : ((internal::max(src_rect.a.y, src_rect.b.y) << 15) + dsy);
	if (dst_rect.a.y < write_rect.a.y) {
		ssy += dsy * (write_rect.a.y - dst_rect.a.y);
		dst_rect.a.y = write_rect.a.y;
	}
	if (dst_rect.b.y >= write_rect.b.y) {
		dst_rect.b.y = write_rect.b.y;
	}

	for (int32_t dy = dst_rect.a.y, sy = 0; dy < dst_rect.b.y; ++dy, sy += dsy) {
		for (int32_t dx = dst_rect.a.x, sx = 0; dx < dst_rect.b.x; ++dx, sx += dsx) {
			set_color(
				dst,
				Point<int32_t>{ dx, dy },
				get_color(
					src,
					Point<int32_t>{
						(ssx + sx) >> 15,
						(ssy + sy) >> 15
					}
				)
			);
		}
	}
}

void cc0::gfx::stretch_image(Image &dst, Rect<int32_t> dst_rect, const Image &src, Shader shader, Sampler sampler, Rect<int32_t> src_rect, Rect<int32_t> write_rect)
{
	write_rect = internal::clip(internal::order(write_rect), Rect<int32_t>{ Point<int32_t>{ 0, 0 }, Point<int32_t>{ dst.width, dst.height } });

	if (src_rect.a.x == src_rect.b.x) { return; }
	if (src_rect.a.y == src_rect.b.y) { return; }
	if (dst_rect.a.x == dst_rect.b.x) { return; }
	if (dst_rect.a.y == dst_rect.b.y) { return; }

	src_rect = internal::clip(src_rect, Rect<int32_t>{ Point<int32_t>{ 0, 0 }, Point<int32_t>{ src.width, src.height } });

	if (dst_rect.a.x > dst_rect.b.x) {
		internal::swap(dst_rect.a.x, dst_rect.b.x);
		internal::swap(src_rect.a.x, src_rect.b.x);
	}
	if (dst_rect.a.y > dst_rect.b.y) {
		internal::swap(dst_rect.a.y, dst_rect.b.y);
		internal::swap(src_rect.a.y, src_rect.b.y);
	}

	if (dst_rect.b.x < write_rect.a.x || dst_rect.a.x >= write_rect.b.x) { return; }
	if (dst_rect.b.y < write_rect.a.x || dst_rect.a.y >= write_rect.b.y) { return; }

	const int32_t dsx = ((src_rect.b.x - src_rect.a.x) << 15) / (dst_rect.b.x - dst_rect.a.x);
	const int32_t dsy = ((src_rect.b.y - src_rect.a.y) << 15) / (dst_rect.b.y - dst_rect.a.y);

	int32_t ssx = dsx >= 0 ? (internal::min(src_rect.a.x, src_rect.b.x) << 15) : ((internal::max(src_rect.a.x, src_rect.b.x) << 15) + dsx);
	if (dst_rect.a.x < write_rect.a.x) {
		ssx += dsx * (write_rect.a.x - dst_rect.a.x);
		dst_rect.a.x = write_rect.a.x;
	}
	if (dst_rect.b.x >= write_rect.b.x) {
		dst_rect.b.x = write_rect.b.x;
	}

	int32_t ssy = dsy >= 0 ? (internal::min(src_rect.a.y, src_rect.b.y) << 15) : ((internal::max(src_rect.a.y, src_rect.b.y) << 15) + dsy);
	if (dst_rect.a.y < write_rect.a.y) {
		ssy += dsy * (write_rect.a.y - dst_rect.a.y);
		dst_rect.a.y = write_rect.a.y;
	}
	if (dst_rect.b.y >= write_rect.b.y) {
		dst_rect.b.y = write_rect.b.y;
	}

	for (int32_t dy = dst_rect.a.y, sy = 0; dy < dst_rect.b.y; ++dy, sy += dsy) {
		for (int32_t dx = dst_rect.a.x, sx = 0; dx < dst_rect.b.x; ++dx, sx += dsx) {
			set_color(
				dst,
				Point<int32_t>{ dx, dy },
				shader(
					get_color(dst, Point<int32_t>{ dx, dy }),
					sampler(src, ssx + sx, ssy + sy)
				)
			);
		}
	}
}

#define arr(X) ((int32_t*)(&(X)))

void cc0::gfx::fill_span(cc0::gfx::Image &dst, int32_t dst_axis, cc0::gfx::Span<int32_t> dst_span, cc0::gfx::RGBA32 color, cc0::gfx::Shader shader, cc0::gfx::Rect<int32_t> write_rect)
{
	write_rect = internal::clip(internal::order(write_rect), Rect<int32_t>{ Point<int32_t>{ 0, 0 }, Point<int32_t>{ dst.width, dst.height } });

	const int32_t fixed_axis = (dst_span.axis + 1) & 1;
	const int32_t float_axis = dst_span.axis & 1;

	if (dst_axis < arr(write_rect.a)[fixed_axis] || dst_axis >= arr(write_rect.b)[fixed_axis]) { return; }

	if (dst_span.a > dst_span.b) { internal::swap(dst_span.a, dst_span.b); }
	dst_span.a = internal::max(dst_span.a, arr(write_rect.a)[float_axis]);
	dst_span.b = internal::min(dst_span.b, arr(write_rect.b)[float_axis]);

	Point<int32_t> p = {
		fixed_axis == 0 ? dst_axis : dst_span.a,
		fixed_axis == 0 ? dst_span.a : dst_axis
	};
	for (; arr(p)[float_axis] < dst_span.b; ++arr(p)[float_axis]) {
		set_color(dst, p, shader(get_color(dst, p), color));
	}
}

void cc0::gfx::stretch_span(cc0::gfx::Image &dst, int32_t dst_axis, cc0::gfx::Span<int32_t> dst_span, const cc0::gfx::Image &src, cc0::gfx::Shader shader, cc0::gfx::Sampler sampler, int32_t src_axis, cc0::gfx::Span<int32_t> src_span, cc0::gfx::Rect<int32_t> write_rect)
{
	write_rect = internal::clip(internal::order(write_rect), Rect<int32_t>{ Point<int32_t>{ 0, 0 }, Point<int32_t>{ dst.width, dst.height } });

	const int32_t d_fixed_axis = (dst_span.axis + 1) & 1;
	const int32_t d_float_axis = dst_span.axis & 1;

	if (dst_axis < arr(write_rect.a)[d_fixed_axis] || dst_axis >= arr(write_rect.b)[d_fixed_axis]) { return; }

	if (dst_span.a > dst_span.b) {
		internal::swap(dst_span.a, dst_span.b);
		internal::swap(src_span.a, src_span.b);
	}

	const int32_t s_fixed_axis = (src_span.axis + 1) & 1;
	const int32_t s_float_axis = src_span.axis & 1;

	const int32_t dss = ((src_span.b - src_span.a) << 15) / (dst_span.b - dst_span.a);
	
	Point<int32_t> s = {
		(s_fixed_axis == 0 ? src_axis : src_span.a) << 15,
		(s_fixed_axis == 0 ? src_span.a : src_axis) << 15
	};

	if (dst_span.a < arr(write_rect.a)[d_float_axis]) {
		arr(s)[s_float_axis] += dss * (write_rect.a.y - dst_span.a);
		dst_span.a = write_rect.a.y;
	}
	dst_span.b = internal::min(dst_span.b, write_rect.b.y);

	Point<int32_t> p = {
		d_fixed_axis == 0 ? dst_axis : dst_span.a,
		d_fixed_axis == 0 ? dst_span.a : dst_axis
	};
	for (; arr(p)[d_float_axis] < dst_span.b; ++arr(p)[d_float_axis], arr(s)[s_float_axis] += dss) {
		set_color(dst, p, shader(get_color(dst, p), sampler(src, s.x, s.y)));
	}
}

void cc0::gfx::stretch_span(cc0::gfx::Image &dst, int32_t dst_axis, cc0::gfx::Span<int32_t> dst_span, const cc0::gfx::Image &src, cc0::gfx::Shader shader, cc0::gfx::Sampler sampler, cc0::gfx::Line<fixed32_t> src_line, cc0::gfx::Rect<int32_t> write_rect)
{
	write_rect = internal::clip(internal::order(write_rect), Rect<int32_t>{ Point<int32_t>{ 0, 0 }, Point<int32_t>{ dst.width, dst.height } });

	const int32_t fixed_axis = (dst_span.axis + 1) & 1;
	const int32_t float_axis = dst_span.axis & 1;

	if (dst_axis < arr(write_rect.a)[fixed_axis] || dst_axis >= arr(write_rect.b)[fixed_axis]) { return; }

	if (dst_span.a > dst_span.b) {
		internal::swap(dst_span.a, dst_span.b);
		internal::swap(src_line.a, src_line.b);
	}

	const int32_t dsx = ((src_line.b.x - src_line.a.x) << 15) / (dst_span.b - dst_span.a);
	const int32_t dsy = ((src_line.b.y - src_line.a.y) << 15) / (dst_span.b - dst_span.a);
	
	Point<int32_t> s = {
		src_line.a.x.x,
		src_line.a.y.x
	};

	if (dst_span.a < arr(write_rect.a)[float_axis]) {
		s.x += dsx * (write_rect.a.y - dst_span.a);
		s.y += dsy * (write_rect.a.y - dst_span.a);
		dst_span.a = write_rect.a.y;
	}
	dst_span.b = internal::min(dst_span.b, write_rect.b.y);

	Point<int32_t> p = {
		fixed_axis == 0 ? dst_axis : dst_span.a,
		fixed_axis == 0 ? dst_span.a : dst_axis
	};
	for (; arr(p)[float_axis] < dst_span.b; ++arr(p)[float_axis], s.x += dsx, s.y += dsy) {
		set_color(dst, p, shader(get_color(dst, p), sampler(src, s.x, s.y)));
	}
}

#undef arr

int32_t cc0::gfx::print_text(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> p, const char *text, int32_t text_len, cc0::gfx::RGBA32 color, int32_t scale, cc0::gfx::Rect<int32_t> write_rect)
{
	if (scale <= 0) { return p.x; }

	static const Image src = new_image(
		FONT_ATLAS,
		FONT_ATLAS_CHAR_WIDTH_COUNT * FONT_CELL_PX_WIDTH, FONT_ATLAS_CHAR_HEIGHT_COUNT * FONT_CELL_PX_HEIGHT,
		encode_1bpp, decode_1bpp, index_linear
	);
	const int32_t CHAR_DST_WIDTH  = FONT_CHAR_PX_WIDTH  * scale;
	const int32_t CHAR_DST_HEIGHT = FONT_CHAR_PX_HEIGHT * scale;
	for (int32_t i = 0; i < text_len && text[i] != 0 && p.x < write_rect.b.x; ++i, p.x += CHAR_DST_WIDTH) {
		if (text[i] != ' ') {
			const int32_t sy = FONT_CELL_PX_HEIGHT * ((int32_t)text[i] - FONT_CHAR_ASCII_START);
			stretch_image(
				dst, Rect<int32_t>{ Point<int32_t>{ p.x, p.y }, Point<int32_t>{ p.x + CHAR_DST_WIDTH, p.y + CHAR_DST_HEIGHT } },
				src, shade_stencil, sample_nearest, Rect<int32_t>{ Point<int32_t>{ 0, sy }, Point<int32_t>{ FONT_CELL_PX_WIDTH, sy + FONT_CELL_PX_HEIGHT } },
				write_rect
			);
		}
	}
	return p.x;
}

/*void internal_impl::DrawTriangle(tiny3d::Image &dst, const tiny3d::Array<float> *zread, tiny3d::Array<float> *zwrite, const internal_impl::IVertex &a, const internal_impl::IVertex &b, const internal_impl::IVertex &c, const tiny3d::Texture *tex, const tiny3d::URect *dst_rect)
{
	// AABB Clipping
	int32_t min_y = tiny3d::Max(tiny3d::Min(a.p.y, b.p.y, c.p.y), int32_t(0));
	int32_t max_y = tiny3d::Min(tiny3d::Max(a.p.y, b.p.y, c.p.y), int32_t(dst.GetHeight() - 1));
	if (max_y - min_y <= 0) { return; }
	int32_t min_x = tiny3d::Max(tiny3d::Min(a.p.x, b.p.x, c.p.x), int32_t(0));
	int32_t max_x = tiny3d::Min(tiny3d::Max(a.p.x, b.p.x, c.p.x), int32_t(dst.GetWidth() - 1));
	if (max_x - min_x <= 0) { return; }

	if (dst_rect != nullptr) {
		min_y = int32_t(tiny3d::Max(uint32_t(min_y), dst_rect->a.y));
		max_y = int32_t(tiny3d::Min(uint32_t(max_y), dst_rect->b.y - 1));
		min_x = int32_t(tiny3d::Max(uint32_t(min_x), dst_rect->a.x));
		max_x = int32_t(tiny3d::Min(uint32_t(max_x), dst_rect->b.x - 1));
	}

	// Triangle setup
	Point p    = { min_x, min_y };
	int64_t         w0_y = DetermineHalfspace(b.p, c.p, p);
	int64_t         w1_y = DetermineHalfspace(c.p, a.p, p);
	int64_t         w2_y = DetermineHalfspace(a.p, b.p, p);

//	if (ShouldDivide(w0_y + w1_y + w2_y)) {
//		DrawSubdivTri(dst, zbuf, a, b, c, tex, dst_rect);
//		return;
//	}

	// Interpolation/triangle setup
	const int32_t w2_x_inc        = a.p.y - b.p.y;
	const int32_t w2_y_inc        = b.p.x - a.p.x;
	const int32_t w0_x_inc        = b.p.y - c.p.y;
	const int32_t w0_y_inc        = c.p.x - b.p.x;
	const int32_t w1_x_inc        = c.p.y - a.p.y;
	const int32_t w1_y_inc        = a.p.x - c.p.x;
	const float sum_inv_area_x2 = 1.0f / int32_t(w0_y + w1_y + w2_y);
	float l0_y           = int32_t(w0_y) * sum_inv_area_x2;
	float l1_y           = int32_t(w1_y) * sum_inv_area_x2;
	float l2_y           = int32_t(w2_y) * sum_inv_area_x2;
	const float l0_x_inc = w0_x_inc * sum_inv_area_x2;
	const float l1_x_inc = w1_x_inc * sum_inv_area_x2;
	const float l2_x_inc = w2_x_inc * sum_inv_area_x2;
	const float l0_y_inc = w0_y_inc * sum_inv_area_x2;
	const float l1_y_inc = w1_y_inc * sum_inv_area_x2;
	const float l2_y_inc = w2_y_inc * sum_inv_area_x2;

	w0_y += IsTopLeft(b.p, c.p) ? 0 : -1; // add offsets to coordinates to enforce fill convention
	w1_y += IsTopLeft(c.p, a.p) ? 0 : -1;
	w2_y += IsTopLeft(a.p, b.p) ? 0 : -1;

	for (p.y = min_y; p.y <= max_y; ++p.y) {

		int32_t w0 = int32_t(w0_y);
		int32_t w1 = int32_t(w1_y);
		int32_t w2 = int32_t(w2_y);

		float l0 = l0_y;
		float l1 = l1_y;
		float l2 = l2_y;

		for (p.x = min_x; p.x <= max_x; ++p.x) {

			if ((w0 | w1 | w2) >= 0) {

				const UPoint q     = { uint32_t(p.x), uint32_t(p.y) };
				const Color  pixel = dst.GetColor(q);
				const uint32_t   zi    = q.x + q.y * dst.GetWidth();
				const float  sz    = 1.0f / (a.w * l0 + b.w * l1 + c.w * l2);
				const float  dz    = (zread != nullptr) ? (*zread)[zi] : std::numeric_limits<float>::infinity();

				if (sz <= dz && pixel.blend != Color::Transparent) { // use transparency bit as a 1-bit stencil

					const float L0 = l0 * sz;
					const float L1 = l1 * sz;
					const float L2 = l2 * sz;

					const Color col = {
						Byte(a.r * L0 + b.r * L1 + c.r * L2),
						Byte(a.g * L0 + b.g * L1 + c.g * L2),
						Byte(a.b * L0 + b.b * L1 + c.b * L2),
						Color::Solid
					};

					const Color texel = (tex != nullptr) ? tex->GetColor(UPoint{ uint32_t(a.u * L0 + b.u * L1 + c.u * L2), uint32_t(a.v * L0 + b.v * L1 + c.v * L2) }) : Color{ 255, 255, 255, Color::Solid };
//					const Color texel = (tex != nullptr) ? tex->GetColor(Dither2x2(Vector2{ a.u * L0 + b.u * L1 + c.u * L2, a.v * L0 + b.v * L1 + c.v * L2}, q)) : Color{ 255, 255, 255, Color::Solid }; // Dithered texture filtering (can look good if texture is relatively high resolution)

					switch (texel.blend)
					{
					case Color::Solid:
						dst.SetColor(q, Dither2x2(texel * col, q));
						if (zwrite != nullptr) { (*zwrite)[zi] = sz; }
						break;
					case Color::AddAlpha:
						dst.SetColor(q, Dither2x2(pixel + texel * col, q));
						break;
					case Color::Emissive:
						dst.SetColor(q, texel);
						if (zwrite != nullptr) { (*zwrite)[zi] = sz; }
						break;
					case Color::EmissiveAddAlpha:
						dst.SetColor(q, Dither2x2(pixel + texel, q));
						break;
					default: break;
					}
				}
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
}*/

void cc0::gfx::fill_tri(cc0::gfx::Image &dst, cc0::gfx::Point<int32_t> a, cc0::gfx::Point<int32_t> b, cc0::gfx::Point<int32_t> c, cc0::gfx::RGBA32 color, cc0::gfx::Shader shader, cc0::gfx::Rect<int32_t> write_rect)
{
	write_rect = internal::clip(internal::order(write_rect), Rect<int32_t>{ Point<int32_t>{ 0, 0 }, Point<int32_t>{ dst.width, dst.height } });

	// AABB Clipping
	const int32_t min_y = internal::max(internal::min(a.y, b.y, c.y), write_rect.a.y);
	const int32_t max_y = internal::min(internal::max(a.y, b.y, c.y), write_rect.b.y - 1);
	if (max_y - min_y <= 0) { return; }
	const int32_t min_x = internal::max(internal::min(a.x, b.x, c.x), write_rect.a.x);
	const int32_t max_x = internal::min(internal::max(a.x, b.x, c.x), write_rect.b.x - 1);
	if (max_x - min_x <= 0) { return; }

	// Triangle setup
	Point<int32_t> p    = { min_x, min_y };
	int64_t        w0_y = internal::determine_halfspace(b, c, p) + (int64_t(internal::is_top_left(b, c)) - 1);
	int64_t        w1_y = internal::determine_halfspace(c, a, p) + (int64_t(internal::is_top_left(c, a)) - 1);
	int64_t        w2_y = internal::determine_halfspace(a, b, p) + (int64_t(internal::is_top_left(a, b)) - 1);

	// Interpolation/triangle setup
	const int32_t w2_x_inc = a.y - b.y;
	const int32_t w2_y_inc = b.x - a.x;
	const int32_t w0_x_inc = b.y - c.y;
	const int32_t w0_y_inc = c.x - b.x;
	const int32_t w1_x_inc = c.y - a.y;
	const int32_t w1_y_inc = a.x - c.x;

	for (p.y = min_y; p.y <= max_y; ++p.y) {

		int32_t w0 = int32_t(w0_y);
		int32_t w1 = int32_t(w1_y);
		int32_t w2 = int32_t(w2_y);

		for (p.x = min_x; p.x <= max_x; ++p.x) {

			if ((w0 | w1 | w2) >= 0) {
				set_color(dst, p, shader(get_color(dst, p), color));
			}

			w0 += w0_x_inc;
			w1 += w1_x_inc;
			w2 += w2_x_inc;
		}

		w0_y += w0_y_inc;
		w1_y += w1_y_inc;
		w2_y += w2_y_inc;
	}
}
