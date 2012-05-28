UNAME := $(shell uname)

CFLAGS = 	-O2 -g -Wall -fmessage-length=0 -fPIC 
INCLUDES = 	
LIBS =		

ifeq ($(UNAME), Linux)
	INCLUDES += `pkg-config --cflags x11` 
	LIBS += `pkg-config --libs x11` -dynamic
	TARGET = screen2im.so
endif

ifeq ($(UNAME), Darwin)
	LIBS += -framework Carbon -dynamiclib
	TARGET = screen2im.dylib
endif

SRC =		screen2im.c

$(TARGET):	$(OBJS)
	@echo 'Creating File: $(TARGET)'
	$(CC) -o $(TARGET) $(SRC) $(LIBS)
	@echo 'Finished Creation: $(TARGET)'

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)


