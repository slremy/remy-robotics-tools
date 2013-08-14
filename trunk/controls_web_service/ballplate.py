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

Dist =	deque([ (0,0), (0,0), (0,0)],10);
Theta = deque([ (0,0), (0,0), (0,0)],10);
U =	deque([ (0,0), (0,0), (0,0)]);
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
r_high = 1.1;


#2.2.6
#http://www.forkosh.com/mimetex.cgi?P(s)=\frac{X(s)}{A(s)}=\frac{-7}{s^2}

A22=-7*h*h
B22=1
B21=-2
B20=1
L=A22/B22
M=B21/B22
N=B20/B22


u = (0,0);
u_time = clock();

web.config.debug = False;
urls = (
		'/', 'index',
		'/init','initcontroller',
		'/state','state',
		'/u','controller',
		'/u3','controller3',
		'/stop','closecontroller'
		)

app = web.application(urls, globals())
render = web.template.render('.')

def calculateControl(signum, _):
	global Theta, Dist, U
	t = clock()
	U.append(u);
	theta0 =  P * U[-1][0] - Q * Theta[-1][0] - R * Theta[-2][0]
	if theta0 > theta_high: theta0 = theta_high
	elif theta0 < -theta_high: theta0 = -theta_high
	
	theta1 =  P * U[-1][1] - Q * Theta[-1][1] - R * Theta[-2][1]
	if theta1 > theta_high: theta1 = theta_high
	elif theta1 < -theta_high: theta1 = -theta_high
	
	Theta.append((theta0,theta1));
	x0 =  L * Theta[-1][0]/16.0 - M * Dist[-1][0] - N * Dist[-2][0]; #alpha = theta/16 eqn 2.2.2 EEE490
	
	if x0 > r_high: x0 = r_high;
	elif x0 < -r_high: x0 = -r_high;
	
	x1 =  L * Theta[-1][1]/16.0 - M * Dist[-1][1] - N * Dist[-2][1]; #alpha = theta/16 eqn 2.2.2 EEE490
	
	if x1 > r_high: x1 = r_high;
	elif x1 < -r_high: x1 = -r_high;
	
	Dist.append((x0,x1));
	#print str(repr(t)) + ","+ str(Dist[-1])+","+ str(Theta[-1])+","+str(U[-1])+","+ str(repr(u_time))+ str(repr(t))+",sekou"

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
        dist0 =     ( float((i.dist0).replace(" ","+")) if hasattr(i, 'dist0') else 0 )
        dist1 =     ( float((i.dist1).replace(" ","+")) if hasattr(i, 'dist1') else 0 )
        Dist=deque([ (dist0,dist1) ]*3);

        theta0 =     ( float((i.dist0).replace(" ","+")) if hasattr(i, 'theta0') else 0 )
        theta1 =     ( float((i.dist1).replace(" ","+")) if hasattr(i, 'theta1') else 0 )
        Theta=deque([ (theta0,theta1) ]*3)
        u = (0,0);
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
        f = str(Dist[-1][0])+" "+str(Dist[-1][1])+" "+ str(Theta[-1][0])+" "+str(Theta[-1][1])+" "+str(clock());
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
        f = str(Dist[-1][0])+" "+ str(Theta[-1][0])+" "+str(clock());
        web.header("Content-Type", "text/plain") # Set the Header
        return f

class controller3:
    def GET(self):
        return self.process();
    def POST(self):
        return self.process();
    def process(self):
        global u, u_time, s_time
        i = web.input();#print i
        u0 =     ( float((i.value0).replace(" ","+")) if hasattr(i, 'value0') else 0 )
        u1 =     ( float((i.value1).replace(" ","+")) if hasattr(i, 'value1') else 0 )
        u = (u0,u1);
        u_time = ( float(i.time) if hasattr(i, 'time') else 0 )
        s_time = ( float(i.stime) if hasattr(i, 'stime') else 0 );
        f = str(Dist[-1][0])+" "+str(Dist[-2][0])+" "+ str(Theta[-1][0])+" "+ str(Theta[-2][0])+" "+str(Dist[-1][1])+" "+str(Dist[-2][1])+" "+ str(Theta[-1][1])+" "+ str(Theta[-2][1])+" "+repr(clock());
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

