OBJ_dir = obj
INC_dir = include
OBJECTS = memory.o main.o process.o restaurant.o driver.o client.o
memory.o =
main.o = memory.h
process.o = main.h memory.h
restaurant.o = main.h memory.h
driver.o = main.h memory.h
client.o = main.h memory.h
CC = gcc
CFLAGS = -Wall -I $(INC_dir)

out: $(OBJECTOS)
	$(CC) $(addprefix $(OBJ_dir)/,$(OBJECTOS)) -o bin/out
%.o: src/%.c $($@)
	$(CC) $(CFLAGS) -o $(OBJ_dir)/$@ -c $<
clean:
	rm -f obj/*.o
	rm -f bin/out
