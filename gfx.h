//
// gfx.h
// Provides a simple interface for graphics using SDL.
//
// Copyright (c) Jonathan Karlsson 2010
// Code may be used freely for commercial and non-commercial purposes.
// Author retains his moral rights under the applicable copyright laws
// (i.e. credit the author where credit is due).
//

#ifndef GFX_H_INCLUDED__
#define GFX_H_INCLUDED__

#ifdef _MSC_VER
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif

#include <string>
#include <limits.h>
#include <fstream>
#include <math.h>
#ifdef __GNUC__
#include <SDL/SDL.h> // -lSDL -SDLmain (gcc/g++)
#else
#include "SDL.h" // SDL.lib SDLmain.lib (msvc)
#endif

//
// defines
// Determines what loader is the most approapriate.
// IMG_Load should be used if possible, but should not
// be a requirement for using this library. Not including
// SDL_image means that only .BMP files can be loaded.
//
#ifdef _SDL_IMAGE_H
#define APIIMGLOAD IMG_Load
#else
#define APIIMGLOAD SDL_LoadBMP
#endif

// really fast way to convert a [0-1] range float to 0-255 range uchar (note: makes permanent changes to input float)
// NOTE: Too compiler-dependent to rely on.
//#define fchan_to_u8chan(flt, uch) flt=flt*255.0f+256.0f; uch = ((*(int*)&flt)&0x7fffff)>>15;

//
// System functions
// 
bool GfxInit(Uint32 pScreenW=640, Uint32 pScreenH=480, bool pFullscreen=false, Uint32 SDL_INIT_FLAGS=SDL_INIT_VIDEO);
void GfxQuit( void );

//
// ARGB32/BGRA32
// 32-bit single channel color structures.
//
struct ARGB32 {
	Uint8 alpha,red,green,blue;
};
struct BGRA32 {
	Uint8 blue,green,red,alpha;
};

//
// Color32
// Providing saturation arithmetic for colors.
//
union Color32
{
public:
	Uint32 value;
	// NOT SURE ABOUT OSX!
	// Maybe Intel machines still use backwards color channels for compatibility with PPC apps... (?)
	// That would mean that I should not reverse color channels if big endinan on OSX.
#ifdef __MACOSX__
	#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		ARGB32 channels;
	#else
		BGRA32 channels;
	#endif
#elif defined __WIN32__
	#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		BGRA32 channels;
	#else
		ARGB32 channels;
	#endif
#elif defined __LINUX__
	#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		BGRA32 channels;
	#else
		ARGB32 channels;
	#endif
#endif
public:
	Color32( void );
	Color32(const Color32 &pColor);
	Color32(const Color32 &pColor, Uint8 pAlpha);
	explicit Color32(Uint32 pColor);
	Color32(Uint8 pR, Uint8 pG, Uint8 pB, Uint8 pA = UCHAR_MAX);
};

Color32 &operator+=(Color32 &pLeft, const Color32 &pRight);
Color32 &operator-=(Color32 &pLeft, const Color32 &pRight);
Color32 &operator*=(Color32 &pLeft, const Color32 &pRight);
Color32 &operator>>=(Color32 &pLeft, Sint32 pRight);
Color32 &operator<<=(Color32 &pLeft, Sint32 pRight);
bool operator==(Color32 pLeft, Color32 pRight);
bool operator!=(const Color32 &pLeft, const Color32 &pRight);
Color32 operator+(Color32 pLeft, const Color32 &pRight);
Color32 operator-(Color32 pLeft, const Color32 &pRight);
Color32 operator*(Color32 pLeft, const Color32 &pRight);
Color32 operator>>(Color32 pLeft, Sint32 pRight);
Color32 operator<<(Color32 pLeft, Sint32 pRight);

//
// BlitPred
// Abstract functor for inserting custom blit code where
// supported in Image class.
//
class Blender {
public:
	virtual Color32 operator()(Color32 pDst, Color32 pSrc) const = 0;
	virtual ~Blender( void ) {}
};

//
// Assign
// Simply assigns one color to another.
//
class Assign : public Blender {
public:
	Color32 operator()(Color32 pDst, Color32 pSrc) const {
		return pSrc;
	}
};

//
// AlphaBlend
// Blends one color with another based on contents in
// alpha channel.
//
class AlphaBlend : public Blender {
public:
	Color32 operator()(Color32 pDst, Color32 pSrc) const {
		pDst.channels.red += (pSrc.channels.alpha *(pSrc.channels.red -pDst.channels.red) >> CHAR_BIT);
		pDst.channels.green += (pSrc.channels.alpha *(pSrc.channels.green -pDst.channels.green) >> CHAR_BIT);
		pDst.channels.blue += (pSrc.channels.alpha *(pSrc.channels.blue -pDst.channels.blue) >> CHAR_BIT);
		pDst.channels.alpha += (pSrc.channels.alpha *(pSrc.channels.alpha -pDst.channels.alpha) >> CHAR_BIT);
		return pDst;
	}
};

//
// ColorKey
// Makes a certain color invisible (alpha not included).
//
class ColorKey : public Blender {
private:
	Color32 key;
public:
	ColorKey(Color32 pColorKey) : key(pColorKey) {}
	Color32 operator()(Color32 pDst, Color32 pSrc) const {
		return pSrc==key ? pDst : pSrc;
	}
};

//
// Grayscale
// Converts the source color to grayscale.
//
class Grayscale : public Blender {
public:
	Color32 operator()(Color32 pDst, Color32 pSrc) const {
		const Uint8 Gray = (Uint8)(
			(float)pSrc.channels.red*0.3f +
			(float)pSrc.channels.green*0.59f +
			(float)pSrc.channels.blue*0.11f
			);
		return Color32(Gray, Gray, Gray);
	}
};

//
// FillGrayscale
// Converts the destination color to grayscale.
//
class FillGrayscale : public Grayscale {
public:
	Color32 operator()(Color32 pDst, Color32 pSrc) const {
		return Grayscale::operator()(pSrc, pDst);
	}
};

//
// forward declaration
//
class Image;

//
// Sampler
// Base class for pixel sampling.
//
class Sampler {
public:
	virtual Color32 operator()(const Image &pImage, float pU, float pV) const = 0;
	virtual ~Sampler( void ) {}
};

//
// Nearest
// Samples the nearest color. Looks pixelated
// when stretched out, and looks aliased when
// compressed to a smaller size than normal.
//
class Nearest : public Sampler {
public:
	Color32 operator()(const Image &pImage, float pU, float pV) const;
};

//
// Bilinear
// Samples the four closest colors and
// interpolates between the colors to get the
// most accurate color. Produces artifacts when
// used in conjunction with alpha and color key
// blit predicates.
//
class Bilinear : public Sampler {
public:
	Color32 operator()(const Image &pImage, float pU, float pV) const;
};

//
// Image
// Class for handling images.
//
class Image
{
protected:
	Color32 *pixels;
	Sint32 width, height;
public:
	Image( void ) : pixels((Color32*)0), width(0), height(0)						{}
	Image(Sint32 pWidth, Sint32 pHeight) : pixels((Color32*)0), width(0), height(0)	{ this->Create(pWidth, pHeight); }
	Image(const Image &pImage) : pixels((Color32*)0), width(0), height(0)			{ this->Copy(pImage); }
	virtual ~Image( void )															{ delete [] pixels; } // don't call virtual functions in destructors
public:
	virtual void Free( void );
	virtual bool Create(Sint32 pWidth, Sint32 pHeight);
	virtual void SetMemory(Color32 *pPix, Sint32 pWidth, Sint32 pHeight);
	virtual bool Copy(const Image &pImage);
	virtual bool Load(const std::string &pFile);
	virtual bool Save(const std::string &pFile) const;
	virtual bool Convert(const std::string &pFile);
	virtual void ReverseByteorder( void );
	virtual void GetRGB(Sint32 pX, Sint32 pY, float &pR, float &pG, float &pB) const;
	virtual void GetRGBA(Sint32 pX, Sint32 pY, float &pR, float &pG, float &pB, float &pA) const;
	virtual void SetRGB(Sint32 pX, Sint32 pY, float pR, float pG, float pB);
	virtual void SetRGBA(Sint32 pX, Sint32 pY, float pR, float pG, float pB, float pA);
	template < typename Blender_t >
	void Fill(Sint32 pX1, Sint32 pY1, Sint32 pX2, Sint32 pY2, Color32 pColor, Blender_t &pBlend);
	template < typename Blender_t >
	void Line(Sint32 pX1, Sint32 pY1, Color32 pColor1, Sint32 pX2, Sint32 pY2, Color32 pColor2, Blender_t &pBlend);
	virtual Sint32 GetWidth( void ) const	{ return this->width; }
	virtual Sint32 GetHeight( void ) const	{ return this->height; }
	virtual bool IsGood( void ) const		{ return (this->pixels != (Color32*)0); }
	virtual bool IsBad( void ) const		{ return (this->pixels == (Color32*)0); }
public:
	virtual Image &operator=(const Image &pImage);
	virtual Color32 *operator[](Sint32 pY)				{ return this->pixels + (this->width * pY); }
	virtual const Color32 *operator[](Sint32 pY) const	{ return this->pixels + (this->width * pY); }
	virtual operator bool( void ) const					{ return this->IsGood(); }
public:
	static const Sint32 MaxDimension = USHRT_MAX;
public:
	//
	// Stream
	// Class used for streaming native images. There is no
	// support for streaming non-native images.
	//
	class Stream
	{
	private:
		Sint32 width, height;
		std::string file;
		Sint32 dataStart;
	public:
		void Free( void );
		bool Load(const std::string &pFile);
		bool IsGood( void ) const;
		bool Refresh( void );
	public:
		Sint32 GetWidth( void ) const				{ return this->width; }
		Sint32 GetHeight( void ) const				{ return this->height; }
		const std::string &GetFile( void ) const	{ return this->file; }
		Sint32 GetDataStart( void ) const			{ return this->dataStart; }
		bool IsBad( void ) const					{ return !this->IsGood(); }
	};
public:
	template < typename Blender_t, typename Sampler_t >
	static void Blit(Image &pDst, Sint32 pDx1, Sint32 pDy1, Sint32 pDx2, Sint32 pDy2, const Image &pSrc, Blender_t &pBlend, Sampler_t &pSample, Sint32 pSx1=0, Sint32 pSy1=0, Sint32 pSx2=Image::MaxDimension, Sint32 pSy2=Image::MaxDimension);
	template < typename Blender_t >
	static bool Blit(Image &pDst, Sint32 pDx1, Sint32 pDy1, Sint32 pDx2, Sint32 pDy2, const Image::Stream &pSrc, Blender_t &pBlend, Sint32 pSx1=0, Sint32 pSy1=0, Sint32 pSx2=Image::MaxDimension, Sint32 pSy2=Image::MaxDimension);
public:
	virtual void Fill(Sint32 pX1, Sint32 pY1, Sint32 pX2, Sint32 pY2, Color32 pColor)
	{
		Assign defBlend;
		this->Fill(pX1, pY1, pX2, pY2, pColor, defBlend);
	}
	virtual void Line(Sint32 pX1, Sint32 pY1, Color32 pColor1, Sint32 pX2, Sint32 pY2, Color32 pColor2)
	{
		Assign defBlend;
		this->Line(pX1, pY1, pColor1, pX2, pY2, pColor2, defBlend);
	}
	static void Blit(Image &pDst, Sint32 pDx1, Sint32 pDy1, Sint32 pDx2, Sint32 pDy2, const Image &pSrc, Sint32 pSx1=0, Sint32 pSy1=0, Sint32 pSx2=Image::MaxDimension, Sint32 pSy2=Image::MaxDimension)
	{
		Assign defBlend;
		Nearest defSamp;
		Image::Blit(pDst, pDx1, pDy1, pDx2, pDy2, pSrc, defBlend, defSamp, pSx1, pSy1, pSx2, pSy2);
	}
	template < typename Blender_t >
	static void Blit(Image &pDst, Sint32 pDx1, Sint32 pDy1, Sint32 pDx2, Sint32 pDy2, const Image &pSrc, Blender_t &pBlend, Sint32 pSx1=0, Sint32 pSy1=0, Sint32 pSx2=Image::MaxDimension, Sint32 pSy2=Image::MaxDimension)
	{
		Nearest defSamp;
		Image::Blit(pDst, pDx1, pDy1, pDx2, pDy2, pSrc, pBlend, defSamp, pSx1, pSy1, pSx2, pSy2);
	}
	static bool Blit(Image &pDst, Sint32 pDx1, Sint32 pDy1, Sint32 pDx2, Sint32 pDy2, const Image::Stream &pSrc, Sint32 pSx1=0, Sint32 pSy1=0, Sint32 pSx2=Image::MaxDimension, Sint32 pSy2=Image::MaxDimension)
	{
		Assign defBlend;
		return Image::Blit(pDst, pDx1, pDy1, pDx2, pDy2, pSrc, defBlend, pSx1, pSy1, pSx2, pSy2);
	}
};

//
// Fill
// Fills the specified area with the specified color using
// the specified predicate (normal assignment is default).
//
template < typename Blender_t >
void Image::Fill(Sint32 pX1, Sint32 pY1, Sint32 pX2, Sint32 pY2, Color32 pColor, Blender_t &pPred)
{
	pX1 = 0>pX1 ? 0 : pX1;
	pY1 = 0>pY1 ? 0 : pY1;
	pX2 = width<pX2 ? width : pX2;
	pY2 = height<pY2 ? height : pY2;
	
	Color32 *dst = (*this)[pY1];
	for (Sint32 y = pY1; y < pY2; ++y, dst += width){
		for (Sint32 x = pX1; x < pX2; ++x){
			dst[x] = pPred(dst[x], pColor);
		}
	}
}

//
// Line
// Draws a line between the two specified points using the
// two specified colors and the specified predicate (normal
// assignment is default).
//
template < typename Blender_t >
void Image::Line(Sint32 pX1, Sint32 pY1, Color32 pColor1, Sint32 pX2, Sint32 pY2, Color32 pColor2, Blender_t &pPred)
{
	float r1 = pColor1.channels.red;
	float g1 = pColor1.channels.green;
	float b1 = pColor1.channels.blue;
	float a1 = pColor1.channels.alpha;
	float r2 = pColor2.channels.red;
	float g2 = pColor2.channels.green;
	float b2 = pColor2.channels.blue;
	float a2 = pColor2.channels.alpha;
	
	float xdiff = (float)(pX2 - pX1);
	float ydiff = (float)(pY2 - pY1);
	
	if(xdiff == 0.f && ydiff == 0.f) {
		if (pX1 >= 0 && pX1 < width && pY1 >= 0 && pY1 < height){
			Color32 color((Uint8)r1, (Uint8)g1, (Uint8)b1, (Uint8)a1);
			(*this)[pY1][pX1] = pPred((*this)[pY1][pX1], color);
		}
		return;
	}
	
	if(fabs(xdiff) > fabs(ydiff)) {
		float xmin, xmax;
		
		// set xmin to the lower x value given
		// and xmax to the higher value
		if(pX1 < pX2) {
			xmin = (float)pX1;
			xmax = (float)pX2;
		} else {
			xmin = (float)pX2;
			xmax = (float)pX1;
		}
		xmin = xmin>0.f ? xmin : 0.f; // clipping
		xmax = xmax<(width-1) ? xmax : (width-1); // clipping
		
		// draw line in terms of y slope
		float slope = ydiff / xdiff;
		for(float x = xmin; x <= xmax; x += 1.f) {
			float y = pY1 + ((x - pX1) * slope);
			if (y < 0.f || y >= height) continue; // clipping
			Color32 color(
						  (Uint8)(r1 + ((r2 - r1) * ((x -pX1) / xdiff))),
						  (Uint8)(g1 + ((g2 - g1) * ((x -pX1) / xdiff))),
						  (Uint8)(b1 + ((b2 - b1) * ((x -pX1) / xdiff))),
						  (Uint8)(a1 + ((a2 - a1) * ((x -pX1) / xdiff)))
						  );
			(*this)[(Sint32)y][(Sint32)x] = pPred((*this)[(Sint32)y][(Sint32)x], color);
		}
	} else {
		float ymin, ymax;
		
		// set ymin to the lower y value given
		// and ymax to the higher value
		if(pY1 < pY2) {
			ymin = (float)pY1;
			ymax = (float)pY2;
		} else {
			ymin = (float)pY2;
			ymax = (float)pY1;
		}
		ymin = ymin>0.f ? ymin : 0.f; // clipping
		ymax = ymax<(height-1) ? ymax : (height-1); // clipping
		
		// draw line in terms of x slope
		float slope = xdiff / ydiff;
		for(float y = ymin; y <= ymax; y += 1.f) {
			float x = pX1 + ((y - pY1) * slope);
			if (x < 0.f || x >= width) continue; // clipping
			Color32 color(
						  (Uint8)(r1 + ((r2 - r1) * ((y -pY1) / ydiff))),
						  (Uint8)(g1 + ((g2 - g1) * ((y -pY1) / ydiff))),
						  (Uint8)(b1 + ((b2 - b1) * ((y -pY1) / ydiff))),
						  (Uint8)(a1 + ((a2 - a1) * ((y -pY1) / ydiff)))
						  );
			(*this)[(Sint32)y][(Sint32)x] = pPred((*this)[(Sint32)y][(Sint32)x], color);
		}
	}
}

//
// Blit
// Blits specified portion of an image (pSrc) to specified
// portion of another image (pDst) using a predicate
// (default normal assignment). If the source portion is
// larger or smaller than destination portion, then resizing
// will occur.
//
template < typename Blender_t, typename Sampler_t >
void Image::Blit(Image &pDst, Sint32 pDx1, Sint32 pDy1, Sint32 pDx2, Sint32 pDy2, const Image &pSrc, Blender_t &pBlend, Sampler_t &pSample, Sint32 pSx1, Sint32 pSy1, Sint32 pSx2, Sint32 pSy2)
{
	if (pSrc.IsBad() || pDst.IsBad()) {
		SDL_SetError("Blit: Bad source/destination");
		return;
	}
	
	// clip pSrcRect against max borders
	pSx1 = 0>pSx1 ? 0 : pSx1;
	pSy1 = 0>pSy1 ? 0 : pSy1;
	pSx2 = pSrc.GetWidth()<pSx2 ? pSrc.GetWidth() : pSx2;
	pSy2 = pSrc.GetHeight()<pSy2 ? pSrc.GetHeight() : pSy2;
	
	float u1 = (float)pSx1 / (float)(pSrc.GetWidth()-1);
	float v1 = (float)pSy1 / (float)(pSrc.GetHeight()-1);
	float u2 = (float)pSx2 / (float)(pSrc.GetWidth()-1);
	float v2 = (float)pSy2 / (float)(pSrc.GetHeight()-1);
	float du = (u2 - u1) / (float)(pDx2 - pDx1);
	float dv = (v2 - v1) / (float)(pDy2 - pDy1);
	
	// enable a negative writable area on pDst (flips blit direction)	
	if (pDx2 < pDx1) {
		Sint32 itemp = pDx1;
		pDx1 = pDx2;
		pDx2 = itemp;
		float ftemp = u1;
		u1 = u2;
		u2 = ftemp;
		if (pDx1 < 0) { // make read offset for pSrc + clip against min borders
			u1 = u1 - du * pDx1;
			pDx1 = 0;
		}
	} else if (pDx1 < 0) { // make read offset for pSrc + clip against min borders
		u1 = u1 + du * -pDx1;
		pDx1 = 0;
	}
	if (pDy2 < pDy1) {
		Sint32 itemp = pDy1;
		pDy1 = pDy2;
		pDy2 = itemp;
		float ftemp = v1;
		v1 = v2;
		v2 = ftemp;
		if (pDy1 < 0) { // make read offset for pSrc + clip against min borders
			v1 = v1 - dv * pDy1;
			pDy1 = 0;
		}
	} else if (pDy1 < 0) { // make read offset for pSrc + clip against min borders
		v1 = v1 + dv * -pDy1;
		pDy1 = 0;
	}
	
	// clip pDstRect agains max borders
	pDx2 = pDst.GetWidth()<pDx2 ? pDst.GetWidth() : pDx2;
	pDy2 = pDst.GetHeight()<pDy2 ? pDst.GetHeight() : pDy2;
	
	// determine writable area
	const Sint32 MAXY = pDy2 -pDy1;
	const Sint32 MAXX = pDx2 -pDx1;
	if (MAXX < 0 || MAXY < 0) { return; } // readable area is negative (probably because pSrc is offscreen)
	
	Color32 *dpix = pDst[pDy1];
	const Sint32 DST_WIDTH = pDst.GetWidth();
	
	// draw scanlines
	float v = v1;
	for (Sint32 y = 0; y < MAXY; ++y, dpix+=DST_WIDTH){
		float u = u1;
		for (Sint32 x = 0; x < MAXX; ++x){
			dpix[x+pDx1] = pBlend(dpix[x+pDx1], pSample(pSrc, u, v));
			//dpix[x+pDx1] = pSrc[(Sint32)((pSrc.GetHeight()-1)*v)][(Sint32)((pSrc.GetWidth()-1)*u)];
			u+=du;
		}
		v+=dv;
	}
}

//
// Blit
// Blits (streams) specified portion of a file (pSrc) to
// specified portion of an image (pDst) using a predicate
// (default normal assignment). If the source portion is
// larger or smaller than destination portion, then
// resizing will occur.
//
template < typename Blender_t >
bool Image::Blit(Image &pDst, Sint32 pDx1, Sint32 pDy1, Sint32 pDx2, Sint32 pDy2, const Image::Stream &pSrc, Blender_t &pBlend, Sint32 pSx1, Sint32 pSy1, Sint32 pSx2, Sint32 pSy2)
{
	// clip pSrcRect against max borders
	pSx1 = 0>pSx1 ? 0 : pSx1;
	pSy1 = 0>pSy1 ? 0 : pSy1;
	pSx2 = pSrc.GetWidth()<pSx2 ? pSrc.GetWidth() : pSx2;
	pSy2 = pSrc.GetHeight()<pSy2 ? pSrc.GetHeight() : pSy2;
	
	const Sint32 SCALEX = (Sint32)((float)(pSx2 -pSx1) /(float)(pDx2 -pDx1) *(1<<16));
	const Sint32 SCALEY = (Sint32)((float)(pSy2 -pSy1) /(float)(pDy2 -pDy1) *(1<<16));
	
	// make read offset for pSrc
	Sint32 sx = 0;
	Sint32 sy = 0;
	if (pDx1 < 0) { sx = -pDx1; }
	if (pDy1 < 0) { sy = -pDy1; }
	
	// clip pDstRect agains max borders
	pDx1 = 0>pDx1 ? 0 : pDx1;
	pDy1 = 0>pDy1 ? 0 : pDy1;
	pDx2 = pDst.GetWidth()<pDx2 ? pDst.GetWidth() : pDx2;
	pDy2 = pDst.GetHeight()<pDy2 ? pDst.GetHeight() : pDy2;
	
	// determine writable area
	const Sint32 MAXY = pDy2 -pDy1;
	const Sint32 MAXX = pDx2 -pDx1;
	if (MAXX < 0 || MAXY < 0) {  return true; } // readable area is negative (probably because pSrc is offscreen)
	
	Color32 *dpix = pDst[pDy1];
	const Sint32 DST_WIDTH = pDst.GetWidth();
	const Sint32 SrcTypeSize = sizeof(Color32);
	const Sint32 SrcXByteRead = ((pSx2 -pSx1) *SrcTypeSize)>0 ? ((pSx2 -pSx1) *SrcTypeSize) : 0;
	const Sint32 SrcByteWidth = pSrc.GetWidth()*SrcTypeSize;
	const Sint32 SrcXOffset = (pSx1 *SrcTypeSize) + pSrc.GetDataStart();
	
	// allocate memory
	Color32 *spix = new Color32[pSx2 -pSx1]; // I trust this will not fail, even if that might be the case
	
	// open file stream (in)
	std::ifstream fin(pSrc.GetFile().c_str(), std::ios::binary);
	if (!fin.is_open()) { return false; } // file could not be opened
	
	// draw scanlines
	for (Sint32 y = 0; y < MAXY; ++y, dpix+=DST_WIDTH){
		fin.seekg( (std::streamoff)(((((SCALEY*(y+sy))>>16)+pSy1)*SrcByteWidth) + SrcXOffset) ); // NOTE: Might not be able to access bytes beyond the INT32_MAX limit.
		fin.read((char*)spix, SrcXByteRead);
		for (Sint32 x = 0; x < MAXX; ++x){
			dpix[x+pDx1] = pBlend(dpix[x+pDx1], spix[((SCALEX*(x+sx))>>16)]);
		}
	}
	delete [] spix;
	fin.close();
	return true;
}

//
// Screen functions
//
bool GfxSetVideo(Uint32 pScreenW, Uint32 pScreenH, bool pFullscreen);
bool GfxFlip(const Image &pSrc);
inline Sint32 GfxWidth( void ) { return SDL_GetVideoSurface()->w; }
inline Sint32 GfxHeight( void ) { return SDL_GetVideoSurface()->h; }
inline Color32 *GfxPixels( void ) { return (Color32*)SDL_GetVideoSurface()->pixels; }

#endif
