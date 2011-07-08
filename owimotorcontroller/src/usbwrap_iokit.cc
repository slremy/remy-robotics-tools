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
// http://developer.apple.com/technotes/tn2007/tn2187.html
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDDevice.h>


struct usb_list_struct {
	IOHIDDeviceRef ref;
	int pid;
	int vid;
	struct usb_list_struct *next;
};

static struct usb_list_struct *usb_list=NULL;

static IOHIDManagerRef hid_manager=NULL;

void attach_callback(void *context, IOReturn r, void *hid_mgr, IOHIDDeviceRef dev)
{
	CFTypeRef type;
	struct usb_list_struct *n, *p;
	int32_t pid, vid;
	
	if (!dev) return;
	type = IOHIDDeviceGetProperty(dev, CFSTR(kIOHIDVendorIDKey));
	if (!type || CFGetTypeID(type) != CFNumberGetTypeID()) return;
	if (!CFNumberGetValue((CFNumberRef)type, kCFNumberSInt32Type, &vid)) return;
	type = IOHIDDeviceGetProperty(dev, CFSTR(kIOHIDProductIDKey));
	if (!type || CFGetTypeID(type) != CFNumberGetTypeID()) return;
	if (!CFNumberGetValue((CFNumberRef)type, kCFNumberSInt32Type, &pid)) return;
	n = (struct usb_list_struct *)malloc(sizeof(struct usb_list_struct));
	if (!n) return;
	//printf("attach callback: vid=%04X, pid=%04X\n", vid, pid);
	n->ref = dev;
	n->vid = vid;
	n->pid = pid;
	n->next = NULL;
	if (usb_list == NULL) {
		usb_list = n;
	} else {
		for (p = usb_list; p->next; p = p->next) ;
		p->next = n;
	}
}

void detach_callback(void *context, IOReturn r, void *hid_mgr, IOHIDDeviceRef dev)
{
	struct usb_list_struct *p, *tmp, *prev=NULL;
	
	p = usb_list;
	while (p) {
		if (p->ref == dev) {
			if (prev) {
				prev->next = p->next;
			} else {
				usb_list = p->next;
			}
			tmp = p;
			p = p->next;
			free(tmp);
		} else {
			prev = p;
			p = p->next;
		}
	}
}

void init_hid_manager(void)
{
	CFMutableDictionaryRef dict;
	IOReturn ret;
	
	if (hid_manager) return;
	hid_manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
	if (hid_manager == NULL || CFGetTypeID(hid_manager) != IOHIDManagerGetTypeID()) {
		if (hid_manager) CFRelease(hid_manager);
		fprintf(stderr, "no HID Manager - maybe this is a pre-Leopard (10.5) system?\n");
		return;
	}
	dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
									 &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	if (!dict) return;
	IOHIDManagerSetDeviceMatching(hid_manager, dict);
	CFRelease(dict);
	IOHIDManagerScheduleWithRunLoop(hid_manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	IOHIDManagerRegisterDeviceMatchingCallback(hid_manager, attach_callback, NULL);
	IOHIDManagerRegisterDeviceRemovalCallback(hid_manager, detach_callback, NULL);
	ret = IOHIDManagerOpen(hid_manager, kIOHIDOptionsTypeNone);
	if (ret != kIOReturnSuccess) {
		IOHIDManagerUnscheduleFromRunLoop(hid_manager,
										  CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		CFRelease(hid_manager);
		fprintf(stderr, "Error opening HID Manager");
	}
}

static void do_run_loop(void)
{
	while (CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true) == kCFRunLoopRunHandledSource) ;
}

static IOHIDDeviceRef iokit_deviceref = NULL;

int write_msg(char* message, char packetsize){
	IOReturn ret;
	
	if (!iokit_deviceref) return 0;
	ret = IOHIDDeviceSetReport(iokit_deviceref,
							   kIOHIDReportTypeOutput, 0, (uint8_t *)message, packetsize);
	return (ret == kIOReturnSuccess ? 1 : 0);
}

void close_device()
{
	if (!iokit_deviceref) return;
	
	struct usb_list_struct *p;
	
	do_run_loop();
	for (p = usb_list; p; p = p->next) {
		if (p->ref == iokit_deviceref) {
			IOHIDDeviceClose(iokit_deviceref, kIOHIDOptionsTypeNone);
			return;
		}
	}
}

int open_device(int vid, int pid, int armindex){
	
	close_device();
	
	char owiarm_cnt = 0;
	struct usb_list_struct *p;
	IOReturn ret = kIOReturnError;
	
	init_hid_manager();
	do_run_loop();
	for (p = usb_list; p; p = p->next) {
		printf("found %d %d\n", p->vid, p->pid);
		if (p->vid == vid && p->pid == pid) {
			if (owiarm_cnt == (char)armindex){
				ret = IOHIDDeviceOpen(p->ref, kIOHIDOptionsTypeNone);
				if (ret == kIOReturnSuccess) iokit_deviceref = p->ref;

				break;
			}
			owiarm_cnt++;			
		}
	}
	return (ret == kIOReturnSuccess ? 0 : 1);
}


