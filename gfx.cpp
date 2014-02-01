//
// gfx.cpp
// Provides a simple interface for graphics using SDL.
//
// Copyright (c) Jonathan Karlsson 2010
// Code may be used freely for commercial and non-commercial purposes.
// Author retains his moral rights under the applicable copyright laws
// (i.e. credit the author where credit is due).
//

#include <sstream>
//#include <fstream>
//#include <math.h>

#include "gfx.h"

//
// Color arithmetic tables
// Used for component based saturation arithmetic.
//
static Uint8 tableAdd[UCHAR_MAX+1][UCHAR_MAX+1];
static Uint8 tableSub[UCHAR_MAX+1][UCHAR_MAX+1];
static Uint8 tableMul[UCHAR_MAX+1][UCHAR_MAX+1];
float u8chan_to_fchan[UCHAR_MAX+1];

//
// GfxInit
// Initializes the Gfx component, such as
// color arithmetic tables.
//
bool GfxInit(Uint32 pScreenW, Uint32 pScreenH, bool pFullscreen, Uint32 SDL_INIT_FLAGS)
{
	if (SDL_Init(SDL_INIT_FLAGS) == -1) {
		return false;
	}
	
	if (sizeof(Sint32) == sizeof(Color32) && sizeof(Color32) == 4) {
		// initializes addition table
		for (Sint32 i = 0; i <= UCHAR_MAX; ++i){
			Sint32 j = 0;
			for (Sint32 n = i; n <= UCHAR_MAX; ++j, ++n){
				tableAdd[i][j] = (Uint8)n;
			}
			for (; j <= UCHAR_MAX; ++j){
				tableAdd[i][j] = UCHAR_MAX;
			}
		}

		// initializes subtration table
		for (Sint32 i = 0; i <= UCHAR_MAX; ++i){
			Sint32 j = 0;
			for (Sint32 n = i; n >= 0; ++j, --n){
				tableSub[i][j] = (Uint8)n;
			}
			for (; j <= UCHAR_MAX; ++j){
				tableSub[i][j] = 0;
			}
		}
		
		// initializes multiplication table
		const float fBYTE_MAX = (float)UCHAR_MAX;
		for (float i = 0; i <= fBYTE_MAX; ++i){
			for (float j = 0; j <= fBYTE_MAX; ++j){
				tableMul[(Sint32)i][(Sint32)j] = (Uint8)(i *(j/fBYTE_MAX));
			}
		}
		
		// initializes Uint8 to float table
		for (Sint32 i = 0; i < UCHAR_MAX+1; ++i){
			u8chan_to_fchan[i] = fBYTE_MAX/(float)i;
		}
	} else {
		SDL_SetError("Gfx cannot be used (platform error)");
		return false;
	}
	return GfxSetVideo(pScreenW, pScreenH, pFullscreen);
}

//
// GfxQuit
// Destroys the screen surface and calls SDL_Quit in the
// correct order to prevent crash.
//
void GfxQuit( void )
{
	SDL_FreeSurface(SDL_GetVideoSurface());
	SDL_Quit();
}

//
// Color32
//

//
// Color32 (ctor)
//
Color32::Color32( void ) : value(0) {}
Color32::Color32(const Color32 &pColor) : value(pColor.value) {}
Color32::Color32(const Color32 &pColor, Uint8 pAlpha) : value(pColor.value) { channels.alpha = pAlpha; }
Color32::Color32(Uint32 pColor) : value(pColor) {}
Color32::Color32(Uint8 pR, Uint8 pG, Uint8 pB, Uint8 pA)
{
	channels.red = pR;
	channels.green = pG;
	channels.blue = pB;
	channels.alpha = pA;
}

//
// Color arithmetic operators
//
Color32 &operator +=(Color32 &pLeft, const Color32 &pRight)
{
	pLeft.channels.red = tableAdd[pLeft.channels.red][pRight.channels.red];
	pLeft.channels.green = tableAdd[pLeft.channels.green][pRight.channels.green];
	pLeft.channels.blue = tableAdd[pLeft.channels.blue][pRight.channels.blue];
	pLeft.channels.alpha = tableAdd[pLeft.channels.alpha][pRight.channels.alpha];
	return pLeft;
}
Color32 &operator -=(Color32 &pLeft, const Color32 &pRight)
{
	pLeft.channels.red = tableSub[pLeft.channels.red][pRight.channels.red];
	pLeft.channels.green = tableSub[pLeft.channels.green][pRight.channels.green];
	pLeft.channels.blue = tableSub[pLeft.channels.blue][pRight.channels.blue];
	pLeft.channels.alpha = tableSub[pLeft.channels.alpha][pRight.channels.alpha];
	return pLeft;
}
Color32 &operator *=(Color32 &pLeft, const Color32 &pRight)
{
	pLeft.channels.red = tableMul[pLeft.channels.red][pRight.channels.red];
	pLeft.channels.green = tableMul[pLeft.channels.green][pRight.channels.green];
	pLeft.channels.blue = tableMul[pLeft.channels.blue][pRight.channels.blue];
	pLeft.channels.alpha = tableMul[pLeft.channels.alpha][pRight.channels.alpha];
	return pLeft;
}
Color32 &operator >>=(Color32 &pLeft, Sint32 pRight)
{
	pLeft.channels.red >>= pRight;
	pLeft.channels.green >>= pRight;
	pLeft.channels.blue >>= pRight;
	pLeft.channels.alpha >>= pRight;
	return pLeft;
}
Color32 &operator <<=(Color32 &pLeft, Sint32 pRight)
{
	pLeft.channels.red <<= pRight;
	pLeft.channels.green <<= pRight;
	pLeft.channels.blue <<= pRight;
	pLeft.channels.alpha <<= pRight;
	return pLeft;
}
bool operator ==(Color32 pLeft, Color32 pRight) // does not test alpha
{
	return (
		pLeft.channels.red == pRight.channels.red &&
		pLeft.channels.green == pRight.channels.green &&
		pLeft.channels.blue == pRight.channels.blue
		);
}
bool operator !=(const Color32 &pLeft, const Color32 &pRight)
{
	return !(pLeft == pRight);
}
Color32 operator+(Color32 pLeft, const Color32 &pRight) { return (pLeft += pRight); }
Color32 operator-(Color32 pLeft, const Color32 &pRight) { return (pLeft -= pRight); }
Color32 operator*(Color32 pLeft, const Color32 &pRight) { return (pLeft *= pRight); }
Color32 operator>>(Color32 pLeft, Sint32 pRight) { return (pLeft >>= pRight); }
Color32 operator<<(Color32 pLeft, Sint32 pRight) { return (pLeft <<= pRight); }

//
// Nearest
//

//
// operator()
// Samples the nearest color. Looks pixelated
// when stretched out, and looks aliased when
// compressed to a smaller size than normal.
//
Color32 Nearest::operator()(const Image &pImage, float pU, float pV) const {
	return pImage[(Sint32)((pImage.GetHeight()-1)*pV)][(Sint32)((pImage.GetWidth()-1)*pU)];
}

//
// Bilinear
//

//
// operator()
// Samples the four closest colors and
// interpolates between the colors to get the
// most accurate color.
//
Color32 Bilinear::operator()(const Image &pImage, float pU, float pV) const {	
	float fU = pU * (pImage.GetWidth() - 2);
	float fV = pV * (pImage.GetHeight() - 2);
	Sint32 iU = (Sint32)fU;
	Sint32 iV = (Sint32)fV;
	
	float u_ratio = fU - iU;
	float v_ratio = fV - iV;
	float u_opposite = 1.f - u_ratio;
	float v_opposite = 1.f - v_ratio;
	
	Color32 c00 = pImage[iV][iU];
	Color32 c01 = pImage[iV+1][iU];
	Color32 c10 = pImage[iV][iU+1];
	Color32 c11 = pImage[iV+1][iU+1];
	
	Color32 final(
		(Uint8)((c00.channels.red * u_opposite + c10.channels.red * u_ratio) * v_opposite + (c01.channels.red * u_opposite + c11.channels.red * u_ratio) * v_ratio),
		(Uint8)((c00.channels.green * u_opposite + c10.channels.green * u_ratio) * v_opposite + (c01.channels.green * u_opposite + c11.channels.green * u_ratio) * v_ratio),
		(Uint8)((c00.channels.blue * u_opposite + c10.channels.blue * u_ratio) * v_opposite + (c01.channels.blue * u_opposite + c11.channels.blue * u_ratio) * v_ratio),
		(Uint8)((c00.channels.alpha * u_opposite + c10.channels.alpha * u_ratio) * v_opposite + (c01.channels.alpha * u_opposite + c11.channels.alpha * u_ratio) * v_ratio)
	);
	
	return final;
}

//
// Image
//

//
// Free
// Frees the data allocated for the image.
//
void Image::Free( void )
{
	delete [] pixels;
	pixels = (Color32*)0;
	width = 0;
	height = 0;
}

//
// Create
// Allocated data for the image.
//
bool Image::Create(Sint32 pWidth, Sint32 pHeight)
{
	Free();
	if (pWidth > 0 && pWidth <= Image::MaxDimension && pHeight > 0 && pHeight <= Image::MaxDimension) {
		try {
			pixels = new Color32 [pWidth*pHeight];
			width = pWidth;
			height = pHeight;
		} catch (std::exception &pEx) {
			std::ostringstream sout;
			sout << "0x" << this << ": " << pEx.what();
			SDL_SetError(sout.str().c_str());
			Free();
		}
	} else {
		std::ostringstream sout;
		sout << "0x" << this << ": Invalid size";
		SDL_SetError(sout.str().c_str());
	}
	return IsGood();
}

//
// SetMemory
// Force a specific memory in image.
// Note: Does not free previous memory and can result
// in memory leakage when used improperly.
//
void Image::SetMemory(Color32 *pPix, Sint32 pWidth, Sint32 pHeight)
{
	pixels = pPix;
	width = pWidth;
	height = pHeight;
}

//
// Copy
// Copies the data from an existing image.
//
bool Image::Copy(const Image &pImage)
{
	if (Create(pImage.width, pImage.height)) {
		Color32 *dst = (*this)[0];
		const Color32 *src = pImage[0];
		for (Sint32 i=0; i < width*height; ++i){
			dst[i] = src[i];
		}
		return true;
	}
	return false;
}

//
// Load
// Allocates data for image and loads a native format
// from a file.
//
bool Image::Load(const std::string &pFile)
{
	std::ifstream fin(pFile.c_str(), std::ios::binary);
	if (!fin.is_open()) {
		std::ostringstream sout;
		sout << "0x" << this << ": Could not open file";
		SDL_SetError(sout.str().c_str());
		return false;
	}

	try {
		Sint32 typeSize=0;
		fin.read((char*)(&typeSize), sizeof(typeSize));
		if (typeSize == sizeof(Color32)) {
			fin.read((char*)(&width), sizeof(width));
			fin.read((char*)(&height), sizeof(height));
			if (Create(width, height)) {
				fin.read((char*)pixels, width*height*sizeof(Color32));
			}
		} else {
			std::ostringstream sout;
			sout << "0x" << this << ": Format not recognized";
			SDL_SetError(sout.str().c_str());
			Free();
		}
	} catch (std::exception &pEx) {
		std::ostringstream sout;
		sout << "0x" << this << ": " << pEx.what();
		SDL_SetError(sout.str().c_str());
		Free();
	}

	fin.close();
	return IsGood();
}

//
// Save
// Saves the current image to a file in a native format.
//
bool Image::Save(const std::string &pFile) const
{
	if (width*height == 0 || pixels == NULL) { return false; }
	std::ofstream fout(pFile.c_str(), std::ios::binary);
	if (!fout.is_open()) {
		std::ostringstream sout;
		sout << "0x" << this << ": Could not open file";
		SDL_SetError(sout.str().c_str());
		return false;
	}

	try {
		Sint32 typeSize = sizeof(Color32);
		fout.write((char*)(&typeSize), sizeof(typeSize));
		fout.write((char*)(&width), sizeof(width));
		fout.write((char*)(&height), sizeof(height));
		fout.write((char*)pixels, width*height*sizeof(Color32));
	} catch (std::exception &pEx) {
		std::ostringstream sout;
		sout << "0x" << this << ": " << pEx.what();
		SDL_SetError(sout.str().c_str());
		fout.close();
		return false;
	}

	fout.close();
	return true;
}

//
// Convert
// Converts a non-native format to a native format.
// Non-native formats are all formats loadable by
// SDL and SDL_Image.
// NOTE: If you are using SDL_image, include SDL_image.h
// BEFORE gfx.h in your main.cpp/main.c.
//
bool Image::Convert(const std::string &pFile)
{
	Free();
	
	SDL_Surface *src = APIIMGLOAD(pFile.c_str());
	if (src == (SDL_Surface*)0) { return false; } // No need to set SDL error manually

	if (Create(src->w, src->h)) {
		const Sint32 SIZE = width*height;
		Color32 *dst = pixels;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				Uint8 *spix = (Uint8 *)src->pixels + y * src->pitch + x * src->format->BytesPerPixel;
				SDL_GetRGBA(
					*((Uint32*)spix),
					src->format,
					&dst->channels.red,
					&dst->channels.green,
					&dst->channels.blue,
					&dst->channels.alpha
				);
				++dst;
			}
		}
	}

	SDL_FreeSurface(src);

	return IsGood();
}

//
// ReverseByteorder
// Reverses the byteorder in case Convert does not
// accurately read the requested file.
//
void Image::ReverseByteorder( void )
{
	for (Sint32 i = 0; i < width*height; ++i){
		Uint8 *bytes = (Uint8*)&pixels[i];
		Uint8 temp = bytes[0];
		bytes[0] = bytes[3];
		bytes[3] = temp;
		temp = bytes[1];
		bytes[1] = bytes[2];
		bytes[2] = temp;
	}
}

//
// GetRGB
// Returns floating point channels of specified
// pixel coordinate
//
void Image::GetRGB(Sint32 pX, Sint32 pY, float &pR, float &pG, float &pB) const
{
	pR = u8chan_to_fchan[(*this)[pY][pX].channels.red];
	pG = u8chan_to_fchan[(*this)[pY][pX].channels.green];
	pB = u8chan_to_fchan[(*this)[pY][pX].channels.blue];
}

//
// GetRGBA
// Returns floating point channels of specified
// pixel coordinate
//
void Image::GetRGBA(Sint32 pX, Sint32 pY, float &pR, float &pG, float &pB, float &pA) const
{
	pR = u8chan_to_fchan[(*this)[pY][pX].channels.red];
	pG = u8chan_to_fchan[(*this)[pY][pX].channels.green];
	pB = u8chan_to_fchan[(*this)[pY][pX].channels.blue];
	pA = u8chan_to_fchan[(*this)[pY][pX].channels.alpha];
}

//
// SetRGB
// Sets the specified pixel in accordance with
// the given color channels
//
void Image::SetRGB(Sint32 pX, Sint32 pY, float pR, float pG, float pB)
{
	(*this)[pY][pX].channels.red = (Uint8)(pR*255.f);
	(*this)[pY][pX].channels.green = (Uint8)(pG*255.f);
	(*this)[pY][pX].channels.blue = (Uint8)(pB*255.f);
}

//
// SetRGBA
// Sets the specified pixel in accordance with
// the given color channels
//
void Image::SetRGBA(Sint32 pX, Sint32 pY, float pR, float pG, float pB, float pA)
{
	(*this)[pY][pX].channels.red = (Uint8)(pR*255.f);
	(*this)[pY][pX].channels.green = (Uint8)(pG*255.f);
	(*this)[pY][pX].channels.blue = (Uint8)(pB*255.f);
	(*this)[pY][pX].channels.alpha = (Uint8)(pA*255.f);
}

//
// operator=
//
Image &Image::operator =(const Image &pImage)
{
	if (this != &pImage) {
		Copy(pImage);
	}
	return *this;
}

//
// Stream
//

//
// Free
// Frees file from file association.
//
void Image::Stream::Free( void )
{
	width = 0;
	height = 0;
	file.clear();
	dataStart = 0;
}

//
// Load
// Associate to a file.
//
bool Image::Stream::Load(const std::string &pFile)
{
	std::ifstream fin(pFile.c_str(), std::ios::binary);
	if (!fin.is_open()){
		Free();
		return false;
	}

	fin.read((char*)(&width), sizeof(width));
	fin.read((char*)(&height), sizeof(height));
	file = pFile;
	dataStart = sizeof(width)+sizeof(height);

	fin.close();
	return true;
}

//
// IsGood
// Returns true if current file is a valid file.
//
bool Image::Stream::IsGood( void ) const
{
	std::ifstream fin(file.c_str(), std::ios::binary);
	bool good = fin.is_open();
	fin.close();
	return good;
}

//
// Refresh
// If changes have been made to a file after it has been
// loaded it could need refreshing.
//
bool Image::Stream::Refresh( void )
{
	return Load(file);
}

//
// Screen
//

//
// GfxSetVideo
// Creates a screen surface for drawing. No direct screen
// manipulation allowed (might solve instancing problems).
//
bool GfxSetVideo(Uint32 pScreenW, Uint32 pScreenH, bool pFullscreen)
{
	SDL_FreeSurface(SDL_GetVideoSurface());
	if (pScreenW != 0 && pScreenW < 640 && pScreenH != 0 && pScreenH < 480 && pFullscreen){
		std::ostringstream sout;
		sout << "GfxScreen: Cannot allocate screen size of less than 640x480 in fullscreen mode";
		SDL_SetError(sout.str().c_str());
		return false;
	}
	if (((pScreenW == 0 && pScreenH == 0) || (pScreenW > 0 && pScreenH > 0)) &&
		SDL_SetVideoMode(pScreenW, pScreenH, 32, SDL_SWSURFACE|SDL_DOUBLEBUF|(SDL_FULLSCREEN *Sint32(pFullscreen))) != (SDL_Surface*)0)
	{
		if (SDL_GetVideoSurface()->format->BytesPerPixel != sizeof(Color32)){
			std::ostringstream sout;
			sout << "GfxScreen: Cannot allocate pixel of byte size " << sizeof(Color32) << " (SDL limitation)";
			SDL_SetError(sout.str().c_str());
			SDL_FreeSurface(SDL_GetVideoSurface());
			return false;
		}
	}
	return true;
}

//
// GfxFlip
// Transfers the data from one image to
// the screen.
//
bool GfxFlip(const Image &pSrc)
{
	class Screen : public Image
	{
	public:
		Screen( void ) : Image() {
			pixels = (Color32*)SDL_GetVideoSurface()->pixels;
			width = SDL_GetVideoSurface()->w;
			height = SDL_GetVideoSurface()->h;
		}
		~Screen( void ) {
			pixels = (Color32*)0;
		}
	public:
		void Lock( void ) {
			if (SDL_MUSTLOCK(SDL_GetVideoSurface()) && SDL_GetVideoSurface()->locked == 0) {
				SDL_LockSurface(SDL_GetVideoSurface());
			}
		}
		void Unlock( void ) {
			if (SDL_MUSTLOCK(SDL_GetVideoSurface())) {
				while (SDL_GetVideoSurface()->locked > 0) {
					SDL_UnlockSurface(SDL_GetVideoSurface());
				}
			}
		}
		void Release( void ) {
			pixels = (Color32*)0;
		}
	};

	Screen screen;
	screen.Lock();

	if (pSrc.GetWidth() == screen.GetWidth() && pSrc.GetHeight() == screen.GetHeight()){
		const Color32 *src = pSrc[0];
		Color32 *dst = screen[0];
		const Sint32 size = pSrc.GetWidth()*pSrc.GetHeight();
		for (Sint32 i = 0; i < size; ++i){
			dst[i] = src[i];
		}
	} else {
		unsigned int xscale = pSrc.GetWidth()/screen.GetWidth();
		float yscale = (float)pSrc.GetHeight()/(float)screen.GetHeight();
		if (xscale == yscale && !(xscale & (xscale - 1))) {
			unsigned int si = (unsigned int)(log(yscale)/log(2.f)); // 2^si = xscale, si*ln(2)=ln(32)
			if (si < 23) { // if more shifts are needed than 22, then loss of information
				Sint32 *aaR = new Sint32[screen.GetWidth()];
				Sint32 *aaG = new Sint32[screen.GetWidth()];
				Sint32 *aaB = new Sint32[screen.GetWidth()];
				for (Sint32 x = 0; x < screen.GetWidth(); ++x) {
					aaR[x] = 0;
					aaG[x] = 0;
					aaB[x] = 0;
				}
				
				Color32 *dst = screen[0];
				const Color32 *src = pSrc[0];
				for (Sint32 y = 0; y < screen.GetHeight(); ++y) {
					for (Sint32 y0 = 0; y0 < yscale; ++y0) {
						for (Sint32 x0 = 0; x0 < pSrc.GetWidth(); ++x0) {
							aaR[x0>>si] += src[x0].channels.red;
							aaG[x0>>si] += src[x0].channels.green;
							aaB[x0>>si] += src[x0].channels.blue;
						}
						src += pSrc.GetWidth();
					}
					for (Sint32 x = 0; x < screen.GetWidth(); ++x) {
						dst[x].channels.red = (Uint8)(aaR[x] >> (si+1));
						dst[x].channels.green = (Uint8)(aaG[x] >> (si+1));
						dst[x].channels.blue = (Uint8)(aaB[x] >> (si+1));
						aaR[x] = 0;
						aaG[x] = 0;
						aaB[x] = 0;
					}
					dst += screen.GetWidth();
				}
				delete [] aaR;
				delete [] aaG;
				delete [] aaB;
			} else {
				Image::Blit(screen, 0, 0, screen.GetWidth(), screen.GetHeight(), pSrc);
			}
		} else {
			Image::Blit(screen, 0, 0, screen.GetWidth(), screen.GetHeight(), pSrc);
		}
	}
	
	screen.Unlock();
	screen.Release();

	return (SDL_Flip(SDL_GetVideoSurface()) != -1);
}
