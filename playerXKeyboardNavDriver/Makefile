ifndef AUTOPYSRC
$(error "Need location of autopy!  make AUTOPYSRC=/somelocation/ondisk/src")
endif

UNAME := $(shell uname)

CFLAGS = 	-O2 -g -Wall -fmessage-length=0 -fPIC 

INCLUDES = 	`pkg-config --cflags playercore` -I $(AUTOPYSRC) 

LIBS =		`pkg-config --libs playercore` 

OBJS =		src/XKeyboardNavDriver.o $(AUTOPYSRC)/keypress.o $(AUTOPYSRC)/deadbeef_rand.o $(AUTOPYSRC)/keycode.o

ifeq ($(UNAME), Linux)
        INCLUDES += `pkg-config --cflags x11`
        LIBS += `pkg-config --libs x11 xtst`
        OBJS += $(AUTOPYSRC)/xdisplay.o
endif

ifeq ($(UNAME), Darwin)
        LIBS += -framework ApplicationServices -framework Carbon
endif

TARGET =	xkeyboardnavdriver.so

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

