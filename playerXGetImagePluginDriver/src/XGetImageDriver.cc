/*
 * XGetImageDriver.cpp
 *
 *  Created on: Jun 10, 2011
 *      Author: sr
 */

#include <assert.h>
#include <time.h>
#include <pthread.h>

#include "XGetImageDriver.h"


Driver * XGetImageDriver_Init(ConfigFile * cf, int section)
{
	return reinterpret_cast<Driver *>(new XGetImageDriver(cf, section));
}

void XGetImageDriver_Register(DriverTable *table)
{
	table->AddDriver("xgetimagedriver", XGetImageDriver_Init);
}

XGetImageDriver::XGetImageDriver(ConfigFile * cf, int section)
: ThreadedDriver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN, PLAYER_CAMERA_CODE)
{
	this->display = NULL;
	this->xImageSample = NULL;
	
	this->startX = cf->ReadInt(section, "startX", 0);
	this->startY = cf->ReadInt(section, "startY", 0);
	this->widthX = cf->ReadInt(section, "widthX", 48);
	this->heightY = cf->ReadInt(section, "heightY", 64);
	this->sleep_nsec = cf->ReadInt(section, "sleep_nsec", 10000000);
}

XGetImageDriver::~XGetImageDriver()
{
	// Always clean up your mess
	XGetImageDriver::DestroyImage();
	XGetImageDriver::CloseDisplay();
}

int XGetImageDriver::MainSetup()
{
	XGetImageDriver::DestroyImage();
	XGetImageDriver::CloseDisplay();
	this->display = XGetImageDriver::OpenDisplay(); 
	
	if (!(this->display))
	{
		PLAYER_ERROR("Couldn't create capture display. Something is wrong with your libraries.");
		return -1;
	}
	
	return 0;
}

void XGetImageDriver::MainQuit()
{
	XGetImageDriver::DestroyImage();
	XGetImageDriver::CloseDisplay();
	
}

void XGetImageDriver::Main()
{
	struct timespec tspec;
	player_camera_data_t * data;
	
	
	for (;;)
	{
		// Go to sleep for a while (this is a polling loop).
		tspec.tv_sec = 0;
		tspec.tv_nsec = this->sleep_nsec;
		nanosleep(&tspec, NULL);
		
		// Test if we are supposed to cancel this thread.
		pthread_testcancel();
		
		ProcessMessages();
		pthread_testcancel();
		
		if (!this->display)
		{
			PLAYER_ERROR("No display!");
			break;
		}
		
		data = reinterpret_cast<player_camera_data_t *>(malloc(sizeof(player_camera_data_t)));
		if (!data)
		{
			PLAYER_ERROR("Out of memory");
			break;
		}
		
		int ret = XGetImageDriver::CopyScreen(data);
		
		if (ret == -1) break;
		else if (ret == 1) continue;
		
		assert(data->bpp > 0);
		data->compression = PLAYER_CAMERA_COMPRESS_RAW;
		this->Publish(device_addr, PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE, reinterpret_cast<void *>(data), 0, NULL, false);
		// copy = false, don't dispose anything here
		
		XGetImageDriver::DestroyImage();
		
		pthread_testcancel();
	}
}

int XGetImageDriver::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data)
{
	assert(hdr);
	return -1;
}

extern "C"
{
	int player_driver_init(DriverTable * table)
	{
		puts("XGetImageDriver driver initializing");
		XGetImageDriver_Register(table);
		
		return 0;
	}
	
}


#if defined(IS_MACOSX)
void XGetImageDriver::DestroyImage(){
	if (this->xImageSample) CGImageRelease(this->xImageSample);
	this->xImageSample = NULL;
}

void XGetImageDriver::CloseDisplay(){
	if (this->display){
		//CGLSetCurrentContext( NULL );
		//CGLClearDrawable( contextObj );
		//CGLDestroyContext( contextObj );
		CGReleaseAllDisplays();
	}
	//this->display = NULL;
}

CGDirectDisplayID XGetImageDriver::OpenDisplay(){
	return CGMainDisplayID();
}

int XGetImageDriver::CopyScreen(player_camera_data_t* screenshot){
	this->xImageSample = CGDisplayCreateImageForRect(this->display, CGRectMake(this->startX, this->startY, this->widthX, this->heightY));

	if (!this->xImageSample)
	{
		PLAYER_ERROR("No Image!");
		return -1;//break
	}
	
	int height=(int)CGImageGetHeight(this->xImageSample);
	int width=(int)CGImageGetWidth(this->xImageSample);
	int bpp=(int)CGImageGetBitsPerPixel(this->xImageSample);
	
	
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	unsigned char rawData[height * width * bpp]; //unsure why this isn't in bytes
	int bytesPerPixel = bpp/8;
	int bytesPerRow = bytesPerPixel * width;
	int bitsPerComponent = 8;
	
	CGContextRef context = CGBitmapContextCreate(rawData, width, height, bitsPerComponent, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);

	if (!context)
	{
		PLAYER_ERROR("Context could not be created!");
		return -1;//break
	}
	
	CGContextDrawImage(context, CGRectMake(0, 0, width, height),this->xImageSample);
	CGContextRelease(context);

	screenshot->image_count = height * width * (bpp/8)-1; //WARNING ignoring the alpha channel and assuming 3 channels!
	assert(screenshot->image_count > 0);
	screenshot->image = reinterpret_cast<unsigned char *>(malloc(screenshot->image_count));
	if (!(screenshot->image))
	{
		PLAYER_ERROR("Out of memory");
		return 1;//continue
	}
	screenshot->width = width;
	screenshot->height = height;
	screenshot->bpp = 24;	//WARNING ignoring the alpha channel and setting bpp=24!
	screenshot->fdiv = 0;
	screenshot->format = PLAYER_CAMERA_FORMAT_RGB888;

	//copy to image
	int red, blue, green,alpha,xx,yy;
	int byteIndex;
	for (xx=0 ; xx < width; xx++){
		for (yy=0 ; yy < height ; yy++){
			// Now your rawData contains the image data in the RGBA8888 pixel format.
			byteIndex = (bytesPerRow * yy) + xx * bytesPerPixel;
			red = rawData[byteIndex];
			green = rawData[byteIndex + 1];
			blue = rawData[byteIndex + 2];
			alpha = rawData[byteIndex + 3];//WARNING ignoring the alpha channel and setting bpp=24!
			
			int index = 3*xx+3*yy*width;  //orientation switch! 
			screenshot->image[index] = red;
			screenshot->image[index + 1] = green;
			screenshot->image[index + 2] = blue;
		}
	}
		
	return 0;
}

#elif defined(USE_X11)

void XGetImageDriver::DestroyImage(){
	if (this->xImageSample) XDestroyImage(this->xImageSample);
	this->xImageSample = NULL;
}

void XGetImageDriver::CloseDisplay(){
	if (this->display) XCloseDisplay(this->display);
	this->display = NULL;
}

Display* XGetImageDriver::OpenDisplay(){
	return XOpenDisplay(NULL);
}

int XGetImageDriver::CopyScreen(player_camera_data_t* screenshot){
	this->xImageSample = XGetImage(this->display, DefaultRootWindow(this->display), this->startX, this->startY, this->widthX, this->heightY, AllPlanes, ZPixmap);
	
	if (!this->xImageSample)
	{
		PLAYER_ERROR("No XImage!");
		return -1;//break
	}
	
	screenshot->image_count = this->xImageSample->width * this->xImageSample->height * this->xImageSample->depth/8;
	assert(screenshot->image_count > 0);
	screenshot->image = reinterpret_cast<unsigned char *>(malloc(screenshot->image_count));
	if (!(screenshot->image))
	{
		PLAYER_ERROR("Out of memory");
		return 1;//continue
	}
	screenshot->width = this->xImageSample->width;
	screenshot->height = this->xImageSample->height;
	screenshot->bpp = this->xImageSample->depth;
	screenshot->fdiv = 0;
	
	if (screenshot->bpp == 24)
	{
		screenshot->format = PLAYER_CAMERA_FORMAT_RGB888;
		// Some of the following code was borrowed for this driver from http://opencv.willowgarage.com/wiki/ximage2opencvimage
		// Some of the following code is borrowed from http://www.roard.com/docs/cookbook/cbsu19.html ("Screen grab with X11" - by Marko Riedel, with an idea by Alexander Malmberg)
		unsigned long rshift, rbits, gshift, gbits, bshift, bbits, rmask, gmask, bmask;
		unsigned char colorChannel[3];
		rmask = this->xImageSample->red_mask;
		gmask = this->xImageSample->green_mask;
		bmask = this->xImageSample->blue_mask;
		
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
		for ( int x = 0; x < this->xImageSample->width; x++) {
			for ( int y = 0; y < this->xImageSample->height; y++) {
				color.pixel = XGetPixel(this->xImageSample, x, y);
				colorChannel[0] = ((color.pixel >> bshift) & ((1 << bbits) - 1)) << (8 - bbits);
				colorChannel[1] = ((color.pixel >> gshift) & ((1 << gbits) - 1)) << (8 - gbits);
				colorChannel[2] = ((color.pixel >> rshift) & ((1 << rbits) - 1)) << (8 - rbits);
				
				int index = 3*x+3*y*this->xImageSample->width;  //orientation switch! 
				screenshot->image[index] =  colorChannel[2];
				screenshot->image[index + 1] = colorChannel[1];
				screenshot->image[index + 2] = colorChannel[0];
				
			}
		}
	}
	else{
		PLAYER_ERROR1("Unsupported image depth %d", screenshot->bpp);
		screenshot->bpp = 0;
		return -1;//break
	}
	
	
}

#elif defined(IS_WINDOWS)
//place holder
#endif
