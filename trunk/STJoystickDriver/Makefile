UNAME := $(shell uname)

STjoystick_src = STjoy

CFLAGS = 	-O2 -g -Wall -fmessage-length=0 -fPIC 

INCLUDES = 	`pkg-config --cflags playercore`

LIBS =		`pkg-config --libs playercore` -L$(STjoystick_src) -l$(STjoystick_src)

ifeq ($(UNAME), Linux)
#	INCLUDES += `pkg-config --cflags x11`
#	LIBS += `pkg-config --libs x11`
endif
ifeq ($(UNAME), Darwin)
	LIBS += -framework Carbon -framework IOKit
endif


OBJS =		src/stjoystick.o 

TARGET =	stjoystick.so

.PHONY: $(STjoystick_src)

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

$(TARGET):	$(OBJS) $(STjoystick_src)
	@echo 'Linking File: $(TARGET)'
	$(CXX) -shared -o $(TARGET) $(OBJS) $(LIBS)
	@echo 'Finished Linking: $(TARGET)'

all:	$(TARGET) 

$(STjoystick_src):
	$(MAKE) -C $@

clean:
	rm -f $(OBJS) $(TARGET)
	cd $(STjoystick_src); $(MAKE) clean

