#install ROS
sudo sed -i "/^# deb.*multiverse/ s/^# //" /etc/apt/sources.list 
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu precise main" > /etc/apt/sources.list.d/ros-latest.list'
wget http://packages.ros.org/ros.key -O - | sudo apt-key add -
sudo apt-get -y update
sudo apt-get -y install  ros-groovy-stage
sudo apt-get -y install python-rosinstall
sudo apt-get -y install xvfb
sudo apt-get -y install gdb

mkdir ros-stacks
echo "source /opt/ros/groovy/setup.bash" >> ~/.bashrc
echo "export ROS_PACKAGE_PATH=/home/$USER/ros-stacks:\$ROS_PACKAGE_PATH" >> ~/.bashrc

#should include the catkin workspace here soon!

#install gazebo
sudo apt-get -y install build-essential libtinyxml-dev libtbb-dev libxml2-dev libqt4-dev pkg-config  libprotoc-dev libfreeimage-dev libprotobuf-dev protobuf-compiler libboost-all-dev freeglut3-dev cmake libogre-dev libtar-dev libcurl4-openssl-dev libcegui-mk2-dev
hg clone https://bitbucket.org/osrf/gazebo gazebo
cd gazebo
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/home/$USER/local ../
make -j4
make install
cd ~
echo "export LD_LIBRARY_PATH=/home/$USER/local/lib:\$LD_LIBRARY_PATH" >> ~/.bashrc
echo "export PATH=/home/$USER/local/bin:\$PATH" >> ~/.bashrc
echo "export PKG_CONFIG_PATH=/home/$USER/local/lib/pkgconfig:\$PKG_CONFIG_PATH" >> ~/.bashrc
echo "source /home/$USER/local/share/gazebo/setup.sh" >> ~/.bashrc
source ~/.bashrc

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

