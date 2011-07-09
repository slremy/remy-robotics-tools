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
/*
 To actually recognize the arm the HID were avoided and the IOKit libs engaged.
 Insight from http://dgwilson.wordpress.com/2007/02/02/usb-missile-launcher-14e-release/
 as well as http://developer.apple.com/library/mac/#documentation/DeviceDrivers/Conceptual/USBBook/USBDeviceInterfaces/USBDevInterfaces.html#//apple_ref/doc/uid/TP40002645-TPXREF101
 and libusb project.
 */
/* Historically inspired by the modified approach for the LUFA HID Bootloader by Dean Camera
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

#include <mach/mach.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFNumber.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>


static IOUSBDeviceInterface** iokit_deviceinterface = NULL;

int write_msg(char* message, char packetsize){
	IOReturn ret;
	
	IOUSBDevRequest urequest;
	bzero(&urequest, sizeof(urequest));
	
	urequest.bmRequestType =	(0x02 << 5);
	urequest.bRequest =			6;
	urequest.wValue =			0x100;
	urequest.wIndex =			0;
	urequest.wLength =			packetsize;
	urequest.pData =			message;
	
	ret = (*iokit_deviceinterface)->DeviceRequest(iokit_deviceinterface, &urequest);
	return (ret != kIOReturnSuccess) ? -1 : urequest.wLenDone; /* Bytes transfered is stored in the wLenDone field*/
}

void close_device(){
	if (iokit_deviceinterface && *iokit_deviceinterface){
		(*iokit_deviceinterface)->USBDeviceClose(iokit_deviceinterface);
		(*iokit_deviceinterface)->Release(iokit_deviceinterface);
	}
	iokit_deviceinterface = NULL;
}

int open_device(int vid, int pid, int armindex){
	
	close_device();
	
	char owiarm_cnt = 0;
	Boolean found;
	mach_port_t 	masterPort = 0;
	kern_return_t			err;
	CFMutableDictionaryRef 	matchingDictionary = 0;
	CFNumberRef				numberRef;
	io_iterator_t			iterator = 0;
	io_service_t			usbDeviceRef;
	IOCFPlugInInterface				**iodev = NULL;
	SInt32							score;
	
	found = false;
	err = IOMasterPort(MACH_PORT_NULL, &masterPort);				
	if (err)
		printf("Could not create master port, err = %08x\n", err);
	else {
		matchingDictionary = IOServiceMatching(kIOUSBDeviceClassName);	// requires <IOKit/usb/IOUSBLib.h>
		if (!matchingDictionary)
			printf("Could not create matching dictionary\n");
		else {
			numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &vid);
			if (!numberRef)
				printf("Could not create CFNumberRef for vendor\n");
			else {
				
				CFDictionaryAddValue(matchingDictionary, CFSTR(kUSBVendorID), numberRef);
				CFRelease(numberRef);
				numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &pid);
				if (!numberRef) {
					printf("Could not create CFNumberRef for product\n");
				}else{
					
					//based on the pid and vid, generate list of matching devices and place in the iterator
					CFDictionaryAddValue(matchingDictionary, CFSTR(kUSBProductID), numberRef);
					CFRelease(numberRef);
					
					err = IOServiceGetMatchingServices(masterPort, matchingDictionary, &iterator);
					matchingDictionary = 0;			// this was consumed by the above call
					
					//from the list of matching devices, find the right one and set found=>true if so.
					while ((usbDeviceRef = IOIteratorNext(iterator))) {
						if (owiarm_cnt == (char)armindex){
							found = true;
							break;
						}
						owiarm_cnt++;				
					}
					IOObjectRelease(iterator);
					
					//Based on preliminary found signal.  If the device cannot be opened this will be set to false!
					if (found)
					{						
						//Create an intermediate plug-in using the IOCreatePlugInInterfaceForService function
						err = IOCreatePlugInInterfaceForService(usbDeviceRef, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &iodev, &score);
						
						//Release the device object after getting the intermediate plug-in
						IOObjectRelease(usbDeviceRef);
						
						if (err || !iodev){
							printf("Unable to create plugin. ret = %08x, iodev = %p\n", err, iodev);
							found = false;
						}
						else {
							//Create the device interface using the QueryInterface function
							err = (*iodev)->QueryInterface(iodev, 
														   CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), 
														   (LPVOID *) &iokit_deviceinterface);
							//Don’t need the intermediate plug-in after device interface is created
							(*iodev)->Release(iodev);
							
							if (err || !iokit_deviceinterface){
								printf("Unable to create a device interface. ret = %08x\n", err);
								found = false;
							} else {
								err = (*iokit_deviceinterface)->USBDeviceOpen(iokit_deviceinterface);
								if (err){
									printf("Unable to open device. ret = %08x\n", err);
									found = false;
								}else{
									// Double checking that this is really the right device
									UInt16 temppid, tempvid;
									(*iokit_deviceinterface)->GetDeviceProduct(iokit_deviceinterface,&temppid);
									(*iokit_deviceinterface)->GetDeviceVendor(iokit_deviceinterface,&tempvid);
									if ( temppid != pid || tempvid != vid)
										found = false;
									else
										found = true;
									
								}
							}
						}
					}
					else
					{
						printf("Failed to find device %i\n",(int)armindex);
					}
					IOObjectRelease(usbDeviceRef);			// no longer need this reference
				}
			}
		}
		mach_port_deallocate(mach_task_self(), masterPort);
	}
	return (found == true ? 0 : 1);
}

