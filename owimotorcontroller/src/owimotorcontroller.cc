/*
Copyright (c) 2010 Vincent Sanders, Sekou Remy

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

#include "owimotorcontroller.h"
#include <stdio.h>
#include <unistd.h>

usbowiarm::usbowiarm(ssize_t armnumber){
	this->handle = NULL;
//init… could fail to init
	libusb_init(NULL);


	libusb_device **list;
	libusb_device *found = NULL;

//get list of usb… could fail to find any usb 
	ssize_t cnt = libusb_get_device_list(NULL, &list);
	ssize_t owiarm_cnt = 0;
	int err = 0;
	if (cnt < 0)
	   fprintf(stderr, "failed to get a valid device list\n");
	else{
		for (ssize_t i = 0; i < cnt; i++) {
		   libusb_device *device = list[i];
//find interesting usb devices
		   if (is_owiarm(device)) {
			if (owiarm_cnt == armnumber){
				found = device;
				break;
			}
			owiarm_cnt++;
		   }
		}
//find target usb… could fail to find
//open target usb… could fail to open
		if (found) {
		   err = libusb_open(found, &(this->handle));
		   if (err)
			fprintf(stderr, "failed to get open the device");

		   libusb_claim_interface(this->handle, 0);
		}else{
			fprintf(stderr, "failed to find device %i",(int)armnumber);
		}
		libusb_free_device_list(list, 1);
	}
//configure initial behavior… can't really fail here.
	//ensure motors and LED are off upon initialization
	halt_motors();
	packetsize = 3;
	number_motors = 5;
}

usbowiarm::~usbowiarm(){
	if (this->handle){
		halt_motors();
		setup_LEDOFF();set_control();
		libusb_release_interface(this->handle, 0);
		libusb_close(this->handle);
		libusb_exit(NULL);
		//delete this->handle;
	}
}

bool usbowiarm::is_owiarm(libusb_device *device)
{
	struct libusb_device_descriptor descriptor;
	if (libusb_get_device_descriptor(device, &descriptor) < 0) {
		fprintf(stderr, "failed to get device descriptor");
		return false;
	}
	if (( descriptor.idProduct== 0x0 ) && (descriptor.idVendor== 0x1267 ))
		return true;
	return false;
}


/** stop the motors from moving */
void usbowiarm::halt_motors()
{
	ctrl = 0;
	set_control();
}

/** actually issue the control to the motor controller */
void usbowiarm::set_control()
{
	uint8_t packet[packetsize];

	packet[0] = ctrl & 0xff;
	packet[1] = (ctrl & 0xff00) >> 8;
	packet[2] = (ctrl & 0xff0000) >> 16;
	if(this->handle)
		libusb_control_transfer(this->handle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, 6, 0x100, 0, packet, 3, 0);
	else
		fprintf(stderr, "Would have liked to send the arm %x\n",ctrl);
}

/** return the last command issued to the motor controller*/
uint32_t usbowiarm::get_control()
{
    return ctrl;
}


void usbowiarm::setup_LEDON()
{
	ctrl |= 0xff<<16;//turn high bytes on
}

void usbowiarm::setup_LEDOFF()
{
	ctrl &= ~(0xff<<16);  //turn high bytes off
}

void usbowiarm::setup_LEDTOGGLE()
{
	ctrl ^= 0xff<<16;//toggle high bytes
}

void usbowiarm::setup_motorforward(char i)
{
	//turn low bit on and high bit off
	if (i < number_motors)
		ctrl |= (1 << (i<<1));		//ctrl |= 1 << (2*i); //might be more readable
}

void usbowiarm::setup_motorreverse(char i)
{
	//turn low bit off and high bit on
	if (i < number_motors)
		ctrl |= (2 << (i<<1));
}

void usbowiarm::setup_motoroff(char i)
{
	//turn both bits off
	if (i < number_motors)
		ctrl &= ~(3 << (i<<1));
}

void usbowiarm::test()
{
	fprintf(stderr, "beginning test\n");
	for (char i = 0; i < number_motors; i++){
		setup_motorreverse(i);
	}
	set_control();
	fprintf(stderr, " the above was the result of all motors reverse\n");
	for (char i = 0; i < number_motors; i++){
		setup_motoroff(i);
		set_control();
		usleep(30000);
	}
	fprintf(stderr, " the above was incrementally each motor turned off\n");
	for (char i = 0; i < number_motors; i++){
		setup_motorforward(i);
		set_control();
		usleep(30000);
		halt_motors();
	}		
	fprintf(stderr, " the above was each motor incrementally turned on\n");
	for (char i = 0; i < number_motors; i++){
		setup_motorreverse(i);
		set_control();
		usleep(30000);
		halt_motors();
	}
	fprintf(stderr, " the above was incrementally each motor turned in reverse\n");
}

