CC=gcc
DEPS = streaming_service.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cs240StreamingService: main.o part01.o
	$(CC) -o cs240StreamingService main.o part01.o

.PHONY: clean

clean:
	rm -f cs240StreamingService
	rm -f main.o
	rm -f part01.o
