import sys
sys.path.append('../interfaces/python')
import owiswig
a=owiswig.usbowiarm(0);
a.test();
