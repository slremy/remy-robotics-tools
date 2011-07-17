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
#include <stdint.h>
#include <time.h>

int ctrl;
char number_motors;
char packetsize;
int connected;


void arm_open(int armnumber){
	connected = 0;
	packetsize = 3;
	number_motors = 5;
	int ret=open_device(0x1267, 0x0, armnumber);
	if (ret)
		fprintf(stderr, "usbowiarm::Failed to open device.\n");
	else
		connected = 1;
	//ensure motors and LED are off upon initialization
	halt_motors();
}

void arm_close(){
	if (connected){
		halt_motors();
		setup_LEDOFF();
		set_control();
		close_device();
	}
}

/** stop the motors from moving */
void   halt_motors()
{
	ctrl = 0;
	set_control();
}

/** actually issue the control to the motor controller */
void   set_control()
{
	char packet[packetsize];

	packet[0] = ctrl & 0xff;
	packet[1] = (ctrl & 0xff00) >> 8;
	packet[2] = (ctrl & 0xff0000) >> 16;
	if(connected)
		write_msg(packet, packetsize);
	else
		fprintf(stderr, "usbowiarm::Would have liked to send the arm %x\n",ctrl);
}

/** return the last command issued to the motor controller*/
int   get_control()
{
    return ctrl;
}


void   setup_LEDON()
{
	ctrl |= 0xff<<16;//turn high bytes on
}

void   setup_LEDOFF()
{
	ctrl &= ~(0xff<<16);  //turn high bytes off
}

void   setup_LEDTOGGLE()
{
	ctrl ^= 0xff<<16;//toggle high bytes
}

void   setup_motorforward(char i)
{
	//turn low bit on and high bit off
	if (i < number_motors)
		ctrl |= (1 << (i<<1));		//ctrl |= 1 << (2*i); //might be more readable
}

void   setup_motorreverse(char i)
{
	//turn low bit off and high bit on
	if (i < number_motors)
		ctrl |= (2 << (i<<1));
}

void   setup_motoroff(char i)
{
	//turn both bits off
	if (i < number_motors)
		ctrl &= ~(3 << (i<<1));
}

void   test()
{
	int millisec = 100; // length of time to sleep, in miliseconds
	struct timespec req = {0};
	req.tv_sec = 1;
	req.tv_nsec = millisec * 1000000L;
	
	
	fprintf(stderr, "usbowiarm::beginning test\n");
	for (char i = 0; i < 10; i++){
		setup_LEDTOGGLE();
		//halt_motors();
		set_control();
		nanosleep(&req, (struct timespec *)NULL);
		fprintf(stderr, "next step in test\n");
	}
	fprintf(stderr, " usbowiarm::the above was the result toggling the light 10 times\n");
	return;
	
	for (char i = 0; i < number_motors; i++){
		setup_motoroff(number_motors-i-1);
		set_control();
		nanosleep(&req, (struct timespec *)NULL);
	}
	fprintf(stderr, " usbowiarm::the above was incrementally each motor turned off\n");
	for (char i = 0; i < number_motors; i++){
		setup_motorforward(i);
		set_control();
		nanosleep(&req, (struct timespec *)NULL);
		halt_motors();
	}		
	fprintf(stderr, " usbowiarm::the above was each motor incrementally turned on\n");
	for (char i = 0; i < number_motors; i++){
		setup_motorreverse(i);
		set_control();
		nanosleep(&req, (struct timespec *)NULL);
		halt_motors();
	}
	fprintf(stderr, " usbowiarm::the above was incrementally each motor turned in reverse\n");
}

