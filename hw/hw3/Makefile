CC=gcc
CFLAGS=-g -Wall
LFLAGS=-lm -pthread
OBJS1=hw3.o wrapper.o timeft.o
OBJS2=hwdiffd.o wrapper.o
TARGET1=hw3
TARGET2=hwdiffd

$(TARGET1): $(OBJS1)
	$(CC) -o $@ $(OBJS1) $(LFLAGS)
$(TARGET2): $(OBJS2)
	$(CC) -o $@ $(OBJS2) $(LFLAGS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET1).o: wrapper.h timeft.h
hwdiffd.o: wrapper.h
wrapper.o: wrapper.h
timeft.o: timeft.h

clean:
	rm $(TARGET1) $(TARGET2) *.o

test: $(TARGET1) $(TARGET2)
	bash test.sh $(ARG)
