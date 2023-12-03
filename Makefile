OBJS	= queue.o
SOURCE	= schedule.c
OUT	= schedule
CC	 = gcc
FLAGS	 = -Wall


$(OUT):	$(OBJS) $(SOURCE)
		$(CC) $(FLAGS) $(SOURCE) -o $(OUT) $(OBJS) $(LFLAGS)

queue.o: queue.c
	$(CC) $(FLAGS) -c queue.c -o queue.o

target:
		$(MAKE) -C target

val:
		valgrind ./$(OBJS) $(ARGS)

clean:
	rm -f $(OBJS) $(OUT)

run: $(OUT)
	./$(OUT) $(ARGS)