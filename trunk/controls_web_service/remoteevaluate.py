
'''    
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
    '''

'''
    This program takes nine parameters as command line arguments:
	the duration of the test, 
	
	the step size, 
	the 3 PID constants for the position of the ball
	the 3 PID constants for the angle of the beam
	
    It produces a "fitness value" (higher is better), and provides this response on stdout.
    The value is derived from the step response error for the closed-loop system.
    
    python evaluatePID.py http://IPADDRESS:PORT/ duration STEPSIZE KPball KIball KDball KPbeam KIbeam KDbeam
    
    '''
import web
import httplib
import socket
import errno
import timeit
from math import cos, sin, pi
from collections import deque
import signal
from sys import exit, exc_info, argv
from time import sleep

import os
localport = os.environ.get('PORT', 8080)

R_ref = .5
w=2*3.14159*1/100.*1;

def sign(val):
	return 1 if val >= 0 else -1
def referencex(t):
#	return R_ref*sign(cos(w*t));
	return R_ref*    (cos(w*t));

def referencey(t):
#	return R_ref*sign(sin(w*t));
	return R_ref*    (sin(w*t));

clientport=0

from pycurlclient import *

u_max=20;
CumulativeError = 0.000000000009
iteration = 0;
Count = 0.

X =		deque([ 0, 0, 0],3);
THETA = deque([ 0, 0, 0],3);
Y =	deque([ 0, 0, 0],3);
PHI = deque([ 0, 0, 0],3);
StateTime = 0;

u_x=0;u_y=0;
mse_x = 0 #Mean squared error in x
mse_y = 0 #Mean squared error in y
mse_z = 0 #Mean squared error in z
tcrash = float('inf') #Time the program crashed. If it didn't crash, this is infinite
crashed = False #If the program crashed

AR= 0;
BR= 0;

AM= 0;
BM= 0;

angle_max = 3.14/180.0*(32+160)

#set up the best clock that can be accessed on this machine
clock = timeit.default_timer;
#get the current time (time the controller was started).
t0 = clock();

def initprocess():
    global CumulativeError, Count, AR,BR,AM,BM,t0;
    AR= KpR;
    BR= KdR/h;

    AM= KpM;
    BM= KdM/h;
    (CumulativeError, Count) = (0.0000000000000001,0)
    t0 = clock()
    mse_x = 0 #Mean squared error in x
    mse_y = 0 #Mean squared error in y
    mse_z = 0 #Mean squared error in z
    tcrash = float('inf') #Time the program crashed. If it didn't crash, this is infinite
    crashed = False #If the program crashed
    process(host, port, "/init?dist0=0&theta0=0&dist1=0&theta1=0", clientport);

def closeprocess():
	#process(host, port, "/stop?", clientport);
	process(host, port, "/init?", clientport);

def catcher(signum, _):
    #Sekou, you or someone, should convert this to a PID controller (11/8/2014)
    global X, THETA, Y, PHI, t, StateTime, u_x, u_y
    global tcrash, crashed, iteration, mse_x, mse_y, CumulativeError, Count

    t1 = clock()-t0
    url = "/z?&value0=%.4f&value1=%.4f&time=%.6f&stime=%.6f&access=8783392" % (u_x,u_y,t1,StateTime);

    response=process(host,port,url,clientport);
    tr = clock() - t0;

    if response != "" :
        try:
                X.appendleft( float(response.split()[0]));
                THETA.appendleft( float(response.split()[2]));
                Y.appendleft( float(response.split()[1]));
                PHI.appendleft( float(response.split()[3]));
                StateTime = float(response.split()[4])
        except:
                print("Did not get a proper value: ",exc_info());

        x_d = referencex(t1);#ref(t1,xkernel ,xamplitude ,xfrequency)
        e_x = x_d - X[0];
        angle_d = AR * (e_x) + BR * (X[0]-X[1]);

        if angle_d > angle_max: angle_d=angle_max;
        elif angle_d < -angle_max: angle_d=-angle_max;
        u_x = AM*(angle_d*16 - THETA[0]) + BM * (THETA[0] - THETA[1])

        y_d = referencey(t1);#ref(t1,ykernel,yamplitude,yfrequency)
        e_y = y_d - Y[0];
        angle_d1 = AR * (e_y) + BR * (Y[0]-Y[1]);

        if angle_d1 > angle_max: angle_d1=angle_max;
        elif angle_d1 < -angle_max: angle_d1=-angle_max;
        u_y = AM*(angle_d1*16 - PHI[0]) + BM * (PHI[0] - PHI[1])

        # Update the time and iteration number
        Count += 1;
        CumulativeError = CumulativeError + abs(e_x) #/(duration/h)
        CumulativeError = CumulativeError + abs(e_y) #/(duration/h)
    else:
        print "Communication timed out! ", clock() - t0
        # Update the time and iteration number
        Count += 1;
        CumulativeError = CumulativeError + 10 #/(duration/h)



web.config.debug = False;
urls = (
		'/mm','remotecontroller',
		'/stop','closecontroller'
		)

app = web.application(urls, globals())
wsgifunc = app.wsgifunc()
wsgifunc = web.httpserver.StaticMiddleware(wsgifunc)
server = web.httpserver.WSGIServer(("0.0.0.0", int(localport)),wsgifunc)
print "http://%s:%s/" % ("0.0.0.0", localport)

class remotecontroller:
    def GET(self):
        return self.process();
    def POST(self):
        return self.process();
    def process(self):
        global h, KpR, KpR, KdR, KpM, KiM, KdM, host, port
        i = web.input();#print i
        value = -.00001929291
        try:
                        if hasattr(i, 'data'):
                                [duration, h, KpR, KiR, KdR, KpM, KiM, KdM]=  [float(vals) for index, vals in enumerate(i.data.split(" ")) if index < 8 ]
                                [host, port]=[str(vals) for index, vals in enumerate(i.data.split()) if index > 7  ]
                                #print "Host: %s:%s/" % (host,port), duration, h, KpR, KiR, KdR, KpM, KiM, KdM
                                #strip off trailing slash and http, if present.
                                host = host.strip('http://').strip('/');
                                signal.setitimer(myitimer, h, h)
                                initprocess();
                                while clock() < (t0 + duration):
                                   pass;
                                signal.setitimer(myitimer, 0, 0)
                                value= (CumulativeError/Count)**-3 if CumulativeError > 0 and Count > 10 else .0001
                                print CumulativeError, Count, value

        except:
                        print exc_info(), i

        f = "%.4f" % (value);
        web.header("Content-Type", "text/plain") # Set the Header
        web.header("Access-Control-Allow-Origin", "*") # Set the Header
        web.header("Access-Control-Allow-Credentials", "true") # Set the Header
        return f


class closecontroller:
    def GET(self):
        return exit(0)
    def POST(self):
        return exit(0)

def stopper():
    server.stop()
    exit(0);

if __name__ == "__main__":
    (mysignal,myitimer)=(signal.SIGALRM,signal.ITIMER_REAL)
    '''
    (mysignal,myitimer)=(signal.SIGPROF,signal.ITIMER_PROF)
    (mysignal,myitimer)=(signal.SIGVTALRM,signal.ITIMER_VIRTUAL)
    '''
    #(endsignal,enditimer)=(signal.SIGVTALRM,signal.ITIMER_VIRTUAL)
    
    #signal.signal(endsignal, stopper)
    #signal.setitimer(enditimer, duration, duration)
        
    signal.signal(mysignal, catcher)                            

    try:
            server.start()
    except (KeyboardInterrupt, SystemExit):
            server.stop()
            print exc_info()[0],"Shutting down service"
                                
    signal.setitimer(myitimer, 0, 0)

    #closeprocess();
