/*
         Copyright (c) 2013 Sekou L. Remy
 
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
        These are helper functions written to make the process of talking to a remote robot
        easier to read. The robot can move, turn or stop.
        It makes use of the cURL library (hence include curl.h)

        The velocity is in meters, and the angular velocity is in degrees per second.
*/

#ifndef ROBOT_H
#define ROBOT_H

void setHost(char temp_hostname[]);
void setPermission(char permissions[]);
void moveForward(double velocity);
void moveAcross(double velocity);
void turn(double angularVelocity);
void stop();

typedef struct {
	char *contents;
	int size;
} data;


void sendToRobot(char httpDataRequest[]);
data getFromRobot(char httpDataRequest[]);
size_t write_data( void *ptr, size_t size, size_t nmeb, void *stream);

#endif ROBOT_H
