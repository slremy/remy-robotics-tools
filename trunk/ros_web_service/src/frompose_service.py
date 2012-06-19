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
from geometry_msgs.msg import Pose

class pose_converter:
        def __init__(self):
                rospy.init_node('pose_converter', anonymous=True)
                self.data = ""
                self.request_sub = rospy.Subscriber("pose", Pose, self.callback)
                self.data_uri = rospy.get_param("data_uri","/pose");
                self.urls = (self.data_uri,'pose', "/stop","stop")
                rospy.logwarn("running")
        def callback(self,msg):
                #get the data from the message and store as a string
                try:
                        self.data = '['+msg.position.x+','msg.position.y+','msg.position.z+']+['+msg.orientation.x+','+msg.orientation.y+','+msg.orientation.z+','+msg.orientation.w+']';
                except Exception, err:
                        rospy.logwarn("Cannot convert the Pose message due to %s" % err)

class stop:
        def GET(self):
                return exit(0)
        def POST(self):
                return exit(0)
class pose:
        def GET(self):
                return self.process()
        def POST(self):
                return self.process()
        def process(self):
                global pc
                web.header("Content-Length", str(pc.data)) # Set the Header
                #output to browser
                web.header("Content-Type", "text/plain") # Set the Header
                return pc.data

pc = pose_converter()
app = web.application(pc.urls, globals())

if __name__ == "__main__":
        wsgifunc = app.wsgifunc()
        wsgifunc = web.httpserver.StaticMiddleware(wsgifunc)
        server = web.httpserver.WSGIServer(("0.0.0.0", 8080),wsgifunc)
        print "http://%s:%d/%s" % ("0.0.0.0", 8080, pc.urls)
        try:
                server.start()
        except (KeyboardInterrupt, SystemExit):
                server.stop()
                print "Shutting down service"

