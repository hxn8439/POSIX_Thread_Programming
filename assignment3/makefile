SRC = processWOSwitch.c processWSwitch.c
OBJ = $(addsuffix .o, $(basename $(SRC)))
TARGET = $(basename $(OBJ))
CC = gcc
CCFLAGS = 

all: $(TARGET)

processWOSwitch: processWOSwitch.o functions.o
	${CC} ${CCFLAGS} -o $@ processWOSwitch.o functions.o

processWSwitch: processWSwitch.o functions.o
	${CC} ${CCFLAGS} -o $@ processWSwitch.o functions.o

functions.o: functions.c functions.h
	${CC} ${CCFLAGS} -c $<

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o $(TARGET) 
