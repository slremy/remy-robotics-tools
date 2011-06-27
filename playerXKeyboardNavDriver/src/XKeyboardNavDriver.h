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
// Desc: Keyboard Navigation driver
// Author: sr
// Date: 14 Jun 2011
//
///////////////////////////////////////////////////////////////////////////

/** @ingroup drivers */
/** @{ */
/** @defgroup driver_XKeyboardNavDriver XKeyboardNavDriver
 * @brief wasd and ulrd (arrow) key streaming sink
 
 The XKeyboardNavDriver driver position2d commands and sends them to a modern operating system using the autopy infrastructure.
 
 @par Compile-time dependencies
 
 - autopy
 
 @par Provides
 
 - @ref interface_position2d
 
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
 name "xkeyboardnavdriver"
 plugin "xkeyboardnavdriver.so"
 provides [
 "ulrd:::position2d:0"
 "wasd:::position2d:1"
 ]
 
 sleep_nsec 33300000
 )
 
 @endverbatim
 
 @author sr
 
 */
/** @} */

#ifndef XKeyboardNavDriver_H_
#define XKeyboardNavDriver_H_

#include <libplayercore/playercore.h>
#include <cstring>

extern "C"{
	#include "keypress.h"
}
//---------------------------------

class XKeyboardNavDriver : public ThreadedDriver
{
public: XKeyboardNavDriver(ConfigFile * cf, int section);
public: virtual ~XKeyboardNavDriver();
	
public: virtual int MainSetup();
public: virtual void MainQuit();
	
	// This method will be invoked on each incoming message
public: virtual int ProcessMessage(QueuePointer & resp_queue,
								   player_msghdr * hdr,
								   void * data);
public: virtual int Subscribe(player_devaddr_t id);
public: virtual int Unsubscribe(player_devaddr_t id);	

private: virtual void Main();

private: player_devaddr_t wasd_id;
private: player_position2d_data_t wasd;
private: player_devaddr_t ulrd_id;
private: player_position2d_data_t ulrd;
private: int wasd_subscriptions;
private: int ulrd_subscriptions;
private: MMKeyCode WASD_keycodes[4];
private: MMKeyCode ULDR_keycodes[4];
private: bool WASD_status[4];
private: bool ULDR_status[4];
private: int sleep_nsec;
};

#endif /* XKeyboardNavDriver_H_ */
