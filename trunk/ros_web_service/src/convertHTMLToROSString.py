import rospy
import roslib
from std_msgs.msg import String
from HTMLParser import HTMLParser

# create a subclass and override the handler methods
class MyHTMLParser(HTMLParser):
    def handle_starttag(self, tag, attrs):
        print "Encountered a start tag:", tag
    def handle_endtag(self, tag):
        print "Encountered an end tag :", tag
    def handle_data(self, data):
        print "Encountered some data  :", data

class converter:
        def __init__(self):
                ;
        def convert(self,data):
                try:   
                        return String(data.)
                except e:
                        rospy.logerr(e)
                        return None
