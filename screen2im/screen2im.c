/*
Copyright (c) 2012 Sekou Remy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#if !defined(IS_MACOSX) && defined(__APPLE__) && defined(__MACH__)
	#define IS_MACOSX
#endif /* IS_MACOSX */

#if !defined(IS_WINDOWS) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	#define IS_WINDOWS
#endif /* IS_WINDOWS */

#if !defined(USE_X11) && !defined(NUSE_X11) && !defined(IS_MACOSX) && !defined(IS_WINDOWS)
	#define USE_X11
#endif /* USE_X11 */

#if defined(IS_MACOSX)
	#include <ApplicationServices/ApplicationServices.h>
#elif defined(USE_X11)
	// X Server includes
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
#elif defined(IS_WINDOWS)
	#define STRICT /* Require use of exact types. */
	#define WIN32_LEAN_AND_MEAN 1 /* Speed up compilation. */
	#include <windows.h>
#endif

#if !defined(IS_MACOSX) && !defined(USE_X11)
	#error "Sorry, this platform isn't supported yet!"
#endif

#if defined(IS_MACOSX)
	CGDirectDisplayID OpenDisplay();
	// OSX/OpenGL resources
	CGDirectDisplayID _display;
	CGImageRef _OSImage;

#elif defined(USE_X11)
	Display* OpenDisplay();
	// X resources
	Display* _display;
	XImage* _OSImage;

#elif defined(IS_WINDOWS)
	//place holder
#endif

void InitImage();
int ImageSetup();

void DestroyImage();
void CloseDisplay();
int CopyScreen(unsigned char * data);

int _startX;
int _startY;
int _widthX;
int _heightY;

void InitImage(int startX,int startY, int widthX, int heightY)
{
	_display = NULL;
	_OSImage = NULL;
	
	_startX = startX;
	_startY = startY;
	_widthX = widthX;
	_heightY = heightY;
}

int ImageSetup()
{
	DestroyImage();
	CloseDisplay();
	_display = OpenDisplay(); 
	
	if (!(_display))
	{
		return -1;
	}
	return 0;
}


#if defined(IS_MACOSX)
	void DestroyImage(){
		if (_OSImage) CGImageRelease(_OSImage);
		_OSImage = NULL;
	}

	void CloseDisplay(){
		if (_display){
			//CGLSetCurrentContext( NULL );
			//CGLClearDrawable( contextObj );
			//CGLDestroyContext( contextObj );
			CGReleaseAllDisplays();
		}
		//_display = NULL;
	}

	CGDirectDisplayID OpenDisplay(){
		return CGMainDisplayID();
	}

	int CopyScreen(unsigned char * screenshot){
		int retval = -1;
		_OSImage = CGDisplayCreateImageForRect(_display, CGRectMake(_startX, _startY, _widthX, _heightY));
		
		if (_OSImage)
		{
			
			int height=(int)CGImageGetHeight(_OSImage);
			int width=(int)CGImageGetWidth(_OSImage);
			int bpp=(int)CGImageGetBitsPerPixel(_OSImage);
			
			CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
			
			int bytesPerPixel = bpp/8;
			int bytesPerRow = bytesPerPixel * width;
			int bitsPerComponent = 8;
			
			CGContextRef context = CGBitmapContextCreate(screenshot, width, height, bitsPerComponent, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
			
			if (!context)
			{
				//ERROR("Context could not be created!");
				retval = -1;//break
			}
			else {
				CGContextDrawImage(context, CGRectMake(0, 0, width, height),_OSImage);
				CGContextRelease(context);
				retval = 0;			
			}
		}
		
		DestroyImage();
		return retval;
	}

#elif defined(USE_X11)

	void DestroyImage(){
		if (_OSImage) XDestroyImage(_OSImage);
		_OSImage = NULL;
	}

	void CloseDisplay(){
		if (_display) XCloseDisplay(_display);
		_display = NULL;
	}

	Display* OpenDisplay(){
		return XOpenDisplay(NULL);
	}

	int CopyScreen(unsigned char * screenshot){
		int retval = -1;
		_OSImage = XGetImage(_display, DefaultRootWindow(_display), _startX, _startY, _widthX, _heightY, AllPlanes, ZPixmap);
		
		// If there is an image, and the depth of th image is 24
		if (_OSImage && _OSImage->depth == 24)
		{
			// Code inspired by http://opencv.willowgarage.com/wiki/ximage2opencvimage
			// and http://www.roard.com/docs/cookbook/cbsu19.html ("Screen grab with X11" - by Marko Riedel, with an idea by Alexander Malmberg)
			unsigned long rshift, rbits, gshift, gbits, bshift, bbits, rmask, gmask, bmask;
			unsigned char colorChannel[3];
			rmask = _OSImage->red_mask;
			gmask = _OSImage->green_mask;
			bmask = _OSImage->blue_mask;
			
			rshift = 0;
			rbits = 0;
			while (!(rmask & 1)) {
				rshift++;
				rmask >>= 1;
			}
			while (rmask & 1) {
				rbits++;
				rmask >>= 1;
			}
			if (rbits > 8) {
				rshift += rbits - 8;
				rbits = 8;
			}
			
			gshift = 0;
			gbits = 0;
			while (!(gmask & 1)) {
				gshift++;
				gmask >>= 1;
			}
			while (gmask & 1) {
				gbits++;
				gmask >>= 1;
			}
			if (gbits > 8) {
				gshift += gbits - 8;
				gbits = 8;
			}
			
			bshift = 0;
			bbits = 0;
			while (!(bmask & 1)) {
				bshift++;
				bmask >>= 1;
			}
			while (bmask & 1) {
				bbits++;
				bmask >>= 1;
			}
			if (bbits > 8) {
				bshift += bbits - 8;
				bbits = 8;
			}
			
			XColor color;
			for ( int x = 0; x < _OSImage->width; x++) {
				for ( int y = 0; y < _OSImage->height; y++) {
					color.pixel = XGetPixel(_OSImage, x, y);
					colorChannel[0] = ((color.pixel >> bshift) & ((1 << bbits) - 1)) << (8 - bbits);
					colorChannel[1] = ((color.pixel >> gshift) & ((1 << gbits) - 1)) << (8 - gbits);
					colorChannel[2] = ((color.pixel >> rshift) & ((1 << rbits) - 1)) << (8 - rbits);
					
					int index = 3*x+3*y*_OSImage->width;  //orientation switch!
					screenshot[index] =  colorChannel[2];
					screenshot[index + 1] = colorChannel[1];
					screenshot[index + 2] = colorChannel[0];
					screenshot[index + 3] = 255;
				}
			}
			retval = 0;
		}
		
		DestroyImage()
		return retval;
	}

#elif defined(IS_WINDOWS)
	//place holder
#endif