CC = g++
CFLAGS = -g -std=c++17

datadisplay: datadisplay.o
	$(CC) $(CFLAGS) $^ -o $@

datadisplay.o: datadisplay.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f datadisplay.o datadisplay
	