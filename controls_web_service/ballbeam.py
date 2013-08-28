'''
 Copyright (c) 2013 Sekou Remy

 Modified AUG 23RD, 2013 by Dara, D.

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

try:
	port = int(argv[1])
except:
	port = 8080;

clock = timeit.default_timer;
 
# system's physical properties
m = 0.111;
R = 0.015;
g = -9.8;
L = 1.0;
d = 0.03;
J = 9.99e-6;
H = -m*g*d/L/(J/R*R+m);
 
# http://www.forkosh.com/mimetex.cgi?P(s)=\frac{-m*g*d/L/(J/R^2+m)}{s^2}
# http://www.forkosh.com/mimetex.cgi?s=\frac{z-1}{zh}
# http://www.forkosh.com/mimetex.cgi?r[n]=2r[n-1]-r[n-2]+Hh^2\theta[n]
 
Dist  = deque([ 0, 0, 0],10); # Ball position
Theta = deque([ 0, 0, 0],10); # Beam angle
U =     deque([ 0, 0, 0]);
h =     .02;
 
# http://www.forkosh.com/mimetex.cgi?P(s)=\frac{\Theta(z)}{V_{in}(z)}=\frac{A_2^2z^2}{B_2^2z^2 + B_1z + B_0}
 
alpha=0.01176
beta=0.58823
# http://www.forkosh.com/mimetex.cgi?P(s)=\frac{\Theta(s)}{V_{in}(s)}=\frac{1}{s(\alpha s+\beta)}
A12=h*h
B12=alpha
B11=(beta*h - 2*alpha)
B10=alpha
P=A12/B12
Q=B11/B12
R=B10/B12
theta_high = 3.14/180.0*(32+20);
 
# 2.2.6
# http://www.forkosh.com/mimetex.cgi?P(s)=\frac{X(s)}{A(s)}=\frac{-7}{s^2}
 
A22=-7*h*h
B22=1
B21=-2
B20=1
L=A22/B22
M=B21/B22
N=B20/B22
 
u = 0; # Voltage
u_time = clock(); # Current time
 
# Turn off webpy's debug mode
web.config.debug = False;
 
# Create URL to class map
urls = (
        '/', 'index',
        '/init','initcontroller',
        '/state','state',
        '/u','controller',
        '/stop','closecontroller'
)
 
# Create webpy application from URL list
app = web.application(urls, globals())
 
# Set webpy template directory
render = web.template.render('.')
 
# Calculate the changes to the system
def calculateControl(signum, _):
        global Theta, Dist, U
 
        t = clock()
 
        U.append(u);
 
        # Calculate beam's angle
        theta =  P * U[-1] - Q * Theta[-1] - R * Theta[-2]
 
        # Constrain the angle
        if theta > theta_high: theta = theta_high
        elif theta < -theta_high: theta = -theta_high
        Theta.append(theta);
 
        # Calculate ball's position
        x =  L * Theta[-1]/16.0 - M * Dist[-1] - N * Dist[-2]; # alpha = theta/16 eqn 2.2.2 EEE490
        Dist.append(x);
 
# Stop the service
class closecontroller:
        def GET(self):
                return exit(0)
        def POST(self):
                return exit(0)
 
class index:
        def GET(self):
                return render.index()
 
class initcontroller:
        def GET(self):
                return self.process()
        def POST(self):
                return self.process()
        def process(self):
                global Dist, Theta, u
 
                qs = web.input() # query string
 
                # Initialize balls position and angle of beam
                Dist = deque([ float((qs.dist).replace(" ", "+")) if hasattr(qs, 'dist') else 0 ] * 3)
                Theta = deque([ float((qs.theta).replace(" ", "+")) if hasattr(qs, 'theta') else 0 ] * 3)
 
                u = 0;
                u_time = ( float(qs.time) if hasattr(qs, 'time') else 0 )
 
                web.header("Content-Type", "text/plain") # Set the Header
 
                return ""
               
# Returns data for the current state of the sytem
class state:
        def GET(self):
                return self.process();
        def POST(self):
                return self.process();
        def process(self):
                web.header("Content-Type", "text/plain") # Set the Header
 
                # Return the most recent ball position and beam angle
                return str(Dist[-1]) + " " + str(Theta[-1]);
 
# Modify the system variables
class controller:
        def GET(self):
                return self.process();
        def POST(self):
                return self.process();
        def process(self):
                global u, u_time
 
                qs = web.input(); # query string
 
                # Set system variables from query string values
                u =     ( float((qs.value).replace(" ", "+")) if hasattr(qs, 'value') else 0 )
                u_time = ( float(qs.time) if hasattr(qs, 'time') else 0 )
 
                web.header("Content-Type", "text/plain") # Set the Header
 
                # Return the most recent ball position and beam angle
                return str(Dist[-1]) + " " + str(Theta[-1]);
 
 
if __name__ == "__main__":
        # Set calculateControl to be called on a timed interval
        signal.signal(signal.SIGALRM, calculateControl)
        signal.setitimer(signal.ITIMER_REAL, h, h)
 
        app.run()