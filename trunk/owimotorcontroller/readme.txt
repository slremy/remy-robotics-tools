Ensure that you have:
1) cmake (cmake.org) installed
	binaries exist for many distributions
	e.g. 'apt-get install cmake'
2) libusb0.1 installed
	binaries exist for many distributions
	for windows, use libusb-win32
3) swig installed
	binaries exist for many distributions
	

To do an out of source build, in the directory where the library was downloaded:

mkdir build
cd build
cmake ..
make

#optional and only recommended for private use
sudo make install

#visual studio users
http://www.cmake.org/cmake/help/runningcmake.html
