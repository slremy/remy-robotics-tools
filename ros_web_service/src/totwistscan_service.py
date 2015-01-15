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
roslib.load_manifest('sensor_msgs')
import rospy
import web
import sys
from geometry_msgs.msg import Twist
from sensor_msgs.msg import LaserScan

class twist_converter:
        def __init__(self):
                rospy.init_node('twist_converter', anonymous=True)
                self.num_robots=int(rospy.get_param('~num_robots',1))
                self.publishers = [None]*self.num_robots; 
                self.subscribers = [None]*self.num_robots; 
                if rospy.has_param('~robot_prefix'): #if there is a robot prefix assume that there is actually one or more
                        #full_param_name = rospy.search_param('robot_prefix')
                        #robot_prefix = rospy.get_param(full_param_name)
                        robot_prefix=rospy.get_param('~robot_prefix')
                        for r in range(self.num_robots):
                                self.publishers[r]=rospy.Publisher(robot_prefix+str(r)+'/cmd_vel',Twist,queue_size=10);
                                self.subscribers[r] = rospy.Subscriber(robot_prefix+str(r)+'/base_scan', LaserScan, self.callback, r)
                else: # if no robot prefix, assume that there is only one robot
                        self.publishers[0] = rospy.Publisher('cmd_vel',Twist,queue_size=10);rospy.logwarn("assuming /cmd_vel, number of robots actually"+str(self.num_robots))
                        self.subscribers[0] = rospy.Subscriber("base_scan", LaserScan, self.callback, 0)

                self.data_uri = rospy.get_param("data_uri","/twist");
                self.urls = (self.data_uri,'twist', "/stop","stop")
                self.data = ['-10']*self.num_robots;
                self.port=int(rospy.get_param("~port","8080"));
                #self.data_uri2 = rospy.get_param("data_uri","/pose");
                rospy.logwarn("running")

        def callback(self,msg,id):
                #get the data from the message and store as a string
                try:
                        self.data[id] = " ".join(map(str,msg.ranges))
                except Exception, err:
                        rospy.logwarn("Cannot convert the LaserScan message due to %s for robot %s" % err, id)

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
                msg=Twist();
                robot_id=0;
                i = web.input();
                try:
                        if hasattr(i, "lx"):
                                msg.linear.x = float(i.lx)
                        if hasattr(i, "ly"):
                                msg.linear.y = float(i.ly)
                        if hasattr(i, "lz"):
                                msg.linear.z = float(i.lz)
                        if hasattr(i, "ax"):
                                msg.angular.x = float(i.ax)
                        if hasattr(i, "ay"):
                                msg.angular.y = float(i.ay)
                        if hasattr(i, "az"):
                                msg.angular.z = float(i.az)
                        if hasattr(i, "id"):
                                robot_id = int(i.id)
                        if robot_id < tc.num_robots: tc.publishers[robot_id].publish(msg);
                except Exception, err:
                        rospy.logwarn("Cannot convert/publish due to %s" % err)

                data = tc.data[robot_id];
                size = len(data);
                web.header("Content-Length", str(size)) # Set the Header
                #output to browser
                web.header("Content-Type", "text/plain") # Set the Header
                return data

tc = twist_converter()
app = web.application(tc.urls, globals())

if __name__ == "__main__":
        wsgifunc = app.wsgifunc()
        wsgifunc = web.httpserver.StaticMiddleware(wsgifunc);
        #server = web.httpserver.WSGIServer(("0.0.0.0", 8080),wsgifunc)
        server = web.httpserver.WSGIServer(("0.0.0.0", tc.port),wsgifunc)
        print "http://%s:%d/%s" % ("0.0.0.0", tc.port, tc.urls)
        try:
                server.start()
        except (KeyboardInterrupt, SystemExit):
                server.stop()
                print "Shutting down service"
                msg=Twist();
                msg.linear.z = -0.0049;
                for i in range(tc.num_robots):
                         tc.publishers[i].publish(msg)