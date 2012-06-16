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

#include "ros/ros.h"
#include <sensor_msgs/Joy.h>

#include <STJoystick.h>

#define STJOY_AXIS_MAX 127

///\brief Opens, reads from and publishes joystick events
class Joystick
{
private:
	ros::NodeHandle nh_;
	bool open_;               
	std::string joy_dev_;
	double deadzone_;
	double autorepeat_rate_;  // in Hz.  0 for no repeat.
	double coalesce_interval_; // Defaults to 100 Hz rate limit.
	int event_count_;
	int pub_count_;
	ros::Publisher pub_;
public:
	Joystick() : nh_()
	{}
	
	///\brief Opens joystick port, reads from port and publishes while node is active
	int main(int argc, char **argv)
	{
		
		// Parameters
		ros::NodeHandle nh_param("~");
		pub_ = nh_.advertise<sensor_msgs::Joy>("joy", 1);
		nh_param.param<std::string>("dev", joy_dev_, "/dev/input/js0");
		nh_param.param<double>("deadzone", deadzone_, 0.05);
		nh_param.param<double>("autorepeat_rate", autorepeat_rate_, 0); //?
		nh_param.param<double>("coalesce_interval", coalesce_interval_, 0.001); //?
		
		// Checks on parameters
		if (autorepeat_rate_ > 1 / coalesce_interval_)
			ROS_WARN("joy_node: autorepeat_rate (%f Hz) > 1/coalesce_interval (%f Hz) does not make sense. Timing behavior is not well defined.", autorepeat_rate_, 1/coalesce_interval_);
		
		if (deadzone_ >= 1)
		{
			ROS_WARN("joy_node: deadzone greater than 1 was requested. The semantics of deadzone have changed. It is now related to the range [-1:1] instead of [-32767:32767]. For now I am dividing your deadzone by 32767, but this behavior is deprecated so you need to update your launch file.");
			deadzone_ /= 32767;
		}
		
		if (deadzone_ > 0.9)
		{
			ROS_WARN("joy_node: deadzone (%f) greater than 0.9, setting it to 0.9", deadzone_);
			deadzone_ = 0.9;
		}
		
		if (deadzone_ < 0)
		{
			ROS_WARN("joy_node: deadzone_ (%f) less than 0, setting to 0.", deadzone_);
			deadzone_ = 0;
		}
		
		if (autorepeat_rate_ < 0)
		{
			ROS_WARN("joy_node: autorepeat_rate (%f) less than 0, setting to 0.", autorepeat_rate_);
			autorepeat_rate_ = 0;
		}
		
		if (coalesce_interval_ < 0)
		{
			ROS_WARN("joy_node: coalesce_interval (%f) less than 0, setting to 0.", coalesce_interval_);
			coalesce_interval_ = 0;
		}
		
		// Parameter conversions
		double autorepeat_interval = 1 / autorepeat_rate_;
		double scale = -1. / (1. - deadzone_) / 32767.;
		double unscaled_deadzone = 32767. * deadzone_;
		
		struct timeval tv;
		fd_set set;
		STJoystick* joy_fd;
		event_count_ = 0;
		pub_count_ = 0;
		
		// Big while loop opens, publishes
		while (nh_.ok())
		{                                      
			open_ = false;
			bool first_fault = true;
			while (STJoystick::NumJoysticks() < 1)
			{
				ros::spinOnce();
				if (first_fault)
				{
					ROS_ERROR("Couldn't open joystick %s. Will retry every second.", joy_dev_.c_str());
					first_fault = false;
				}
				sleep(1.0);
			}
			
			joy_fd = STJoystick::OpenJoystick(joy_dev_.c_str());
			if (joy_fd != NULL)
			{
				ROS_INFO("Opened joystick: %s. deadzone_: %f.", joy_dev_.c_str(), deadzone_);
				open_ = true;
				
				bool tv_set = false;
				bool publication_pending = false;
				tv.tv_sec = 1;
				tv.tv_usec = 0;
				sensor_msgs::Joy joy_msg; // Here because we want to reset it on device close.
				while (nh_.ok()) 
				{
					ros::spinOnce();
					
					bool publish_now = false;
					bool publish_soon = false;

					// get the next event from the joystick
					if (joy_fd == NULL)
					{
						//ROS_INFO("Read data...");
						joy_msg.header.stamp = ros::Time().now();
						event_count_++;

						if(joy_fd->NumButtons() >= joy_msg.buttons.size())
						{
							int old_size = joy_msg.buttons.size();
							joy_msg.buttons.resize(joy_fd->NumButtons());
							for(unsigned int i=old_size;i<joy_msg.buttons.size();i++)
								joy_msg.buttons[i] = 0.0;
						}

						for (int buttons_index = 0 ; buttons_index <  joy_fd->NumButtons() ; buttons_index++)
						{
							joy_msg.buttons[buttons_index] |= (1 <<  joy_fd->GetButton(buttons_index));
						}
												
						if(joy_fd->NumAxes() >= joy_msg.axes.size())
						{
							int old_size = joy_msg.axes.size();
							joy_msg.axes.resize(joy_fd->NumAxes());
							for(unsigned int i=old_size;i<joy_msg.axes.size();i++)
								joy_msg.axes[i] = 0.0;
						}
						
						for (int axes_index = 0 ; axes_index <  joy_fd->NumAxes() ; axes_index++)
						{
							double val = joy_fd->GetAxis(axes_index)/STJOY_AXIS_MAX*32767.;
							// Allows deadzone to be "smooth"
							if (val > unscaled_deadzone)
								val -= unscaled_deadzone;
							else if (val < -unscaled_deadzone)
								val += unscaled_deadzone;
							else
								val = 0;
							
							joy_msg.axes[axes_index] = val * scale;
						}
						
						publish_now = true;						
						
					}
					
					if (publish_now)
					{
						// Assume that all the JS_EVENT_INIT messages have arrived already.
						// This should be the case as the kernel sends them along as soon as
						// the device opens.
						//ROS_INFO("Publish...");
						pub_.publish(joy_msg);
						publish_now = false;
						tv_set = false;
						publication_pending = false;
						publish_soon = false;
						pub_count_++;
					}
					
					// If an axis event occurred, start a timer to combine with other
					// events.
					if (!publication_pending && publish_soon)
					{
						tv.tv_sec = trunc(coalesce_interval_);
						tv.tv_usec = (coalesce_interval_ - tv.tv_sec) * 1e6;
						publication_pending = true;
						tv_set = true;
						//ROS_INFO("Pub pending...");
					}
					
					// If nothing is going on, start a timer to do autorepeat.
					if (!tv_set && autorepeat_rate_ > 0)
					{
						tv.tv_sec = trunc(autorepeat_interval);
						tv.tv_usec = (autorepeat_interval - tv.tv_sec) * 1e6; 
						tv_set = true;
						//ROS_INFO("Autorepeat pending... %i %i", tv.tv_sec, tv.tv_usec);
					}
					
					if (!tv_set)
					{
						tv.tv_sec = 1;
						tv.tv_usec = 0;
					}
					
				} // End of joystick open loop.
				
				// Close the joystick
				joy_fd->Close();
				ros::spinOnce();
			}

			if (nh_.ok())
				ROS_ERROR("Connection to joystick device lost unexpectedly. Will reopen.");
		}
		
		ROS_INFO("joy_node shut down.");
		
		return 0;
	}
};

int main(int argc, char **argv)
{
	ros::init(argc, argv, "joy_node");
	Joystick j;
	return j.main(argc, argv);
}

