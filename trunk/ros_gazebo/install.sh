#install ROS
sudo sed -i "/^# deb.*multiverse/ s/^# //" /etc/apt/sources.list 
#instructions for 13.04 (Raring)
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu raring main" > /etc/apt/sources.list.d/ros-latest.list'

wget http://packages.ros.org/ros.key -O - | sudo apt-key add -
sudo apt-get -y update
sudo apt-get -y install ros-hydro-desktop
sudo apt-get -y install xvfb
sudo apt-get -y install gdb
sudo apt-get -y install python-rosdistro python-rosdep python-rosinstall python-rosinstall-generator
sudo apt-get -y install ros-hydro-mjpeg-server

#install STAGE
sudo apt-get -y install ros-hydro-stage-ros

echo "source /opt/ros/hydro/setup.bash" >> ~/.bashrc
source ~/.bashrc
sudo apt-get -y install python-webpy

#install ROS web service manually
mkdir ~/ros-stacks
cd ~/ros-stacks/
svn co https://remy-robotics-tools.googlecode.com/svn/trunk/ros_web_service/
#rosmake ros_web_service
##this library isn't catkin ready as yet!

#install gazebo
sudo apt-get -y install ros-hydro-gazebo-ros-control ros-hydro-gazebo-ros-pkgs
sudo rosdep init
rosdep update