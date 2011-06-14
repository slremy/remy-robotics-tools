/*
 * XKeyboardNavDriver.cpp
 *
 *  Created on: Jun 14, 2011
 *      Author: sr
 */

#include <assert.h>
#include <time.h>
#include <pthread.h>

#include "XKeyboardNavDriver.h"


Driver * XKeyboardNavDriver_Init(ConfigFile * cf, int section)
{
	return reinterpret_cast<Driver *>(new XKeyboardNavDriver(cf, section));
}

void XKeyboardNavDriver_Register(DriverTable *table)
{
	table->AddDriver("xkeyboardnavdriver", XKeyboardNavDriver_Init);
}

XKeyboardNavDriver::XKeyboardNavDriver(ConfigFile * cf, int section)
: ThreadedDriver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN)
{
	this->display = NULL;
	this->screen = NULL;

	this->wasd_subscriptions=0;
	this->ulrd_subscriptions=0;
	
	memset(&this->wasd_id, 0, sizeof(player_devaddr_t));
	memset(&this->ulrd_id, 0, sizeof(player_devaddr_t));
		
	// Do we create a wasd position interface?
	if(cf->ReadDeviceAddr(&(this->wasd_id), section, "provides",
						  PLAYER_POSITION2D_CODE, -1, "wasd") == 0)
	{
		if(this->AddInterface(this->wasd_id) != 0)
		{
			PLAYER_ERROR("Couldn't add WASD interface.");
			return;
		}
	}
	// Do we create an ulrd position interface?
	if(cf->ReadDeviceAddr(&(this->ulrd_id), section, "provides",
						  PLAYER_POSITION2D_CODE, -1, "ulrd") == 0)
	{
		if(this->AddInterface(this->ulrd_id) != 0)
		{
			PLAYER_ERROR("Couldn't add ulrd interface.");
			return;
		}
	}
	
	this->sleep_nsec = cf->ReadInt(section, "sleep_nsec", 10000000);

	/*these default keycodes are were generated on osx.  
	 Run xev on your machine to detmine your approriate values */

	this->WASD_keycodes[0] = cf->ReadTupleInt(section, "WASD_keycodes", 0, 21);
	this->WASD_keycodes[1] = cf->ReadTupleInt(section, "WASD_keycodes", 1, 29);
	this->WASD_keycodes[2] = cf->ReadTupleInt(section, "WASD_keycodes", 2, 9 );
	this->WASD_keycodes[3] = cf->ReadTupleInt(section, "WASD_keycodes", 3, 10);

	this->ULDR_keycodes[0] = cf->ReadTupleInt(section, "ULDR_keycodes", 0, 134);
	this->ULDR_keycodes[1] = cf->ReadTupleInt(section, "ULDR_keycodes", 1, 131);
	this->ULDR_keycodes[2] = cf->ReadTupleInt(section, "ULDR_keycodes", 2, 133);
	this->ULDR_keycodes[3] = cf->ReadTupleInt(section, "ULDR_keycodes", 3, 132);
	WASD_status[0]=WASD_status[1]=WASD_status[2]=WASD_status[3]=false;
	ULDR_status[0]=ULDR_status[1]=ULDR_status[2]=ULDR_status[3]=false;
}

XKeyboardNavDriver::~XKeyboardNavDriver()
{
	// Always clean up your mess
	if (this->display) XCloseDisplay(this->display);
}


 int XKeyboardNavDriver::Subscribe(player_devaddr_t id)
{
	int setupResult;
	
	// do the subscription
	if((setupResult = Driver::Subscribe(id)) == 0)
	{
		// also increment the appropriate subscription counter
		if(Device::MatchDeviceAddress(id, this->wasd_id))
			this->wasd_subscriptions++;
		else if(Device::MatchDeviceAddress(id, this->ulrd_id))
			this->ulrd_subscriptions++;
	}
	
	return(setupResult);
}

int XKeyboardNavDriver::Unsubscribe(player_devaddr_t id)
{
	int shutdownResult;
	
	// do the unsubscription
	if((shutdownResult = Driver::Unsubscribe(id)) == 0)
	{
		// also decrement the appropriate subscription counter
		if(Device::MatchDeviceAddress(id, this->wasd_id))
		{
			this->wasd_subscriptions--;
			assert(this->wasd_subscriptions >= 0);
		}
		else if(Device::MatchDeviceAddress(id, this->ulrd_id))
		{
			this->ulrd_subscriptions--;
			assert(this->ulrd_subscriptions >= 0);
		}
	}
	
	return(shutdownResult);
}


int XKeyboardNavDriver::MainSetup()
{
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

void XKeyboardNavDriver::MainQuit()
{
	if (this->display) XCloseDisplay(this->display);
	this->display = NULL;
	this->screen = NULL;
}

void XKeyboardNavDriver::Main()
{
	struct timespec tspec;
	
	if (!(this->screen))
	{
		PLAYER_ERROR("Couldn't access screen-> Something is wrong with your XLib.");
		return;
	}
	
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
	}
}

int XKeyboardNavDriver::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data)
{
	if(hdr->type == PLAYER_MSGTYPE_CMD)
	{
		float px;
		float pa;
		if(Message::MatchMessage(hdr,
								 PLAYER_MSGTYPE_CMD,
								 PLAYER_POSITION2D_CMD_VEL,
								 this->wasd_id))
		{
			// get and send the latest motor command
			player_position2d_cmd_vel_t position_cmd;
			position_cmd = *(player_position2d_cmd_vel_t*)data;
			px = (float)(position_cmd.vel.px * 1e3);
			if ( px > .5) {
				PLAYER_WARN1("W:%d",this->WASD_keycodes[0]);
				if (!WASD_status[0])
				{
					XTestFakeButtonEvent(this->display, XKeysymToKeycode(this->display,WASD_keycodes[0]), True, CurrentTime);
					WASD_status[0]=true;
				}
				if (WASD_status[1])
				{
					XTestFakeButtonEvent(this->display, XKeysymToKeycode(this->display,WASD_keycodes[1]), False, CurrentTime);
					WASD_status[1]=false;
				}
			}else if (px <-.5) {
				PLAYER_WARN1("S:%d",this->WASD_keycodes[1]);
				if (!WASD_status[1])
				{
					XTestFakeButtonEvent(this->display, XKeysymToKeycode(this->display,WASD_keycodes[1]), True, CurrentTime);
					WASD_status[1]=true;
				}
				if (WASD_status[0])
				{
					XTestFakeButtonEvent(this->display, XKeysymToKeycode(this->display,WASD_keycodes[0]), False, CurrentTime);
					WASD_status[0]=false;
				}
			}else {
				if (WASD_status[0])
				{
					XTestFakeButtonEvent(this->display, XKeysymToKeycode(this->display,WASD_keycodes[0]), False, CurrentTime);
					WASD_status[0]=false;
				}
				if (WASD_status[1])
				{
					XTestFakeButtonEvent(this->display, XKeysymToKeycode(this->display,WASD_keycodes[1]), False, CurrentTime);
					WASD_status[1]=false;
				}
			}

			pa = (float)position_cmd.vel.pa;
			if (pa > .5) {
				PLAYER_WARN1("A:%d",XKeysymToKeycode(this->display,this->WASD_keycodes[2]));
				;//left
			}else if (pa<-.5) {
				PLAYER_WARN1("D:%d",XKeysymToKeycode(this->display,this->WASD_keycodes[3]));
				;//right
			}else {
				;//stop
			}
			return 0;
		}
		else if(Message::MatchMessage(hdr,
										 PLAYER_MSGTYPE_CMD,
										 PLAYER_POSITION2D_CMD_VEL,
										 this->ulrd_id))
		{
			// get and send the latest motor command
			player_position2d_cmd_vel_t position_cmd;
			position_cmd = *(player_position2d_cmd_vel_t*)data;
			return 0;
		}
		else 
		{
			return -1;
		}
	}
	else
	{
		if(Message::MatchMessage(hdr,
									  PLAYER_MSGTYPE_REQ,
									  PLAYER_POSITION2D_REQ_GET_GEOM,
									  this->ulrd_id))
		{
			// get and send the latest motor command
			player_position2d_geom_t* dummy = new player_position2d_geom_t;
			dummy->pose.px = 0;
			dummy->pose.py = 0;
			dummy->pose.pyaw = 0;
			Publish(this->ulrd_id, resp_queue, PLAYER_MSGTYPE_RESP_ACK, hdr->subtype, (void*)&dummy, sizeof(player_position2d_geom_t));
			return 0;
		}
		if(Message::MatchMessage(hdr,
								 PLAYER_MSGTYPE_REQ,
								 PLAYER_POSITION2D_REQ_GET_GEOM,
								 this->wasd_id))
		{
			// get and send the latest motor command
			player_position2d_geom_t* dummy = new player_position2d_geom_t;
			dummy->pose.px = 10;
			dummy->pose.py = 0;
			dummy->pose.pyaw = 0;
			Publish(this->wasd_id, resp_queue, PLAYER_MSGTYPE_RESP_ACK, hdr->subtype, (void*)&dummy, sizeof(player_position2d_geom_t));
			return 0;
		}
		assert(hdr);
		return -1;
	}
}

extern "C"
{
	int player_driver_init(DriverTable * table)
	{
		puts("XKeyboardNavDriver driver initializing");
		XKeyboardNavDriver_Register(table);
		
		return 0;
	}
	
}
