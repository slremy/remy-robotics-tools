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

usbowiarm::usbowiarm(int armnumber){
	this->handle = NULL;

	struct usb_bus *busses;
	struct usb_device *found = NULL;
	char owiarm_cnt = 0;
	int err = 0;

	usb_init();
	usb_find_busses();
	usb_find_devices();
	busses = usb_get_busses();

	if (busses == NULL)
	   fprintf(stderr, "failed to get a valid device list\n");
	else{

		struct usb_bus *bus;
		for (bus = busses; bus; bus = bus->next) {
			struct usb_device *dev;
			for (dev = bus->devices; dev; dev = dev->next) {
				/* Check if this device is a printer */
				if (is_owiarm(dev)) {
					if (owiarm_cnt == (char)armnumber){
						found = dev;
						break;
					}
					owiarm_cnt++;
				}
			}
		}

		if (found) {
		   this->handle = usb_open(found);
		   if (this->handle == NULL)
			fprintf(stderr, "failed to get open the device");
                   if (usb_set_configuration(this->handle, 1)<0)
			fprintf(stderr, "failed to set device device config to 1");
		   if (usb_claim_interface(this->handle, 0) < 0)
			fprintf(stderr, "failed to claim device");

		}else{
			fprintf(stderr, "failed to find device %i",(int)armnumber);
		}
		//usb_free_device_list(list, 1);
	}
	//ensure motors and LED are off upon initialization
	halt_motors();
	packetsize = 3;
	number_motors = 5;
}

usbowiarm::~usbowiarm(){
	if (this->handle){
		halt_motors();
		setup_LEDOFF();set_control();
		usb_release_interface(this->handle, 0);
		usb_close(this->handle);
	}
}

bool usbowiarm::is_owiarm(struct usb_device *device)
{
	if (( device->descriptor.idProduct== 0x0 ) && (device->descriptor.idVendor== 0x1267 ))
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
		usb_control_msg(this->handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE,6, 0x100, 0, (char *)packet, 3, 0);
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
//	for (char i = 0; i < number_motors; i++){
//		setup_motorreverse(i);
//	}
//	set_control();
//	fprintf(stderr, " the above was the result of all motors reverse\n");
	for (char i = 0; i < number_motors; i++){
		setup_motoroff(number_motors-i-1);
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

