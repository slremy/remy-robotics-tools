/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2003
 *     Brian Gerkey, Andrew Howard
 *
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

/*
 * Desc: Read data from a joystick/gamepad using the stjoystick class
 * Author: S Remy
 * Date: 29 June 2011
 *
 */

/** @ingroup drivers */
/** @{ */
/** @defgroup driver_stjoystick stjoystick
 * @brief STjoy joystick
 
 The stjoystick driver reads data from a standard Linux joystick and
 provides the data via the @ref interface_joystick interface.
 This driver can also control a @ref interface_position2d device by
 converting joystick positions to velocity commands.
 
 @par Compile-time dependencies
 
 - &lt;STJoy/STJoystick.h&gt;
 
 @par Provides
 
 - @ref interface_joystick : joystick data
 - @ref interface_position2d : joystick data represented as 2-D
 position data.  Raw X-axis, Y-axis and Yaw-axis values are reported
 as pos[0], pos[1] and pos[3] in the position packet (all other
 fields are zero).
 
 @par Requires
 
 - @ref interface_position2d : if present, joystick positions will be
 interpreted as velocities and sent as commands to this position2d device.
 See also max_speed, and deadman_button options below.
 - @ref interface_gripper : if present, joystick buttons will be used to
 control given gripper (open, close, store, retrieve, stop).
 
 @par Configuration requests
 
 - None
 
 @par Configuration file options
 
 - port (int)
 - Default: 0
 - The "index" of the joystick to be used.
 - axes (integer tuple)
 - Default: [1 2 0]
 - Which joystick axes to call the "X" , "Y" and "Yaw" axes, respectively.
 - axes_maxima (integer tuple)
 - Default: [127 127 127]
 - Maximum absolute values attainable on the X, Y and Yaw axes, respectively.
 - axes_minima (integer tuple)
 - Default: [0 0 0]
 - Minimum values on the X and Yaw axes, respectively.  Anything smaller
 in absolute value than this limit will be reported as zero.
 Useful for implementing a dead zone on a touchy joystick.
 - deadman_button (integer)
 - Default: -1
 - When controlling a @ref interface_position2d device, if deadman_button is
 >= 0, this joystick button must be depressed for commands to be
 sent to that device.
 - max_speed (float tupple, m / sec or angle / sec)
 - Default: [0.5 0.5 30]
 - The maximum absolute X and Y translational and rotational velocities to be
 used when commanding a position device. (Y is only revelant for
 holonomous robot)
 - scale_pos (float tuple)
 - Default: [1.0 1.0 1.0]
 - Position2d readings scale
 - timeout (float)
 - Default: 5.0
 - Time (in seconds) since receiving a new joystick event after which
 the underlying position device will be stopped, for safety.  Set to
 0.0 for no timeout.
 
 - Notes:
 - Joysticks use X for side-to-side and Y for forward-back, also
 their axes are backwards with respect to intuitive driving
 controls.
 - This driver does not swap any axis, you have to handle this in the
 configuration file options via "axes". However the defaults values
 should suit your needs.
 - This driver reverse the axes so that the joystick respect the
 intuitive driving controls.
 - The Y axis is only revelant for holonomous platform (like the WizBot).
 @par Examples
 
 Basic configuration
 
 @verbatim
 driver
 (
 name "stjoystick"
 provides ["joystick:0"]
 port 0
 )
 @endverbatim
 
 Provide a position2d interface, instead of a joystick interface.
 
 @verbatim
 driver
 (
 name "stjoystick"
 plugin "stjoystick.so"
 provides ["position2d:0"]
 port 0
 scale_pos [ 0.0001 0.0001 0.0001 ]
 )
 @endverbatim
 
 Controlling a Pioneer, plus remapping joystick axes and setting various
 limits.
 
 @verbatim
 driver
 (
 name "p2os"
 provides ["odometry::position:0"]
 port "/dev/usb/tts/0"
 )
 
 # 1 m/sec max linear velocity
 # 30 deg/sec max angular velocity
 # Axis 4 is X
 # Axis 3 is Yaw
 # Y is not used here
 driver
 (
 name "stjoystick"
 plugin "stjoystick.so"
 provides ["joystick:0"]
 requires ["odometry::position:0"]
 max_speed [1 0 30]
 axes [4 -1 3]
 axes_minima [50 0 50]
 port 0
 alwayson 1
 )
 @endverbatim
 
 Controlling a WizBot in Gazebo, plus remapping joystick axes.
 
 @verbatim
 driver
 (
 name "gazebo"
 provides ["simulation:0"]
 plugin "libgazeboplugin"
 server_id "default"
 )
 
 driver
 (
 name "gazebo"
 provides ["position2d:0"]
 gz_id "position_iface_0"
 )
 
 # 0.5 m/sec max linear velocity
 # 120 deg/sec max angular velocity
 # Axis 1 is X
 # Axis 2 is Y
 # Axis 0 is Yaw
 driver
 (
 name "stjoystick"
 plugin "stjoystick.so"
 provides ["joystick:0"]
 requires ["position2d:0"]
 max_speed [0.5 0.5 120]
 axes [1 2 0]
 port 0
 alwayson 1
 )
 @endverbatim
 
 @todo
 Add support for continuously sending commands, which might be needed for
 position devices that use watchdog timers.
 
 @author Andrew Howard, Brian Gerkey, Paul Osmialowski, S Remy
 
 */
/** @} */

#include "STJoy/STJoystick.h"

#include <libplayercore/playercore.h>

#define XAXIS 1
#define YAXIS 2
#define YAWAXIS 0
#define SLIDER 3

#define MAX_XSPEED 0.5
#define MAX_YSPEED 0.5
#define MAX_YAWSPEED 30.0

#define XAXIS_MAX ((int16_t) 127)
#define YAXIS_MAX ((int16_t) 127)
#define YAWAXIS_MAX ((int16_t) 127)

#define XAXIS_MIN ((int16_t) 0)
#define YAXIS_MIN ((int16_t) 0)
#define YAWAXIS_MIN ((int16_t) 0)

////////////////////////////////////////////////////////////////////////////////
// The class for the driver
class STjoyJoystick : public ThreadedDriver
{
	// Constructor; need that
public: STjoyJoystick(ConfigFile* cf, int section);
	
	// Must implement the following methods.
public: int MainSetup();
public: void MainQuit();
	
	// Main function for device thread.
private: virtual void Main();
	
public: virtual int ProcessMessage(QueuePointer & resp_queue,
								   player_msghdr * hdr,
								   void * data) {return 0;}
	
	// Read the joystick
private: void ReadJoy();
	
	// Write new data to server
private: void RefreshData();
	
	// Check for new configuration requests
	//private: void CheckConfig();
	
	// Put new position command
private: void PutPositionCommand();
	
	// Joystick device
private: player_devaddr_t joystick_addr;
private: int dev;
private: STJoystick* joy;
private: int axes_count;
private: int32_t pos[4];
private: uint32_t buttons;
private: int axes_max[4];
private: int axes_min[4];
private: double scale_pos[4];
private: double timeout;
private: struct timeval lastread;
	
	// Position device
private: player_devaddr_t position_addr;
private: player_position2d_data_t pos_data;
	
	// These are used when we send commands to a position device
private: double max_speed[4];
private: int axes[4];
private: int deadman_button;
private: player_devaddr_t cmd_position_addr;
private: Device * position;
private: player_devaddr_t cmd_gripper_addr;
private: Device * gripper;
	
	// Joystick
private: player_joystick_data_t joy_data;
};


////////////////////////////////////////////////////////////////////////////////
// A factory creation function, declared outside of the class so that it
// can be invoked without any object context (alternatively, you can
// declare it static in the class).  In this function, we create and return
// (as a generic Driver*) a pointer to a new instance of this driver.
Driver* STjoyJoystick_Init(ConfigFile* cf, int section)
{
	// Create and return a new instance of this driver
	return reinterpret_cast<Driver *>(new STjoyJoystick(cf, section));
}


////////////////////////////////////////////////////////////////////////////////
// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void STjoyJoystick_Register(DriverTable* table)
{
	table->AddDriver("stjoystick", STjoyJoystick_Init);
}


////////////////////////////////////////////////////////////////////////////////
// Constructor.  Retrieve options from the configuration file and do any
// pre-Setup() setup.
STjoyJoystick::STjoyJoystick(ConfigFile* cf, int section) : ThreadedDriver(cf, section)
{
	// zero ids, so that we'll know later which interfaces were requested
	memset(&(this->cmd_position_addr), 0, sizeof(player_devaddr_t));
	memset(&(this->cmd_gripper_addr), 0, sizeof(player_devaddr_t));
	memset(&(this->position_addr), 0, sizeof(player_devaddr_t));
	memset(&(this->joystick_addr), 0, sizeof(player_devaddr_t));
	this->position = NULL;
	this->gripper = NULL;
	
	// Do we create a position interface?
	if(cf->ReadDeviceAddr(&(this->position_addr), section, "provides",
						  PLAYER_POSITION2D_CODE, -1, NULL) == 0)
	{
		if(this->AddInterface(this->position_addr))
		{
			this->SetError(-1);
			return;
		}
	}
	// Do we create a joystick interface?
	if(cf->ReadDeviceAddr(&(this->joystick_addr), section, "provides",
						  PLAYER_JOYSTICK_CODE, -1, NULL) == 0)
	{
		if(this->AddInterface(this->joystick_addr))
		{
			this->SetError(-1);
			return;
		}
	}
	
	this->dev = cf->ReadInt(section, "port", 0);
	// partially implementing axes_count
	this->axes_count=4;
	this->axes[0] = cf->ReadTupleInt(section,"axes", 0, XAXIS);
	this->axes[1] = cf->ReadTupleInt(section,"axes", 1, YAXIS);
	this->axes[2] = cf->ReadTupleInt(section,"axes", 2, YAWAXIS);
	this->axes[3] = cf->ReadTupleInt(section,"axes", 3, SLIDER);
	
	
	this->deadman_button = cf->ReadInt(section,"deadman", -1);
	this->axes_max[0] = cf->ReadTupleInt(section, "axes_maxima", 0, XAXIS_MAX);
	this->axes_max[1] = cf->ReadTupleInt(section, "axes_maxima", 1, YAXIS_MAX);
	this->axes_max[2] = cf->ReadTupleInt(section, "axes_maxima", 2, YAWAXIS_MAX);
	this->axes_min[0] = cf->ReadTupleInt(section, "axes_minima", 0, XAXIS_MIN);
	this->axes_min[1] = cf->ReadTupleInt(section, "axes_minima", 1, YAXIS_MIN);
	this->axes_min[2] = cf->ReadTupleInt(section, "axes_minima", 2, YAWAXIS_MIN);
	this->scale_pos[0] = cf->ReadTupleFloat(section, "scale_pos", 0, 1.0);
	this->scale_pos[1] = cf->ReadTupleFloat(section, "scale_pos", 1, 1.0);
	this->scale_pos[2] = cf->ReadTupleFloat(section, "scale_pos", 2, 1.0);
	
	// Do we talk to any device?
	if(cf->GetTupleCount(section, "requires"))
	{
		if(cf->ReadDeviceAddr(&(this->cmd_position_addr), section, "requires",
							  PLAYER_POSITION2D_CODE, -1, NULL) == 0)
		{
			this->max_speed[0] = cf->ReadTupleFloat(section, "max_speed", 0, MAX_XSPEED);
			this->max_speed[1] = cf->ReadTupleFloat(section, "max_speed", 1, MAX_YSPEED);
			this->max_speed[2] = DTOR(cf->ReadTupleFloat(section, "max_speed", 2, MAX_YAWSPEED));
			this->timeout = cf->ReadFloat(section, "timeout", 5.0);
		} else memset(&(this->cmd_position_addr), 0, sizeof(player_devaddr_t));
		if (cf->ReadDeviceAddr(&(this->cmd_gripper_addr), section, "requires",
							   PLAYER_GRIPPER_CODE, -1, NULL) == 0)
		{
		} else memset(&(this->cmd_gripper_addr), 0, sizeof(player_devaddr_t));
	}
	
	return;
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device.  Return 0 if things go well, and -1 otherwise.
int STjoyJoystick::MainSetup()
{
	// Open the joystick device
	if (STJoystick::NumJoysticks() < 1)
	{
		PLAYER_ERROR1("unable to find joysticks or error reading list; %d",
					  STJoystick::NumJoysticks());
		return -1;
	}
	
	this->joy = STJoystick::OpenJoystick(this->dev);
	if (this->joy == NULL)
	{
		PLAYER_ERROR2("unable to open joystick [%s]; %s",
					  this->dev, strerror(errno));
		return -1;
	}
	
	this->lastread.tv_sec = this->lastread.tv_usec = 0;
	
	this->position = NULL;
	// If we're asked, open the position2d device
	if (this->cmd_position_addr.interf)
	{
		if (!(this->position = deviceTable->GetDevice(this->cmd_position_addr)))
		{
			PLAYER_ERROR("unable to locate suitable position2d device");
			return -1;
		}
		if (this->position->Subscribe(this->InQueue) != 0)
		{
			PLAYER_ERROR("unable to subscribe to position2d device");
			return -1;
		}
	}
	this->gripper = NULL;
	// If we're asked, open the gripper device
	if (this->cmd_gripper_addr.interf)
	{
		this->gripper = deviceTable->GetDevice(this->cmd_gripper_addr);
		if (!(this->gripper))
		{
			PLAYER_ERROR("unable to locate suitable gripper device");
			if ((this->cmd_position_addr.interf) && (this->position))
				this->position->Unsubscribe(this->InQueue);
			return -1;
		}
		if (this->gripper->Subscribe(this->InQueue) != 0)
		{
			PLAYER_ERROR("unable to subscribe to gripper device");
			if ((this->cmd_position_addr.interf) && (this->position))
				this->position->Unsubscribe(this->InQueue);
			return -1;
		}
	}
	
	this->pos[0] = this->pos[1] = this->pos[2] = this->pos[3] = 0;
	
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Shutdown the device
void STjoyJoystick::MainQuit()
{
	if ((this->cmd_position_addr.interf) && (this->position))
		this->position->Unsubscribe(this->InQueue);
	if ((this->cmd_gripper_addr.interf) && (this->gripper))
		this->gripper->Unsubscribe(this->InQueue);
	
	// Close the joystick
	joy->Close();
}


////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void STjoyJoystick::Main()
{
	if (this->cmd_position_addr.interf)
	{
		// Enable the motors
		player_position2d_power_config_t motorconfig;
		memset(&motorconfig, 0, sizeof motorconfig);
		motorconfig.state = 1;
		Message* msg;
		if (!(msg = this->position->Request(this->InQueue,
											PLAYER_MSGTYPE_REQ,
											PLAYER_POSITION2D_REQ_MOTOR_POWER,
											reinterpret_cast<void *>(&motorconfig),
											sizeof motorconfig, NULL)))
		{
			PLAYER_WARN("failed to enable motors");
		}
		else
			delete msg;
		
		// Stop the robot
		player_position2d_cmd_vel_t cmd;
		memset(&cmd, 0, sizeof cmd);
		this->position->PutMsg(this->InQueue,
							   PLAYER_MSGTYPE_CMD,
							   PLAYER_POSITION2D_CMD_VEL,
							   reinterpret_cast<void *>(&cmd), sizeof cmd,
							   NULL);
	}
	// The main loop; interact with the device here
	while (true)
	{
		// test if we are supposed to cancel
		pthread_testcancel();
		
		// Process incoming messages
		this->ProcessMessages();
		
		// Run and process output
		this->ReadJoy();
		
		// Write outgoing data
		this->RefreshData();
		
		// Send new commands to position device
		if(this->cmd_position_addr.interf)
		{
			if((this->deadman_button < 0) ||
			   ((this->buttons >> this->deadman_button) & 0x01))
			{
				this->PutPositionCommand();
			}
			else
			{
				player_position2d_cmd_vel_t cmd;
				memset(&cmd,0,sizeof(cmd));
				this->position->PutMsg(this->InQueue,
									   PLAYER_MSGTYPE_CMD,
									   PLAYER_POSITION2D_CMD_VEL,
									   (void*)&cmd, sizeof(player_position2d_cmd_vel_t),
									   NULL);
			}
		}
		// Send new commands to gripper device
		if (this->cmd_gripper_addr.interf)
		{
			if (joy->GetButton(0))
			{
				this->gripper->PutMsg(this->InQueue, PLAYER_MSGTYPE_CMD, PLAYER_GRIPPER_CMD_CLOSE, NULL, 0, NULL);
			} else if (joy->GetButton(1))
			{
				this->gripper->PutMsg(this->InQueue, PLAYER_MSGTYPE_CMD, PLAYER_GRIPPER_CMD_OPEN, NULL, 0, NULL);
			} else if (joy->GetButton(2))
			{
				this->gripper->PutMsg(this->InQueue, PLAYER_MSGTYPE_CMD, PLAYER_GRIPPER_CMD_STORE, NULL, 0, NULL);
			} else if (joy->GetButton(3))
			{
				this->gripper->PutMsg(this->InQueue, PLAYER_MSGTYPE_CMD, PLAYER_GRIPPER_CMD_RETRIEVE, NULL, 0, NULL);
			} else if (joy->GetButton(4))
			{
				this->gripper->PutMsg(this->InQueue, PLAYER_MSGTYPE_CMD, PLAYER_GRIPPER_CMD_STOP, NULL, 0, NULL);
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Read the joystick
void STjoyJoystick::ReadJoy()
{
	// get the next event from the joystick
	if (this->joy == NULL)
	{
		PLAYER_ERROR("Failed to read from the joystick");
		return;
	}
	
	this->joy->Update();
	
	// Update buttons
	this->buttons = 0;
	for (int buttons_index = 0 ; buttons_index <  this->joy->NumButtons() ; buttons_index++)
		this->buttons |= (1 <<  this->joy->GetButton(buttons_index));
	
	for (int axes_index = 0 ; axes_index <  this->joy->NumAxes() ; axes_index++){
		if(axes_index == this->axes[0])
		{
			this->pos[0] =  this->joy->GetAxis(axes_index);
			if(abs(this->pos[0]) < this->axes_min[0])
				this->pos[0] = 0;
			GlobalTime->GetTime(&this->lastread);
		}
		else if(axes_index == this->axes[1])
		{
			this->pos[1] =  this->joy->GetAxis(axes_index);
			if(abs(this->pos[1]) < this->axes_min[1])
				this->pos[1] = 0;
			GlobalTime->GetTime(&this->lastread);
		}
		else if(axes_index == this->axes[2])
		{
			this->pos[2] =joy->GetAxis(axes_index);
			if(abs(this->pos[2]) < this->axes_min[2])
				this->pos[2] = 0;
			GlobalTime->GetTime(&this->lastread);
		}
		else if(axes_index == this->axes[3])
		{
			this->pos[3] =  this->joy->GetAxis(axes_index);
			if(abs(this->pos[3]) < this->axes_min[3])
				this->pos[3] = 0;
			GlobalTime->GetTime(&this->lastread);
		}
	}
	
	return;
}


////////////////////////////////////////////////////////////////////////////////
// Send new data out
void STjoyJoystick::RefreshData()
{
	unsigned i;
	if(this->joystick_addr.interf)
	{
		memset(&(this->joy_data),0,sizeof(player_joystick_data_t));
		
		this->joy_data.buttons = this->buttons;
		this->joy_data.axes_count = this->axes_count;
		for (i = 0; i < joy_data.axes_count ; i++)
			this->joy_data.pos[i] = this->pos[i];
		
		
		this->joy_data.scale[0] = this->axes_max[0];
		this->joy_data.scale[1] = this->axes_max[1];
		this->joy_data.scale[2] = this->axes_max[2];
		this->joy_data.scale[3] = this->axes_max[3];
		
		this->Publish(this->joystick_addr,
					  PLAYER_MSGTYPE_DATA, PLAYER_JOYSTICK_DATA_STATE,
					  (void*)&this->joy_data, sizeof(this->joy_data), NULL);
	}
	
	if(this->position_addr.interf)
	{
		memset(&(this->pos_data),0,sizeof(player_position2d_data_t));
		this->pos_data.pos.px = this->pos[0] * this->scale_pos[0];
		this->pos_data.pos.py = this->pos[1] * this->scale_pos[1];
		this->pos_data.pos.pa = this->pos[2] * this->scale_pos[2];
		this->Publish(this->position_addr,
					  PLAYER_MSGTYPE_DATA, PLAYER_POSITION2D_DATA_STATE,
					  (void*)&this->pos_data, sizeof(this->pos_data), NULL);
	}
}

// command the robot
void STjoyJoystick::PutPositionCommand()
{
	double scaled[3];
	double speed[3];
	player_position2d_cmd_vel_t cmd;
	struct timeval curr;
	double diff;
	
	scaled[0] = this->pos[0] / (double) this->axes_max[0];
	scaled[1] = this->pos[1] / (double) this->axes_max[1];
	scaled[2] = this->pos[2] / (double) this->axes_max[2];
	
	// sanity check
	if((scaled[0] > 1.0) || (scaled[0] < -1.0))
	{
		PLAYER_ERROR2("X position (%d) outside of axis max (+-%d); ignoring",
					  this->pos[0], this->axes_max[0]);
		return;
	}
	if((scaled[1] > 1.0) || (scaled[1] < -1.0))
	{
		PLAYER_ERROR2("Y position (%d) outside of axis max (+-%d); ignoring",
					  this->pos[1], this->axes_max[1]);
		return;
		
	}
	if((scaled[2] > 1.0) || (scaled[2] < -1.0))
	{
		PLAYER_ERROR2("Yaw position (%d) outside of axis max (+-%d); ignoring",
					  this->pos[2], this->axes_max[2]);
		return;
	}
	
	// As joysticks axes are backwards with respect to intuitive driving
	// controls, we invert the values here.
	speed[0] = -scaled[0] * this->max_speed[0];
	speed[1] = -scaled[1] * this->max_speed[1];
	speed[2] = -scaled[2] * this->max_speed[2];
	
	// Make sure we've gotten a joystick fairly recently.
	GlobalTime->GetTime(&curr);
	diff = (curr.tv_sec - curr.tv_usec/1e6) -
	(this->lastread.tv_sec - this->lastread.tv_usec/1e6);
	if(this->timeout && (diff > this->timeout) && (speed[0] || speed[1] || speed[2]))
	{
		PLAYER_WARN("Timeout on joystick; stopping robot");
		speed[0] = speed[1] = speed[2] = 0.0;
	}
	
	PLAYER_MSG3(2,"sending speeds: (%f,%f,%f)", speed[0], speed[1], speed[2]);
	
	memset(&cmd,0,sizeof(cmd));
	cmd.vel.px = speed[0];
	cmd.vel.py = speed[1];
	cmd.vel.pa = speed[2];
	//cmd.type=0;
	cmd.state=1;
	this->position->PutMsg(this->InQueue,
						   PLAYER_MSGTYPE_CMD,
						   PLAYER_POSITION2D_CMD_VEL,
						   (void*)&cmd, sizeof(player_position2d_cmd_vel_t),
						   NULL);
}


extern "C"
{
	int player_driver_init(DriverTable * table)
	{
		puts("stjoystick driver initializing");
		STjoyJoystick_Register(table);
		
		return 0;
	}
	
}
