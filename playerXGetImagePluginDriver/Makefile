UNAME := $(shell uname)

CFLAGS = 	-O2 -g -Wall -fmessage-length=0 -fPIC 

INCLUDES = 	`pkg-config --cflags playercore`

LIBS =		`pkg-config --libs playercore`  

ifeq ($(UNAME), Linux)
	INCLUDES += `pkg-config --cflags x11` 
	LIBS += `pkg-config --libs x11`
endif
ifeq ($(UNAME), Darwin)
	LIBS += -framework ApplicationServices
endif


OBJS =		src/XGetImageDriver.o

TARGET =	xgetimagedriver.so

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
	@echo 'Invoking: GCC C++ Compiler'
	gcc $(CFLAGS) $(INCLUDES) -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(TARGET):	$(OBJS)
	@echo 'Linking File: $(TARGET)'
	$(CXX) -shared -o $(TARGET) $(OBJS) $(LIBS)
	@echo 'Finished Linking: $(TARGET)'

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

