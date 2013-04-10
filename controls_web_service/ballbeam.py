'''
 Copyright (c) 2013 Sekou Remy
 
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

import web
import timeit
import signal
from collections import deque
from sys import exit, exc_info, argv
from numpy import dot

#initialize the port on which the web server runs
try:
	port = int(argv[1])
except:
	port = 8080;

clock = timeit.default_timer;

#system's physical properties
m = 0.111;
R = 0.015;
g = -9.8;
L = 1.0;
d = 0.03;
J = 9.99e-6;
H = -m*g*d/L/(J/R*R+m);

#http://www.forkosh.com/mimetex.cgi?P(s)=\frac{-m*g*d/L/(J/R^2+m)}{s^2}
#http://www.forkosh.com/mimetex.cgi?s=\frac{z-1}{zh}
#http://www.forkosh.com/mimetex.cgi?r[n]=2r[n-1]-r[n-2]+Hh^2\theta[n]

Dist =	deque([ 0, 0, 0],10);
Theta = deque([ 0, 0, 0],10);
U =	deque([ 0, 0, 0]);
h =	.02;

#http://www.forkosh.com/mimetex.cgi?P(s)=\frac{\Theta(z)}{V_{in}(z)}=\frac{A_2^2z^2}{B_2^2z^2 + B_1z + B_0}

alpha=0.01176
beta=0.58823
#http://www.forkosh.com/mimetex.cgi?P(s)=\frac{\Theta(s)}{V_{in}(s)}=\frac{1}{s(\alpha s+\beta)}
A12=h*h
B12=alpha
B11=(beta*h - 2*alpha)
B10=alpha
P=A12/B12
Q=B11/B12
R=B10/B12
theta_high = 3.14/180.0*(32+20);

#2.2.6
#http://www.forkosh.com/mimetex.cgi?P(s)=\frac{X(s)}{A(s)}=\frac{-7}{s^2}

A22=-7*h*h
B22=1
B21=-2
B20=1
L=A22/B22
M=B21/B22
N=B20/B22


u = 0;
u_time = clock();

web.config.debug = False;
urls = (
    '/', 'index',
    '/init','initcontroller',
    '/state','state',
    '/u','controller',
    '/stop','closecontroller'
)
app = web.application(urls, globals())
render = web.template.render('.')

def calculateControl(signum, _):
	global Theta, Dist, U
	t = clock()
	U.append(u);
	theta =  P * U[-1] - Q * Theta[-1] - R * Theta[-2]
	if theta > theta_high: theta = theta_high
	elif theta < -theta_high: theta = -theta_high
	
	Theta.append(theta);
	x =  L * Theta[-1]/16.0 - M * Dist[-1] - N * Dist[-2]; #alpha = theta/16 eqn 2.2.2 EEE490
	Dist.append(x);

class closecontroller:
    def GET(self):
        return exit(0)
    def POST(self):
        return exit(0)

class index:
    def GET(self):
        return render.index();

class initcontroller:
    def GET(self):
        return self.process();
    def POST(self):
        return self.process();
    def process(self):
        global Dist,Theta,u
        i = web.input();#print i
        Dist=deque([ float((i.dist).replace(" ","+")) if hasattr(i, 'dist') else 0 ]*3);
        Theta=deque([ float((i.theta).replace(" ","+")) if hasattr(i, 'theta') else 0 ]*3)
        u = 0;
        u_time = ( float(i.time) if hasattr(i, 'time') else 0 )
        f = ""
        web.header("Content-Type", "text/plain") # Set the Header
        return str(f)
		
class state:
    def GET(self):
        return self.process();
    def POST(self):
        return self.process();
    def process(self):
        f = str(Dist[-1])+" "+ str(Theta[-1]);
        web.header("Content-Type", "text/plain") # Set the Header
        return f

class controller:
    def GET(self):
        return self.process();
    def POST(self):
        return self.process();
    def process(self):
        global u, u_time
        i = web.input();#print i
        u =	( float((i.value).replace(" ","+")) if hasattr(i, 'value') else 0 )
        u_time = ( float(i.time) if hasattr(i, 'time') else 0 )
        f = str(Dist[-1])+" "+ str(Theta[-1]);
        web.header("Content-Type", "text/plain") # Set the Header
        return f

if __name__ == "__main__":
	signal.signal(signal.SIGALRM, calculateControl)
	signal.setitimer(signal.ITIMER_REAL, h, h)
	wsgifunc = app.wsgifunc()
	wsgifunc = web.httpserver.StaticMiddleware(wsgifunc)
	server = web.httpserver.WSGIServer(("0.0.0.0", port),wsgifunc)
	print "http://%s:%d/" % ("0.0.0.0", port)
	try:
		server.start()
	except (KeyboardInterrupt, SystemExit):
		server.stop()
		print exc_info()[0]
		print "Shutting down service"