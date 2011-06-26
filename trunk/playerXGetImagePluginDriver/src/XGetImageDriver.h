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

#include <libplayercore/playercore.h>
#include <cstring>
// X Server includes
#include <X11/Xlib.h>
#include <X11/Xutil.h>

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
	
    // X resources
private: Display* display;
private: XImage* xImageSample;
	
private: int startX;
private: int startY;
private: int widthX;
private: int heightY;
private: int sleep_nsec;
};

#endif /* XGetImageDriver_H_ */
