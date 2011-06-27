/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2000  Brian Gerkey et al.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
///////////////////////////////////////////////////////////////////////////
//
// Desc: Xwindows screen capture driver
// Author: sr
// Date: 10 Jun 2011
//
///////////////////////////////////////////////////////////////////////////

/** @ingroup drivers */
/** @{ */
/** @defgroup driver_XGetImageDriver XGetImageDriver
 * @brief XGetImage streaming source
 
 The XGetImageDriver driver captures images from an Xwindows desktop using the xlib infrastructure.
 
 @par Compile-time dependencies
 
 - xlib
 
 @par Provides
 
 - @ref interface_camera
 
 @par Requires
 
 - none
 
 @par Configuration requests
 
 - none
 
 @par Configuration file options
  
 - sleep_nsec (integer)
 - Default: 10000000 (=10ms which gives max 100 fps)
 - timespec value for nanosleep()
 
 @par Example
 
 @verbatim
 driver
 (
  name "xgetimagedriver"
  plugin "xgetimagedriver.so"
  provides ["camera:0"]

  startX 0
  startY 0
  widthX 100
  heightY 100
  sleep_nsec 33300000

 )
 @endverbatim
 
 @author sr
 
 */
/** @} */

#ifndef XGetImageDriver_H_
#define XGetImageDriver_H_

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
#elif !defined(IS_MACOSX) && !defined(USE_X11)
	#error "Sorry, this platform isn't supported yet!"
#endif


#include <libplayercore/playercore.h>
#include <cstring>





//---------------------------------

class XGetImageDriver : public ThreadedDriver
{
public: XGetImageDriver(ConfigFile * cf, int section);
public: virtual ~XGetImageDriver();
	
public: virtual int MainSetup();
public: virtual void MainQuit();
	
	// This method will be invoked on each incoming message
public: virtual int ProcessMessage(QueuePointer & resp_queue,
								   player_msghdr * hdr,
								   void * data);
	
private: virtual void Main();

#if defined(IS_MACOSX)
private: CGDirectDisplayID OpenDisplay();
    // OSX/OpenGL resources
private: CGDirectDisplayID display;
private: CGImageRef xImageSample;
	
#elif defined(USE_X11)
private: Display* OpenDisplay();
    // X resources
private: Display* display;
private: XImage* xImageSample;
	
#elif defined(IS_WINDOWS)
	//place holder
#endif
	
private: void DestroyImage();
private: void CloseDisplay();
private: int CopyScreen(player_camera_data_t* data);


private: int startX;
private: int startY;
private: int widthX;
private: int heightY;
private: int sleep_nsec;
};

#endif /* XGetImageDriver_H_ */
