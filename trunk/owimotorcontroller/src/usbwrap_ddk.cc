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
// http://msdn.microsoft.com/en-us/library/ms790932.aspx
#define STRICT /* Require use of exact types. */
#define WIN32_LEAN_AND_MEAN 1 /* Speed up compilation. */
#include <windows.h>
#include <setupapi.h>

extern "C"
{
//#include <ddk/hidsdi.h>
	#include <api/hidsdi.h>
	#include <ddk/hidclass.h>
}
#include <stdlib.h>
#include <malloc.h>

#pragma comment(lib, "SetupApi.lib")
#pragma comment(lib, "hid.lib")

static HANDLE win32_hid_handle = NULL;

int write_usb_device(HANDLE h, void *buf, int len, int timeout)
{
	static HANDLE event = NULL;
	unsigned char tmpbuf[1040];
	OVERLAPPED ov;
	DWORD n, r;

	if (len > sizeof(tmpbuf) - 1) return 0;
	if (event == NULL) {
		event = CreateEvent(NULL, TRUE, TRUE, NULL);
		if (!event) return 0;
	}
	ResetEvent(&event);
	memset(&ov, 0, sizeof(ov));
	ov.hEvent = event;
	tmpbuf[0] = 0;
	memcpy(tmpbuf + 1, buf, len);
	if (!WriteFile(h, tmpbuf, len + 1, NULL, &ov)) {
		if (GetLastError() != ERROR_IO_PENDING) return 0;
		r = WaitForSingleObject(event, timeout);
		if (r == WAIT_TIMEOUT) {
			CancelIo(h);
			return 0;
		}
		if (r != WAIT_OBJECT_0) return 0;
	}
	if (!GetOverlappedResult(h, &ov, &n, FALSE)) return 0;
	if (n <= 0) return 0;
	return 1;
}

int write_msg(char* message, char packetsize){
	int ret;
	if (!win32_hid_handle) return 0;
	ret = write_usb_device(win32_hid_handle, (void *)message, packetsize, (int)(100 * 1000.0));//100 timeout here....
	return ret;
}

void close_device()
{
	if (!win32_hid_handle) return;
	CloseHandle(win32_hid_handle);
	win32_hid_handle = NULL;
}

int open_device(int vid, int pid, int armindex){
	
	close_device();

	char owiarm_cnt = 0;
	GUID guid;
	HDEVINFO info;
	DWORD index, required_size;
	SP_DEVICE_INTERFACE_DATA iface;
	SP_DEVICE_INTERFACE_DETAIL_DATA *details;
	HIDD_ATTRIBUTES attrib;
	BOOL ret;
	
	HidD_GetHidGuid(&guid);
	info = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (info == INVALID_HANDLE_VALUE) return NULL;
	for (index=0; 1 ;index++) {
		iface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		ret = SetupDiEnumDeviceInterfaces(info, NULL, &guid, index, &iface);
		if (!ret) {
			SetupDiDestroyDeviceInfoList(info);
			break;
		}
		SetupDiGetInterfaceDeviceDetail(info, &iface, NULL, 0, &required_size, NULL);
		details = (SP_DEVICE_INTERFACE_DETAIL_DATA *)malloc(required_size);
		if (details == NULL) continue;
		memset(details, 0, required_size);
		details->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		ret = SetupDiGetDeviceInterfaceDetail(info, &iface, details,
											  required_size, NULL, NULL);
		if (!ret) {
			free(details);
			continue;
		}
		win32_hid_handle = CreateFile(details->DevicePath, GENERIC_READ|GENERIC_WRITE,
					   FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
					   FILE_FLAG_OVERLAPPED, NULL);
		free(details);
		if (win32_hid_handle == INVALID_HANDLE_VALUE) continue;
		attrib.Size = sizeof(HIDD_ATTRIBUTES);
		ret = HidD_GetAttributes(win32_hid_handle, &attrib);
		if (!ret) {
			CloseHandle(win32_hid_handle);
			continue;
		}
		if (attrib.VendorID != vid || attrib.ProductID != pid) {
			CloseHandle(win32_hid_handle);
			continue;
		}
		if (owiarm_cnt != (char)armindex) {
			owiarm_cnt++;
			continue;
		}
		SetupDiDestroyDeviceInfoList(info);
		return 0;
	}
	return 1;
	
}
