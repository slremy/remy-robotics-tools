#install ROS
sudo cp /etc/apt/sources.list  /etc/apt/sources.list.backup
sudo sed -i "/^# deb.*multiverse/ s/^# //" /etc/apt/sources.list 
#instructions for 14.04 (Trusty)
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu trusty main" > /etc/apt/sources.list.d/ros-latest.list'
wget https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -O - | sudo apt-key add -
sudo apt-get -y update
sudo apt-get -y install xvfb x11-apps screen
sudo apt-get -y install gdb
sudo apt-get -y install python-rosdistro python-rosdep python-rosinstall python-rosinstall-generator
sudo apt-get -y install ros-indigo-rosbash
sudo apt-get -y install ros-indigo-mjpeg-server
sudo apt-get -y install ros-indigo-stage-ros
sudo rosdep init
rosdep update
echo "source /opt/ros/indigo/setup.bash" >> ~/.bashrc
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

cd ~/catkin_ws/src
wget https://remy-robotics-tools.googlecode.com/svn/trunk/ros_morse/ros_quadrotor.py

sudo apt-get -y install python3-dev python3-yaml python3-setuptools
sudo apt-get -y install blender

git clone https://github.com/morse-simulator/morse.git
cd morse
mkdir build && cd build
cmake -DBUILD_ROS_SUPPORT=ON ..
sudo make install

sudo rosdep init
rosdep update

git clone git://github.com/ros/rospkg.git
cd rospkg
sudo python3 setup.py install
cd ..

git clone git://github.com/ros-infrastructure/catkin_pkg.git
cd catkin_pkg
sudo python3 setup.py install
cd ..

git clone git://github.com/ros/catkin.git
cd catkin
sudo python3 setup.py install

morse check
