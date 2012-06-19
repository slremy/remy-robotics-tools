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

from numpy import fromstring, uint8
import rospy
import roslib
import cv
import cv2
roslib.load_manifest("cv_bridge")
from cv_bridge import CvBridge, CvBridgeError

class converter:
        def __init__(self):
                self.bridge = CvBridge()
        def convert(self,data):
                im_arr=fromstring(data, dtype=uint8)
                cv_image = cv2.imdecode(im_arr,1)
                try:
                        img = cv.fromarray(cv_image);
                        smaller_img = cv.CreateMat(img.rows/2, img.cols/2, img.type)
                        cv.Resize(img, smaller_img)
                        return (self.bridge.cv_to_imgmsg(smaller_img,"bgr8"))
                except CvBridgeError, e:
                        rospy.logerr(e)
                        return None
