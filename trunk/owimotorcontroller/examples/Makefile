UNAME := $(shell uname)

CFLAGS = 	-O2 -g -Wall -fmessage-length=0 -fPIC 

INCLUDES = 	-I ../src

LIBS =		-L../ -lowimotorcontroller  

OBJS =		testarm.o

TARGET =	testarm

ifeq ($(UNAME), Linux)
	LIBUSB = $(shell pkg-config --cflags libusb --print-errors)
#Check to see if pkg-config knew where libusb was stored
ifeq (,$(findstring I,$(LIBUSB)))
$(error "Unable to find libusb")
endif
	INCLUDES += `pkg-config --cflags libusb` 
	LIBS += `pkg-config --libs libusb`
endif
ifeq ($(UNAME), Darwin)
	LIBS += -framework IOKit -framework Carbon
endif


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
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)
	@echo 'Finished Linking: $(TARGET)'

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

