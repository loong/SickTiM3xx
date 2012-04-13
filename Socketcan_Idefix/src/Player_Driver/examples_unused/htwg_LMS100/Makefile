LIBPATH =
INCPATH = libplayercore
INCPATH += -Ilibplayerxdr

CFLAGS = -Dlinux32 -Wall -fpic -g3 -I$(INCPATH) `pkg-config --cflags playercore`
LDFLAGS += -Wl,-rpath,$(LIBPATH) #-L$(LIBPATH)
CC = gcc
OBJ = htwg_LMS100.o
OBJ += htwg_LMS100_cola.o

TARGET = libsicklms100.so

all: $(TARGET)

clean:
	rm -f *.o *.so

$(TARGET): $(OBJ)
	$(CC) $? -shared -o $@ $(LDFLAGS)

%.o: %.cc
	$(CC) $(CFLAGS) -c $? -o $@
install: 
	cp $(TARGET) $(LIBPATH)
	ldconfig

uninstall:
	rm -f $(LIBPATH)/$(TARGET)
