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



 This code mplements ImageGrab module and provides missing functionality 
 for OSX and Linux. See http://www.pythonware.com/library/pil/handbook/imagegrab.htm

 (uses native libraries so should not suffer from blank images that occur 
  on other implementation)
  
 Usage:
 import screen2im as ImageGrab
 im=ImageGrab.grab((50,50,10,500))
 im.show()
'''


import Image as pilImage
from ctypes import cdll, c_ubyte
import sys
import os
path = os.path.dirname(os.path.realpath(__file__))

if sys.platform=='win32':
    libc=cdll.LoadLibrary(os.path.dirname(os.path.abspath(__file__)) + os.path.sep + "screen2im.dll") 
elif sys.platform.startswith('linux'):
    myclib = cdll.LoadLibrary(os.path.dirname(os.path.abspath(__file__)) + os.path.sep + "screen2im.so")
elif sys.platform.startswith('darwin'):
    myclib = cdll.LoadLibrary(os.path.dirname(os.path.abspath(__file__)) + os.path.sep + "screen2im.dylib")

def grab(bbox=(0,0,400,400)):
    '''Copy the contents of the screen to PIL image memory.
    '''  
    data = (c_ubyte*(bbox[2]*bbox[3]*4))();
    myclib.InitImage(bbox[0],bbox[1],bbox[2],bbox[3]);

    if myclib.ImageSetup() == 0 & myclib.CopyScreen(data) == 0:
		im = pilImage.frombuffer('RGBA', (bbox[2],bbox[3]), data, 'raw', 'RGBA', 0, 1)
    else:
		im = pilImage.Image()
    return im

