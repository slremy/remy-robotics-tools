#install ROS
sudo sed -i "/^# deb.*multiverse/ s/^# //" /etc/apt/sources.list 
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu precise main" > /etc/apt/sources.list.d/ros-latest.list'
wget http://packages.ros.org/ros.key -O - | sudo apt-key add -
sudo apt-get -y update
sudo apt-get -y install ros-hydro-desktop
sudo apt-get -y install ros-hydro-roslaunch    #may not be needed since ros-hydro-desktop installed
sudo apt-get -y install ros-hydro-stage
sudo apt-get -y install ros-hydro-image-view
sudo apt-get -y install ros-hydro-rxtools
sudo apt-get -y install python-rosinstall
sudo apt-get -y install xvfb
sudo apt-get -y install gdb

sudo rosdep init
rosdep update

mkdir ros-stacks
echo "source /opt/ros/hydro/setup.bash" >> ~/.bashrc
echo "export ROS_PACKAGE_PATH=/home/$USER/ros-stacks:\$ROS_PACKAGE_PATH" >> ~/.bashrc

source ~/.bashrc

#Create catkin workspace (untested)
# http://www.ros.org/wiki/catkin/Tutorials/create_a_workspace
mkdir ~/ros-stacks/catkin_ws
mkdir ~/ros-stacks/catkin_ws/src
cd ~/ros-stacks/catkin_ws/src
catkin_init_workspace
cd ~

##install gazebo
#sudo apt-get -y install build-essential libtinyxml-dev libtbb-dev libxml2-dev libqt4-dev pkg-config  libprotoc-dev libfreeimage-dev libprotobuf-dev protobuf-compiler libboost-all-dev freeglut3-dev cmake libogre-dev libtar-dev libcurl4-openssl-dev libcegui-mk2-dev
#hg clone https://bitbucket.org/osrf/gazebo gazebo
#cd gazebo
#mkdir build
#cd build
#cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/home/$USER/local ../
#make
#make install
#cd ~
#echo "export LD_LIBRARY_PATH=/home/$USER/local/lib:\$LD_LIBRARY_PATH" >> ~/.bashrc
#echo "export PATH=/home/$USER/local/bin:\$PATH" >> ~/.bashrc
#echo "export PKG_CONFIG_PATH=/home/$USER/local/lib/pkgconfig:\$PKG_CONFIG_PATH" >> ~/.bashrc
#echo "source /home/$USER/local/share/gazebo/setup.sh" >> ~/.bashrc
#source ~/.bashrc

#install Stage simulator manually
cd ~/ros-stacks/
svn co https://code.ros.org/svn/ros-pkg/stacks/stage/trunk stageros
#could remove this step later and get all of these files, but until then..
cd stageros
wget https://remy-robotics-tools.googlecode.com/svn/trunk/ros_gazebo/complete_stage_patch.patch
patch -p0 -i complete_stage_patch.patch
rosmake stageros
#how to verify installation?
cd ~

#install ROS web service manually
cd ~/ros-stacks/
svn co https://remy-robotics-tools.googlecode.com/svn/trunk/ros_web_service/
rosmake ros_web_service

#install gazebo_simulator (with ROS)
sudo apt-get install -y build-essential libtinyxml-dev libtbb-dev libxml2-dev libqt4-dev pkg-config  libprotoc-dev libfreeimage-dev libprotobuf-dev protobuf-compiler libboost-all-dev freeglut3-dev cmake libogre-dev libtar-dev libcurl4-openssl-dev libcegui-mk2-dev
sudo apt-get install -y ros-groovy-bullet ros-groovy-xacro ros-groovy-dynamic-reconfigure ros-groovy-urdf ros-groovy-ivcon ros-groovy-driver-base ros-groovy-pcl-ros
cd ~/ros-stacks/
hg clone https://bitbucket.org/osrf/simulator_gazebo
rosmake simulator_gazebo

#install mjpeg_server
cd ~/ros-stacks/catkin_ws/src
git clone https://github.com/RobotWebTools/mjpeg_server.git
cd ~/ros-stacks/catkin_ws/
catkin_make
