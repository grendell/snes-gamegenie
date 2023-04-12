CC = clang
LD = clang
CC_FLAGS = -O2 -Wall -Wextra -c
LD_FLAGS = -Wall -Wextra
OBJ = obj

gg: $(OBJ) $(OBJ)/gg.o
	$(CC) $(LD_FLAGS) $(OBJ)/gg.o -o gg

$(OBJ):
	mkdir $(OBJ)

$(OBJ)/gg.o: gg.c
	$(CC) $(CC_FLAGS) gg.c -o $(OBJ)/gg.o

.PHONY: clean
clean:
	rm -rf $(OBJ) gg
