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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "robot.h"

#define _PI 3.141592654

char _hostname[64] = "http://127.0.0.1:8080/";
char _permissions[64] = "&id=0";

/*set the base url for communication with the robot*/
void setHost(char temp_hostname[])
{
	strcpy(_hostname, temp_hostname);
	return;
}

/*set the permissions for communication with the robot*/
void setPermission(char permissions[])
{
	strcpy(_permissions, permissions);
	return;
}

/*set the x-component of the linear velocity as requested*/
void moveForward(double velocity)
{
	char twist[1024];
	sprintf(twist,"twist?id=2&lx=%f",velocity);
	sendToRobot(twist);
	return;
}

/*set the y-component of the linear velocity as requested*/
void moveAcross(double velocity)
{
	char twist[1024];
	sprintf(twist,"twist?id=2&ly=%f",velocity);
	sendToRobot(twist);
	return;
}

/*set the angular velocity as requested*/
void turn(double angle)
{
	double radians = angle/180*_PI;
	char twist[1024];
	sprintf(twist,"twist?id=2&az=%f",radians);
	sendToRobot(twist);	
	return;
}

/*set all velocities to zero*/
void stop(void)
{
	char twist[1024];
	sprintf(twist,"twist?id=2&lx=0&ly=0&lz=0&ax=0&ay=0&az=0");
	sendToRobot(twist);
	return;
}

/* send the data to the robot on the required URL*/
void sendToRobot(char httpDataRequest[])
{
	char url[1024];
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	
	if(curl) {
		sprintf(url,"%s%s%s", _hostname, httpDataRequest, _permissions);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		res = curl_easy_perform(curl);
//		printf("send: %s\n",url); 
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	else
	{
		printf("Couldn't send message properly;\n\n");
	}
	return;
}

/* send the data to the robot on the required URL*/
data getFromRobot(char httpDataRequest[])
{
	char url[1024];
	CURL *curl;
	curl = curl_easy_init();
	
	data currentInfo;
	currentInfo.contents = (char *)malloc(1);
	currentInfo.size = 1;

	if(curl) {
		sprintf(url,"%s%s%s", _hostname, httpDataRequest, _permissions);
 		
		curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, url);
		
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, write_data); /*Setting up the function meant to copy data*/
		curl_easy_setopt(curl,CURLOPT_WRITEDATA, &currentInfo); /*The data pointer to copy the data*/
		curl_easy_perform(curl);
		
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	else
	{
		printf("Couldn't send message properly;\n\n");
	}
	return currentInfo;
}

/*Curl uses this function to write the contents of a webpage to a file/stdout*/
size_t write_data( void *ptr, size_t size, size_t nmeb, void *stream)
{
	data *curl_output = (data *)stream;
	int curl_output_size = size * nmeb;
	
	curl_output->contents = (char *) realloc(curl_output->contents, curl_output->size + curl_output_size + 1);
	if (curl_output->contents) {
		memcpy(curl_output->contents, ptr, curl_output_size); /*Copying the contents*/
		curl_output->size += curl_output_size;
		curl_output->contents[curl_output->size] = 0;
	}
	return curl_output_size;
}
