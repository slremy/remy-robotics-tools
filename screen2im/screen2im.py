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
 
import Image as pilImage
from ctypes import cdll, c_ubyte
import Tkinter
import ImageTk
import sys

if sys.platform=='win32':
    libc=cdll.LoadLibrary("screen2im.dll") 
elif sys.platform.startswith('linux'):
    myclib = cdll.LoadLibrary("screen2im.so")
elif sys.platform.startswith('darwin'):
    myclib = cdll.LoadLibrary("screen2im.dylib")

w,h=600,400
interval = 30 #milliseconds

root = Tkinter.Tk() # A root window for displaying objects

data = (c_ubyte*(w*h*4))()
myclib.InitImage(0,0,w,h);
ret = myclib.ImageSetup()
if ret == 0:
	while 1:
		ret = myclib.CopyScreen(data);
		if ret == 0:
			#got a new image
			i = pilImage.frombuffer('RGBA', (w, h), data, 'raw', 'RGBA', 0, 1)
			root.geometry('%dx%d' % (i.size[0],i.size[1]))
			tkpi = ImageTk.PhotoImage(i) # Convert the Image object into a TkPhoto object
			label_image = Tkinter.Label(root, image=tkpi);
			label_image.place(x=0,y=0,width=i.size[0],height=i.size[1])
			root.after(interval, root.quit)
			root.mainloop(0)