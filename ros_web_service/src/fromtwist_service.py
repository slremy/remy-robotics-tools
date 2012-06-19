#!/usr/bin/env python
'''
 Copyright (c) 2012 Sekou Remy
 
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

import roslib
roslib.load_manifest('geometry_msgs')
import rospy
import web
from geometry_msgs.msg import Twist

class twist_converter:
        def __init__(self):
                rospy.init_node('twist_converter', anonymous=True)
                self.data = ""
                self.request_sub = rospy.Subscriber("cmd_vel", Twist, self.callback)
                self.data_uri = rospy.get_param("data_uri","/twist");
                self.urls = (self.data_uri,'twist', "/stop","stop")
                rospy.logwarn("running")
        def callback(self,msg):
                #get the data from the message and store as a string
                try:
                        self.data = '['+msg.linear.x+','msg.linear.y+','msg.linear.z+']+['+msg.angular.x+','+msg.angular.y+','+msg.angular.z+']';
                except Exception, err:
                        rospy.logwarn("Cannot convert the Twist message due to %s" % err)

class stop:
        def GET(self):
                return exit(0)
        def POST(self):
                return exit(0)
class twist:
        def GET(self):
                return self.process()
        def POST(self):
                return self.process()
        def process(self):
                global tc
                web.header("Content-Length", str(tc.data)) # Set the Header
                #output to browser
                web.header("Content-Type", "text/plain") # Set the Header
                return tc.data

tc = twist_converter()
app = web.application(tc.urls, globals())

if __name__ == "__main__":
        wsgifunc = app.wsgifunc()
        wsgifunc = web.httpserver.StaticMiddleware(wsgifunc)
        server = web.httpserver.WSGIServer(("0.0.0.0", 8080),wsgifunc)
        print "http://%s:%d/%s" % ("0.0.0.0", 8080, tc.urls)
        try:
                server.start()
        except (KeyboardInterrupt, SystemExit):
                server.stop()
                print "Shutting down service"

