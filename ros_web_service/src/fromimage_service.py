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
import struct
import cStringIO
roslib.load_manifest('sensor_msgs')
import rospy
import web
import Image as pilImage
from sensor_msgs.msg import Image


def imgmsg_to_pil(rosimage, encoding_to_mode = {
        'mono8' :     'L',
        '8UC1' :      'L',
        '8UC3' :      'RGB',
        'rgb8':       'RGB',
        'bgr8':       'RGB',
        'rgba8':      'RGBA',
        'bgra8':      'RGBA',
        'bayer_rggb': 'L',
        'bayer_gbrg': 'L',
        'bayer_grbg': 'L',
        'bayer_bggr': 'L',
        'yuv422':     'YCbCr',
        'yuv411':     'YCbCr'}, PILmode_channels = { 'L' : 1, 'RGB' : 3, 'RGBA' : 4, 'YCbCr' : 3 }):

    '''
    conversion from imgmsg_to_pil modified from posedetectiondb from cmu-ros-pkg
    '''

    conversion = 'B'
    channels = 1
    if rosimage.encoding.find('32FC') >= 0:
        conversion = 'f'
    elif rosimage.encoding.find('64FC') >= 0:
        conversion = 'd'
    elif rosimage.encoding.find('8SC') >= 0:
        conversion = 'b'
    elif rosimage.encoding.find('8UC') >= 0:
        conversion = 'B'
    elif rosimage.encoding.find('16UC') >= 0:
        conversion = 'H'
    elif rosimage.encoding.find('16SC') >= 0:
        conversion = 'h'
    elif rosimage.encoding.find('32UC') >= 0:
        conversion = 'I'
    elif rosimage.encoding.find('32SC') >= 0:
        conversion = 'i'
    else:
        conversion = 'Z'

    channels = PILmode_channels[encoding_to_mode[rosimage.encoding]]  
	#data = struct.unpack( ('>' if rosimage.is_bigendian else '<') + '%d'%(rosimage.width*rosimage.height*channels) + conversion,rosimage.data)
  
    if conversion == 'f' or conversion == 'd':
        dimsizes = [rosimage.height, rosimage.width, channels]
        imagearr = numpy.array(255*I,dtype=numpy.uint8)
        im = pilImage.frombuffer('RGB' if channels == 3 else 'L',dimsizes[1::-1],imagearr.tostring(), 'raw','RGB',0,1)
        if channels == 3:
            im = pilImage.merge('RGB',im.split()[-1::-1])
        return im
    else:
        mode = encoding_to_mode[rosimage.encoding]
        step_size = PILmode_channels[mode]
        dimsizes = [rosimage.height, rosimage.width, step_size]
        im = pilImage.frombuffer(mode,dimsizes[1::-1], rosimage.data,'raw',mode,0,1)
        if mode == 'RGB':
            im = pilImage.merge('RGB',im.split()[-1::-1])
        return im
		

class image_converter:
        def __init__(self):
                rospy.init_node('image_converter', anonymous=True)
                self.request_sub = rospy.Subscriber("image", Image, self.callback)
                self.data = pilImage.Image()
                self.data_uri = rospy.get_param("data_uri","/image");
                self.urls = (self.data_uri,'image', "/stop","stop")
                rospy.logwarn("running")
        def callback(self,msg):
                #get the image from the message and store as a string
                try:
                        self.data = imgmsg_to_pil(msg)
                except Exception, err:
                        rospy.logwarn("Cannot convert the image due to %s" % err)
class stop:
        def GET(self):
                return exit(0)
        def POST(self):
                return exit(0)
class image:
        def GET(self):
                return self.process()
        def POST(self):
                return self.process()
        def process(self):
                global ic
                try:
                        f = cStringIO.StringIO()
                        ic.data.resize((160,120)).save(f, "PNG");
                        f.seek(0, 2)
                        size = f.tell()
                        f.seek(0)
                        data = f.read();
                except Exception, err:
                        print "Cannot load the image due to %s" % err
                        data = '';
                        size = 0
                web.header("Content-Length", str(size)) # Set the Header
                #output to browser
                web.header("Content-Type", "image/png") # Set the Header
                return data

ic = image_converter()
app = web.application(ic.urls, globals())

if __name__ == "__main__":
        wsgifunc = app.wsgifunc()
        wsgifunc = web.httpserver.StaticMiddleware(wsgifunc)
        server = web.httpserver.WSGIServer(("0.0.0.0", 8080),wsgifunc)
        print "http://%s:%d/%s" % ("0.0.0.0", 8080, ic.urls)
        try:
                server.start()
        except (KeyboardInterrupt, SystemExit):
                server.stop()
                print "Shutting down service"

