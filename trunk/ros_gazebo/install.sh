#install ROS (and Stage, and Gazebo)
sudo cp /etc/apt/sources.list  /etc/apt/sources.list.backup
sudo sed -i "/^# deb.*multiverse/ s/^# //" /etc/apt/sources.list 
#instructions for 14.04 (Trusty)
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu trusty main" > /etc/apt/sources.list.d/ros-latest.list'
wget https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -O - | sudo apt-key add -
sudo apt-get -y update
sudo apt-get -y install xvfb screen
sudo apt-get -y install gdb
sudo apt-get -y install python-rosdistro python-rosdep python-rosinstall python-rosinstall-generator
sudo apt-get -y install ros-indigo-rosbash
sudo apt-get -y install ros-indigo-mjpeg-server
sudo apt-get -y install ros-indigo-stage-ros
sudo rosdep init
rosdep update
echo "source /opt/ros/indigo/setup.bash" >> ~/.bashrc
source ~/.bashrc

sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu precise main" > /etc/apt/sources.list.d/gazebo-latest.list'
wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -
sudo apt-get -y install ros-indigo-gazebo-ros-pkgs
echo "source /usr/share/gazebo/setup.sh" >> ~/.bashrc
source ~/.bashrc
  
#install ROS web service manually
#create catkin workspace
mkdir -p ~/catkin_ws/src
cd ~/catkin_ws/
catkin_make
echo "source ~/catkin_ws/devel/setup.bash" >> ~/.bashrc
cd ~/catkin_ws/src/
svn co https://remy-robotics-tools.googlecode.com/svn/trunk/ros_web_service/
cd ~/catkin_ws/
catkin_make
source ~/.bashrc

cd ~/catkin_ws/src/
git clone https://github.com/ros-simulation/stage_ros.git 
cd ~/catkin_ws/
catkin_make
source ~/.bashrc

screen
Xvfb :1 -screen 0 1024x768x24 2>&1 >/dev/null &

sudo apt-get -y install python-webpy
cd ~/catkin_ws/src
wget https://remy-robotics-tools.googlecode.com/svn/trunk/ros_gazebo/stage.launch
wget https://remy-robotics-tools.googlecode.com/svn/trunk/ros_gazebo/willow-erratic.world
