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
	this->screen = NULL;
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
	if (this->xImageSample) XDestroyImage(this->xImageSample);
	if (this->display) XCloseDisplay(this->display);
}

int XGetImageDriver::MainSetup()
{
	if (this->xImageSample) XDestroyImage(this->xImageSample);
	if (this->display) XCloseDisplay(this->display);
	this->display = XOpenDisplay(NULL); // Open first (-best) display
    this->screen = DefaultScreenOfDisplay(display);
	
	if (!(this->display))
	{
		PLAYER_ERROR("Couldn't create X capture display. Something is wrong with your XLib.");
		return -1;
	}
	return 0;
}

void XGetImageDriver::MainQuit()
{
	if (this->xImageSample) XDestroyImage(this->xImageSample);
	if (this->display) XCloseDisplay(this->display);
	this->display = NULL;
	this->screen = NULL;
	this->xImageSample = NULL;
}

void XGetImageDriver::Main()
{
	struct timespec tspec;
	XColor color;
	player_camera_data_t * data;
	
	if (!(this->screen))
	{
		PLAYER_ERROR("Couldn't access screen-> Something is wrong with your XLib.");
		return;
	}
	PLAYER_WARN2("Achieved base screen size %.4f x %.4f", this->screen->width , this->screen->height);
	
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
		
		if (!(this->screen))
		{
			PLAYER_ERROR("No screen->");
			break;
		}
		if (!this->display)
		{
			PLAYER_ERROR("No display!");
			break;
		}
		
		this->xImageSample = XGetImage(this->display, DefaultRootWindow(this->display), this->startX, this->startY, this->widthX, this->heightY, AllPlanes, ZPixmap);
		
		if (!this->xImageSample)
		{
			PLAYER_ERROR("No XImage!");
			break;
		}
		
		/*
		 if (frame->depth != IPL_DEPTH_8U)
		 {
		 PLAYER_ERROR1("Unsupported depth %d", frame->depth);
		 continue;
		 }
		 */
		
		data = reinterpret_cast<player_camera_data_t *>(malloc(sizeof(player_camera_data_t)));
		if (!data)
		{
			PLAYER_ERROR("Out of memory");
			break;
		}
		
		data->image_count = this->xImageSample->width * this->xImageSample->height * this->screen->depths->depth/8;
		assert(data->image_count > 0);
		data->image = reinterpret_cast<unsigned char *>(malloc(data->image_count));
		if (!(data->image))
		{
			PLAYER_ERROR("Out of memory");
			continue;
		}
		data->width = this->xImageSample->width;
		data->height = this->xImageSample->height;
		data->bpp = this->screen->depths->depth;
		data->fdiv = 0;
		
		switch (data->bpp)
		{
			case 24:
				data->format = PLAYER_CAMERA_FORMAT_RGB888;
				/*				for (i = 0; i < static_cast<int>(data->image_count); i += (frame->nChannels))
				 {
				 data->image[i] = frame->imageData[i + 2];
				 data->image[i + 1] = frame->imageData[i + 1];
				 data->image[i + 2] = frame->imageData[i];
				 }
				 */				
				
				// Some of the following code is borrowed from http://www.roard.com/docs/cookbook/cbsu19.html ("Screen grab with X11" - by Marko Riedel, with an idea by Alexander Malmberg)
				unsigned long rshift, rbits, gshift, gbits, bshift, bbits, rmask, gmask, bmask;
				unsigned char colorChannel[3];
				rmask = this->screen->root_visual->red_mask;
                gmask = this->screen->root_visual->green_mask;
                bmask = this->screen->root_visual->blue_mask;
				
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
				
				for ( int x = 0; x < this->xImageSample->width; x++) {
					for ( int y = 0; y < this->xImageSample->height; y++) {
						color.pixel = XGetPixel(this->xImageSample, x, y);
						colorChannel[0] = ((color.pixel >> bshift) & ((1 << bbits) - 1)) << (8 - bbits);
						colorChannel[1] = ((color.pixel >> gshift) & ((1 << gbits) - 1)) << (8 - gbits);
						colorChannel[2] = ((color.pixel >> rshift) & ((1 << rbits) - 1)) << (8 - rbits);
						
						int index = y+x*this->xImageSample->width;
						data->image[index] =  colorChannel[0];
						data->image[index + 1] = colorChannel[1];
						data->image[index + 2] = colorChannel[2];
						
						//CV_IMAGE_ELEM(ocvImage, uchar, y, x * ocvImage->nChannels) = colorChannel[0];
						//CV_IMAGE_ELEM(ocvImage, uchar, y, x * ocvImage->nChannels + 1) = colorChannel[1];
						//CV_IMAGE_ELEM(ocvImage, uchar, y, x * ocvImage->nChannels + 2) = colorChannel[2];
					}
				}
				
				
				break;
				/*
				 case 8:
				 data->format = PLAYER_CAMERA_FORMAT_MONO8;
				 memcpy(data->image, frame->imageData, data->image_count);
				 break;
				 case 32:
				 data->format = PLAYER_CAMERA_FORMAT_RGB888;
				 for (i = 0; i < static_cast<int>(data->image_count); i += (frame->nChannels))
				 {
				 data->image[i] = frame->imageData[i + 2];
				 data->image[i + 1] = frame->imageData[i + 1];
				 data->image[i + 2] = frame->imageData[i];
				 data->image[i + 3] = frame->imageData[i + 3];
				 }
				 break;
				 */
			default:
				PLAYER_ERROR1("Unsupported image depth %d", data->bpp);
				data->bpp = 0;
		}
		assert(data->bpp > 0);
		data->compression = PLAYER_CAMERA_COMPRESS_RAW;
		this->Publish(device_addr, PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE, reinterpret_cast<void *>(data), 0, NULL, false);
		// copy = false, don't dispose anything here
		
		/*not sure if the above statement applied to destroy image, no cvrelease :(*/XDestroyImage(this->xImageSample);
		
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
