.SUFFIXES : .x .o .c .s

#ROOT = /usr/local/arm_linux_4.2/
ROOT = /usr/include/
#LIB = $(ROOT)/lib/gcc/arm-linux/4.2.1
#LIB1 =$(ROOT)/arm-linux/lib
#INCSYS:=$(ROOT)/arm-linux/sys-include
INC :=$(ROOT)/x86_64-linux-gnu/include


CC=gcc -O2 -I$(INC) -I$(INCSYS) -static
WEC_LDFLAGS=-L$(LIB) -L$(LIB1)
STRIP=strip
TARGET = test_baud

SRCS1 := test_baud.c

LIBS= -lc -lgcc -lc

all: 
	$(CC) $(WEC_LDFLAGS) $(SRCS1) -o $(TARGET) $(LIBS)
	$(STRIP) $(TARGET) 

clean:
	rm -f *.o 
	rm -f *.x 
	rm -f *.flat
	rm -f *.map
	rm -f temp
	rm -f *.img
	rm -f $(TARGET)	
	rm -f *.gdb
