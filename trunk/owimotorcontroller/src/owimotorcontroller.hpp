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

//this is a speed controlled arm, where the speeds are hopefully constant for each joint, and consistent between calls and generally unknown.
class usbowiarm{

	private: 
	public: 
		usbowiarm(int armnumber){
			::arm_open(armnumber);
		}
		~usbowiarm(){
			::arm_close();
		}
		void halt_motors()				{ return ::halt_motors();}
		void set_control()				{ return ::set_control();}
		int  get_control()				{ return ::get_control();}
		void setup_LEDON()				{ return ::setup_LEDON();}
		void setup_LEDOFF()				{ return ::setup_LEDOFF();}
		void setup_LEDTOGGLE()			{ return ::setup_LEDTOGGLE();}
		void setup_motorforward(char i)	{ return ::setup_motorforward(i);}
		void setup_motorreverse(char i)	{ return ::setup_motorreverse(i);}
		void setup_motoroff(char i)		{ return ::setup_motoroff(i);}
		void test()						{ return ::test();}
};
