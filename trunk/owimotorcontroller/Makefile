UNAME := $(shell uname)
UNAME=Linux
examples_dir = examples

CFLAGS = 	-O2 -g -Wall -fmessage-length=0 -fPIC 

INCLUDES = 	

LIBS =		  

ifeq ($(UNAME), Linux)
	INCLUDES += `pkg-config --cflags libusb` 
	LIBS += `pkg-config --libs libusb`
endif
ifeq ($(UNAME), Darwin)
	LIBS += -framework IOKit -framework Carbon
endif


OBJS =		src/owimotorcontroller.o src/usbwrap.o

TARGET =	libowimotorcontroller.so

.PHONY: $(examples_dir)

%.o: %.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(CFLAGS) $(INCLUDES) -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: %.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(CFLAGS) $(INCLUDES) -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Compiler'
	gcc $(CFLAGS) $(INCLUDES) -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(TARGET):	$(OBJS)
	@echo 'Linking File: $(TARGET)'
	$(CXX) -shared -o $(TARGET) $(OBJS) $(LIBS)
	@echo 'Finished Linking: $(TARGET)'

all:	$(TARGET) $(examples_dir)

$(examples_dir): $(TARGET)
	$(MAKE) -C $@

clean:
	rm -f $(OBJS) $(TARGET)
	cd $(examples_dir); $(MAKE) clean

