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

int32_t cc0::gfx::index_linear(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return (src.width * p.y) + p.x;
}

int32_t cc0::gfx::index_repeat_linear(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return index_linear(src, Point{ p.x % src.width, p.y % src.height });
}

int32_t cc0::gfx::index_z(const cc0::gfx::Image&, cc0::gfx::Point p)
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

int32_t cc0::gfx::index_repeat_z(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return index_z(src, Point{ p.x % src.width, p.y % src.height });
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
	const uint32_t     r          = (uint32_t(color.red) * FIX_SCALAR) >> 8;
	const uint32_t     g          = (uint32_t(color.green) * FIX_SCALAR) >> 8;
	const uint32_t     b          = (uint32_t(color.blue) * FIX_SCALAR) >> 8;
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
	const uint32_t     r          = (uint32_t(color.red) * FIX_SCALAR) >> 8;
	const uint32_t     g          = (uint32_t(color.green) * FIX_SCALAR) >> 8;
	const uint32_t     b          = (uint32_t(color.blue) * FIX_SCALAR) >> 8;
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

cc0::gfx::RGBA32 cc0::gfx::decode_RGBA16(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return decode_RGBA16(src.pixels + src.index(src, p) * 2);
}

cc0::gfx::RGBA32 cc0::gfx::decode_RGB24(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return decode_RGB24(src.pixels + src.index(src, p) * 3);
}

cc0::gfx::RGBA32 cc0::gfx::decode_RGBA32(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return decode_RGBA32(src.pixels + src.index(src, p) * 3);
}

cc0::gfx::RGBA32 cc0::gfx::decode_BGRA16(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return decode_BGRA16(src.pixels + src.index(src, p) * 2);
}

cc0::gfx::RGBA32 cc0::gfx::decode_BGR24 (const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return decode_BGR24(src.pixels + src.index(src, p) * 3);
}

cc0::gfx::RGBA32 cc0::gfx::decode_BGRA32(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return decode_BGRA32(src.pixels + src.index(src, p) * 4);
}

cc0::gfx::RGBA32 cc0::gfx::decode_ARGB16(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return decode_ARGB16(src.pixels + src.index(src, p) * 2);
}

cc0::gfx::RGBA32 cc0::gfx::decode_ARGB32(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return decode_ARGB32(src.pixels + src.index(src, p) * 4);
}

cc0::gfx::RGBA32 cc0::gfx::decode_ABGR16(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return decode_ABGR16(src.pixels + src.index(src, p) * 2);
}

cc0::gfx::RGBA32 cc0::gfx::decode_ABGR32(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return decode_ABGR32(src.pixels + src.index(src, p) * 4);
}

cc0::gfx::RGBA32 cc0::gfx::decode_1bpp(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	const uint32_t INDEX = src.index(src, p);
	const uint8_t  MASK = decode_1bpp(((const uint8_t*)src.pixels)[INDEX >> 3], INDEX & 7);
	return RGBA32{ MASK, MASK, MASK, MASK };
}

void cc0::gfx::encode_RGBA16(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 color)
{
	encode_RGBA16(color, dst.pixels + dst.index(dst, p) * 2);
}

void cc0::gfx::encode_RGB24(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 color)
{
	encode_RGB24(color, dst.pixels + dst.index(dst, p) * 3);
}

void cc0::gfx::encode_RGBA32(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 color)
{
	encode_RGBA32(color, dst.pixels + dst.index(dst, p) * 4);
}

void cc0::gfx::encode_BGRA16(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 color)
{
	encode_BGRA16(color, dst.pixels + dst.index(dst, p) * 2);
}

void cc0::gfx::encode_BGR24(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 color)
{
	encode_BGR24(color, dst.pixels + dst.index(dst, p) * 3);
}

void cc0::gfx::encode_BGRA32(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 color)
{
	encode_BGRA32(color, dst.pixels + dst.index(dst, p) * 4);
}

void cc0::gfx::encode_ARGB16(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 color)
{
	encode_ARGB16(color, dst.pixels + dst.index(dst, p) * 2);
}

void cc0::gfx::encode_ARGB32(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 color)
{
	encode_ARGB32(color, dst.pixels + dst.index(dst, p) * 4);
}

void cc0::gfx::encode_ABGR16(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 color)
{
	encode_ABGR16(color, dst.pixels + dst.index(dst, p) * 2);
}

void cc0::gfx::encode_ABGR32(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 color)
{
	encode_ABGR32(color, dst.pixels + dst.index(dst, p) * 4);
}

void cc0::gfx::encode_1bpp(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 color)
{
	const uint32_t INDEX = dst.index(dst, p);
	encode_1bpp(color.alpha > 0, ((uint8_t*)dst.pixels) + (INDEX >> 3), INDEX & 7);
}

cc0::gfx::RGBA32 cc0::gfx::shade_set(cc0::gfx::RGBA32, cc0::gfx::RGBA32 src)
{
	return src;
}

cc0::gfx::RGBA32 cc0::gfx::shade_alpha(cc0::gfx::RGBA32 pDst, cc0::gfx::RGBA32 pSrc)
{
	pDst.red   += (pSrc.alpha * (pSrc.red   - pDst.red)   >> CHAR_BIT);
	pDst.green += (pSrc.alpha * (pSrc.green - pDst.green) >> CHAR_BIT);
	pDst.blue  += (pSrc.alpha * (pSrc.blue  - pDst.blue)  >> CHAR_BIT);
	pDst.alpha += (pSrc.alpha * (pSrc.alpha - pDst.alpha) >> CHAR_BIT);
	return pDst;
}

cc0::gfx::RGBA32 cc0::gfx::shade_colorkey(cc0::gfx::RGBA32 pDst, cc0::gfx::RGBA32 pSrc)
{
	static const RGBA32   COLORKEY  = cc0::gfx::RGBA32{ 255, 0, 255, 0 };
	static const uint32_t KEY       = *(uint32_t*)(&COLORKEY);
	static const RGBA32   ALPHAMASK = cc0::gfx::RGBA32{ 255, 255, 255, 0 };
	static const uint32_t MASK      = *(uint32_t*)(&ALPHAMASK);
	return (((*(uint32_t*)(&pSrc)) & MASK) == KEY) ? pDst : pSrc;
}

cc0::gfx::RGBA32 cc0::gfx::shade_gray(cc0::gfx::RGBA32, cc0::gfx::RGBA32 pSrc)
{
	const uint8_t GRAY = (uint8_t)(
		(float)pSrc.red   * 0.30f +
		(float)pSrc.green * 0.59f +
		(float)pSrc.blue  * 0.11f
	);
	return RGBA32{ GRAY, GRAY, GRAY, pSrc.alpha };
}

cc0::gfx::RGBA32 cc0::gfx::shade_grayalpha(cc0::gfx::RGBA32 pDst, cc0::gfx::RGBA32 pSrc)
{
	return shade_alpha(pDst, shade_gray(pDst, pSrc));
}

cc0::gfx::RGBA32 cc0::gfx::shade_graycolorkey(cc0::gfx::RGBA32 pDst, cc0::gfx::RGBA32 pSrc)
{
	static const RGBA32   COLORKEY  = cc0::gfx::RGBA32{ 255, 0, 255, 0 };
	static const uint32_t KEY       = *(uint32_t*)(&COLORKEY);
	static const RGBA32   ALPHAMASK = cc0::gfx::RGBA32{ 255, 255, 255, 0 };
	static const uint32_t MASK      = *(uint32_t*)(&ALPHAMASK);
	return (((*(uint32_t*)(&pSrc)) & MASK) == KEY) ? pDst : shade_gray(pDst, pSrc);
}

cc0::gfx::RGBA32 cc0::gfx::shade_stencil(cc0::gfx::RGBA32 dst, cc0::gfx::RGBA32 src)
{
	return src.alpha > 0 ? src : dst;
}

//cc0::gfx::RGBA32 cc0::gfx::sample_bilinear(const cc0::gfx::Image &pImage, float pU, float pV)
//{
//	const float   fU = pU * (pImage.width  - 2); // [ ] Similar to how we need to access in the fixed-point version, but width - 1 instead.
//	const float   fV = pV * (pImage.height - 2); // [ ] Similar to how we need to access in the fixed-point version, but height - 1 instead.
//	const int32_t iU = (int32_t)fU;
//	const int32_t iV = (int32_t)fV;
//	
//	const float u_ratio    = fU - iU;
//	const float v_ratio    = fV - iV;
//	const float u_opposite = 1.0f - u_ratio;
//	const float v_opposite = 1.0f - v_ratio;
//	
//	const RGBA32 c00 = get_color(pImage, Point{ iU,     iV });
//	const RGBA32 c01 = get_color(pImage, Point{ iU,     iV + 1 });
//	const RGBA32 c10 = get_color(pImage, Point{ iU + 1, iV });
//	const RGBA32 c11 = get_color(pImage, Point{ iU + 1, iV + 1 });
//	
//	return RGBA32{
//		(uint8_t)((c00.red   * u_opposite + c10.red   * u_ratio) * v_opposite + (c01.red   * u_opposite + c11.red   * u_ratio) * v_ratio),
//		(uint8_t)((c00.green * u_opposite + c10.green * u_ratio) * v_opposite + (c01.green * u_opposite + c11.green * u_ratio) * v_ratio),
//		(uint8_t)((c00.blue  * u_opposite + c10.blue  * u_ratio) * v_opposite + (c01.blue  * u_opposite + c11.blue  * u_ratio) * v_ratio),
//		(uint8_t)((c00.alpha * u_opposite + c10.alpha * u_ratio) * v_opposite + (c01.alpha * u_opposite + c11.alpha * u_ratio) * v_ratio)
//	};
//}

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
	return get_color(pImage, Point{ pU >> 15, pV >> 15 });
}

cc0::gfx::RGBA32 cc0::gfx::sample_bilinear(const cc0::gfx::Image &pImage, int32_t pU, int32_t pV)
{
	const int32_t x = pU >> 15;
	const int32_t y = pV >> 15;
	const RGBA32 c00 = get_color(pImage, Point{ x,     y });
	const RGBA32 c01 = get_color(pImage, Point{ x,     y + 1 });
	const RGBA32 c10 = get_color(pImage, Point{ x + 1, y });
	const RGBA32 c11 = get_color(pImage, Point{ x + 1, y + 1 });
	return rgba_bilerp(c00, c10, c01, c11, pU & 0x7fff, pV & 0x7fff);
}

cc0::gfx::Image cc0::gfx::new_image(void *pixels, int32_t width, int32_t height, int32_t bytes_per_pixel, cc0::gfx::Encoder encoder, cc0::gfx::Decoder decoder, cc0::gfx::Indexer indexer)
{
	return Image{
		(uint8_t*)pixels,
		width, height,
		encoder, decoder, indexer
	};
}

cc0::gfx::RGBA32 cc0::gfx::get_color(const cc0::gfx::Image &src, cc0::gfx::Point p)
{
	return src.decode(src, p);
}

void cc0::gfx::set_color(cc0::gfx::Image &dst, cc0::gfx::Point p, cc0::gfx::RGBA32 c)
{
	dst.encode(dst, p, c);
}

void cc0::gfx::fill_rect(cc0::gfx::Image &dst, cc0::gfx::Rect dst_rect, cc0::gfx::RGBA32 color, cc0::gfx::Shader shader)
{
	dst_rect.a.x = 0          > dst_rect.a.x ? 0          : dst_rect.a.x;
	dst_rect.a.y = 0          > dst_rect.a.y ? 0          : dst_rect.a.y;
	dst_rect.b.x = dst.width  < dst_rect.b.x ? dst.width  : dst_rect.b.x;
	dst_rect.b.y = dst.height < dst_rect.b.y ? dst.height : dst_rect.b.y;
	
	for (int32_t y = dst_rect.a.y; y < dst_rect.b.y; ++y) {
		for (int32_t x = dst_rect.a.x; x < dst_rect.b.x; ++x) {
			const Point p = { x, y };
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
			set_color(pDst, Point{ pX1, pY1 }, shader(get_color(pDst, Point{ pX1, pY1 }), color));
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
			const Point p = { (int32_t)x, (int32_t)y };
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
			const Point p = { (int32_t)x, (int32_t)y };
			set_color(pDst, p, shader(get_color(pDst, p), color));
		}
	}
}

template < typename type_t >
void swap(type_t &a, type_t &b)
{
	type_t t = a;
	a = b;
	b = t;
}

template < typename type_t >
type_t min(type_t a, type_t b)
{
	return a < b ? a : b;
}

template < typename type_t >
type_t max(type_t a, type_t b)
{
	return a > b ? a : b;
}

template < typename type_t >
type_t clamp(type_t min, type_t x, type_t max)
{
	return ::max(min, ::min(x, max));
}

cc0::gfx::Rect order(cc0::gfx::Rect r)
{
	if (r.a.x > r.b.x) { swap(r.a.x, r.b.x); }
	if (r.a.y > r.b.y) { swap(r.a.y, r.b.x); }
	return r;
}

void cc0::gfx::stretch_image(cc0::gfx::Image &dst, cc0::gfx::Rect dst_rect, const cc0::gfx::Image &src, cc0::gfx::Rect src_rect, cc0::gfx::Rect write_rect)
{
	// [ ] we need to test:
	//	[ ] moving dst_rect off-screen in all orientations
	//	[ ] ensure write_rect works

	write_rect = order(write_rect);
	clamp(0, write_rect.a.x, dst.width);
	clamp(0, write_rect.b.x, dst.width);
	clamp(0, write_rect.a.y, dst.height);
	clamp(0, write_rect.b.y, dst.height);

	if (src_rect.a.x == src_rect.b.x) { return; }
	if (src_rect.a.y == src_rect.b.y) { return; }
	if (dst_rect.a.x == dst_rect.b.x) { return; }
	if (dst_rect.a.y == dst_rect.b.y) { return; }

	clamp(0, src_rect.a.x, src.width);
	clamp(0, src_rect.b.x, src.width);
	clamp(0, src_rect.a.y, src.height);
	clamp(0, src_rect.b.y, src.height);

	if (dst_rect.a.x > dst_rect.b.x) {
		swap(dst_rect.a.x, dst_rect.b.x);
		swap(src_rect.a.x, src_rect.b.x);
	}
	if (dst_rect.a.y > dst_rect.b.y) {
		swap(dst_rect.a.y, dst_rect.b.y);
		swap(src_rect.a.y, src_rect.b.y);
	}

	if (dst_rect.b.x < write_rect.a.x || dst_rect.a.x >= write_rect.b.x) { return; }
	if (dst_rect.b.y < write_rect.a.x || dst_rect.a.y >= write_rect.b.y) { return; }

	const int32_t dsx = ((src_rect.b.x - src_rect.a.x) << 15) / (dst_rect.b.x - dst_rect.a.x);
	const int32_t dsy = ((src_rect.b.y - src_rect.a.y) << 15) / (dst_rect.b.y - dst_rect.a.y);

	int32_t ssx = dsx >= 0 ? (min(src_rect.a.x, src_rect.b.x) << 15) : ((max(src_rect.a.x, src_rect.b.x) << 15) + dsx);
	if (dst_rect.a.x < write_rect.a.x) {
		if (dsx >= 0) { ssx += dsx * (dst_rect.a.x - write_rect.a.x); }
		dst_rect.a.x = write_rect.a.x;
	}
	if (dst_rect.b.x >= write_rect.b.x) {
		if (dsx < 0) { ssx += dsx * (write_rect.b.x - dst_rect.b.x); }
		dst_rect.b.x = write_rect.b.x;
	}

	int32_t ssy = dsy >= 0 ? (min(src_rect.a.y, src_rect.b.y) << 15) : ((max(src_rect.a.y, src_rect.b.y) << 15) + dsy);
	if (dst_rect.a.y < write_rect.a.y) {
		if (dsy >= 0) { ssy += dsy * (dst_rect.a.y - write_rect.a.y); }
		dst_rect.a.y = write_rect.a.y;
	}
	if (dst_rect.b.y >= write_rect.b.y) {
		if (dsy < 0) { ssy += dsy * (write_rect.b.y - dst_rect.b.y); }
		dst_rect.b.y = write_rect.b.y;
	}

	for (int32_t dy = dst_rect.a.y, sy = 0; dy < dst_rect.b.y; ++dy, sy += dsy) {
		for (int32_t dx = dst_rect.a.x, sx = 0; dx < dst_rect.b.x; ++dx, sx += dsx) {
			set_color(
				dst,
				Point{ dx, dy },
				get_color(
					src,
					Point{
						(ssx + sx) >> 15,
						(ssy + sy) >> 15
					}
				)
			);
		}
	}
}

void cc0::gfx::stretch_image(Image &dst, Rect dst_rect, const Image &src, Shader shader, Sampler sampler, Rect src_rect, Rect write_rect)
{
	// [ ] We probably need to interpolate normalized texture coordinates because bilinear needs to interpolate 0 - width-1, while nearest needs to interpolate 0 - width.

	write_rect = order(write_rect);
	clamp(0, write_rect.a.x, dst.width);
	clamp(0, write_rect.b.x, dst.width);
	clamp(0, write_rect.a.y, dst.height);
	clamp(0, write_rect.b.y, dst.height);

	if (src_rect.a.x == src_rect.b.x) { return; }
	if (src_rect.a.y == src_rect.b.y) { return; }
	if (dst_rect.a.x == dst_rect.b.x) { return; }
	if (dst_rect.a.y == dst_rect.b.y) { return; }

	clamp(0, src_rect.a.x, src.width);
	clamp(0, src_rect.b.x, src.width);
	clamp(0, src_rect.a.y, src.height);
	clamp(0, src_rect.b.y, src.height);

	if (dst_rect.a.x > dst_rect.b.x) {
		swap(dst_rect.a.x, dst_rect.b.x);
		swap(src_rect.a.x, src_rect.b.x);
	}
	if (dst_rect.a.y > dst_rect.b.y) {
		swap(dst_rect.a.y, dst_rect.b.y);
		swap(src_rect.a.y, src_rect.b.y);
	}

	if (dst_rect.b.x < write_rect.a.x || dst_rect.a.x >= write_rect.b.x) { return; }
	if (dst_rect.b.y < write_rect.a.x || dst_rect.a.y >= write_rect.b.y) { return; }

	const int32_t dsx = ((src_rect.b.x - src_rect.a.x) << 15) / (dst_rect.b.x - dst_rect.a.x);
	const int32_t dsy = ((src_rect.b.y - src_rect.a.y) << 15) / (dst_rect.b.y - dst_rect.a.y);

	int32_t ssx = dsx >= 0 ? (min(src_rect.a.x, src_rect.b.x) << 15) : ((max(src_rect.a.x, src_rect.b.x) << 15) + dsx);
	if (dst_rect.a.x < write_rect.a.x) {
		if (dsx >= 0) { ssx += dsx * (dst_rect.a.x - write_rect.a.x); }
		dst_rect.a.x = write_rect.a.x;
	}
	if (dst_rect.b.x >= write_rect.b.x) {
		if (dsx < 0) { ssx += dsx * (write_rect.b.x - dst_rect.b.x); }
		dst_rect.b.x = write_rect.b.x;
	}

	int32_t ssy = dsy >= 0 ? (min(src_rect.a.y, src_rect.b.y) << 15) : ((max(src_rect.a.y, src_rect.b.y) << 15) + dsy);
	if (dst_rect.a.y < write_rect.a.y) {
		if (dsy >= 0) { ssy += dsy * (dst_rect.a.y - write_rect.a.y); }
		dst_rect.a.y = write_rect.a.y;
	}
	if (dst_rect.b.y >= write_rect.b.y) {
		if (dsy < 0) { ssy += dsy * (write_rect.b.y - dst_rect.b.y); }
		dst_rect.b.y = write_rect.b.y;
	}

	for (int32_t dy = dst_rect.a.y, sy = 0; dy < dst_rect.b.y; ++dy, sy += dsy) {
		for (int32_t dx = dst_rect.a.x, sx = 0; dx < dst_rect.b.x; ++dx, sx += dsx) {
			set_color(
				dst,
				Point{ dx, dy },
				shader(
					get_color(dst, Point{ dx, dy }),
					sampler(src, ssx + sx, ssy + sy)
				)
			);
		}
	}
}

int32_t cc0::gfx::print_text(cc0::gfx::Image &dst, cc0::gfx::Point p, const char *text, int32_t text_len, cc0::gfx::RGBA32 color, int32_t scale)
{
	static const Image src = new_image(
		FONT_ATLAS,
		FONT_ATLAS_CHAR_WIDTH_COUNT * FONT_CELL_PX_WIDTH, FONT_ATLAS_CHAR_HEIGHT_COUNT * FONT_CELL_PX_HEIGHT, 0,
		encode_1bpp, decode_1bpp, index_linear
	);
	const int32_t CHAR_DST_WIDTH  = FONT_CHAR_PX_WIDTH * scale;
	const int32_t CHAR_DST_HEIGHT = FONT_CHAR_PX_HEIGHT * scale;
	for (int32_t i = 0; i < text_len && text[i] != 0; ++i, p.x += CHAR_DST_WIDTH) {
		if (text[i] != ' ') {
			const int32_t sy = FONT_CELL_PX_HEIGHT * ((int32_t)text[i] - FONT_CHAR_ASCII_START);
			stretch_image(
				dst, Rect{ Point{ p.x, p.y }, Point{ p.x + CHAR_DST_WIDTH, p.y + CHAR_DST_HEIGHT } },
				src, shade_stencil, sample_nearest, Rect{ Point{ 0, sy }, Point{ FONT_CELL_PX_WIDTH, sy + FONT_CELL_PX_HEIGHT } }
			);
		}
	}
	return p.x;
}
