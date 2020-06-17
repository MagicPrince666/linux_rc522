CROSS_COMPILE = aarch64-imx8mm-linux-gnu-
CXX = $(CROSS_COMPILE)g++
CC  = $(CROSS_COMPILE)gcc

TARGET	= RC522

DIR		= . ./spi
INC		= -I./include -I./spi
CFLAGS	= -g -Wall
LDFLAGS = -lpthread

OBJPATH	= .

FILES	= $(foreach dir,$(DIR),$(wildcard $(dir)/*.cpp))
CFILES	= $(foreach dir,$(DIR),$(wildcard $(dir)/*.c))

OBJS	= $(patsubst %.cpp,%.o,$(FILES))
COBJS	= $(patsubst %.c,%.o,$(CFILES))

all:$(OBJS) $(COBJS) $(TARGET)

$(OBJS):%.o:%.cpp
	$(CXX) $(CFLAGS) $(INC) -c -o $(OBJPATH)/$(notdir $@) $<

$(COBJS):%.o:%.c
	$(CC) $(CFLAGS) $(INC) -c -o $(OBJPATH)/$(notdir $@) $< 

$(TARGET):$(OBJPATH)
	$(CXX) -o $@ $(OBJPATH)/*.o $(LDFLAGS)

clean:
	-rm $(OBJPATH)/*.o
	-rm $(TARGET)
	