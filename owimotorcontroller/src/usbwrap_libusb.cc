/*
 Copyright (c) 2011, Sekou Remy
 
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

/* Inspired by the modified approach for the LUFA HID Bootloader by Dean Camera
 *           http://www.lufa-lib.org
 *
 *   THIS MODIFIED VERSION IS UNSUPPORTED BY PJRC.
 
 which was based on the following
 
 * Teensy Loader, Command Line Interface
 * Program and Reboot Teensy Board with HalfKay Bootloader
 * http://www.pjrc.com/teensy/loader_cli.html
 * Copyright 2008-2010, PJRC.COM, LLC
 */

#include "usbwrap.h"
#include <usb.h>
#include <stdio.h>

static usb_dev_handle *libusb_handle = NULL;

int write_msg(char* message, char packetsize){
	int ret;
	if(!libusb_handle) return 0;
	ret = usb_control_msg(libusb_handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE, 6, 0x100, 0, message, packetsize, 0);
	return ret;
}

void close_device(){
	if (libusb_handle){
		usb_release_interface(libusb_handle, 0);
		usb_close(libusb_handle);
	}
}

int open_device(int vid, int pid, int armindex){
	
	close_device();
	
	struct usb_bus *busses;
	struct usb_device *found = NULL;
	char owiarm_cnt = 0;
	int err = 1;
	
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
				/* Check if this device is an owi arm */
				if (( dev->descriptor.idProduct== pid ) && (dev->descriptor.idVendor== vid )) {
					if (owiarm_cnt == (char)armindex){
						found = dev;
						break;
					}
					owiarm_cnt++;
				}
			}
			if (found) break;
		}
		
		if (found) {
			libusb_handle = usb_open(found);
			if (libusb_handle == NULL)
				fprintf(stderr, "failed to get open the device");
			else if (usb_set_configuration(libusb_handle, 1) < 0)
				fprintf(stderr, "failed to set device device config to 1");
			else if (usb_claim_interface(libusb_handle, 0) < 0)
				fprintf(stderr, "failed to claim device");
			else
				err = 0;	
		}else{
			fprintf(stderr, "failed to find device %i",(int)armindex);
		}
		//usb_free_device_list(list, 1);
	}
	return err;
}

