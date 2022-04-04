OBJ_dir = obj
INC_dir = include
OBJECTS = memory.o main.o process.o restaurant.o driver.o client.o
memory.o =
main.o = memory.h process.h
process.o = main.h memory.h restaurant.h driver.h client.h
restaurant.o = main.h memory.h
driver.o = main.h memory.h
client.o = main.h memory.h
CC = gcc
CFLAGS = -g -Wall -I $(INC_dir)
LIBS = -lrt

out: $(OBJECTS)
	$(CC) $(addprefix $(OBJ_dir)/,$(OBJECTS)) -o bin/out $(LIBS)
%.o: src/%.c $($@)
	$(CC) $(CFLAGS) -o $(OBJ_dir)/$@ -c $<
clean:
	rm -f obj/*.o
	rm -f bin/out
