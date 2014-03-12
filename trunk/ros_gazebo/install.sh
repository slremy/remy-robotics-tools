#install ROS
sudo sed -i "/^# deb.*multiverse/ s/^# //" /etc/apt/sources.list 
#instructions for 13.04 (Raring)
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu raring main" > /etc/apt/sources.list.d/ros-latest.list'

wget http://packages.ros.org/ros.key -O - | sudo apt-key add -
sudo apt-get -y update
#install ROS with STAGE and GAZEBO
sudo apt-get -y install ros-hydro-rosbash
sudo apt-get -y install ros-hydro-stage-ros
sudo apt-get -y install ros-hydro-gazebo-ros-control ros-hydro-gazebo-ros-pkgs
sudo apt-get -y install xvfb
sudo apt-get -y install gdb
sudo apt-get -y install python-rosdistro python-rosdep python-rosinstall python-rosinstall-generator
sudo apt-get -y install ros-hydro-mjpeg-server

sudo rosdep init
rosdep update
echo "source /opt/ros/hydro/setup.bash" >> ~/.bashrc

sudo apt-get -y install python-webpy

#create catkin workspace
mkdir -p ~/catkin_ws/src
cd ~/catkin_ws/src
catkin_init_workspace
cd ~/catkin_ws
catkin_make
echo "source ~/catkin_ws/devel/setup.bash" >> ~/.bashrc
source ~/.bashrc

#install ROS web service manually
cd ~/catkin_ws/src/
svn co https://remy-robotics-tools.googlecode.com/svn/trunk/ros_web_service/
cd ~/catkin_ws/
catkin_make
source ~/catkin_ws/devel/setup.bash
