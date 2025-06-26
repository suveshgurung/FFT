CC = gcc
CFLAGS = -Wall -pedantic

all: data_analyzer plotter

data_analyzer: data_analyzer.o
	$(CC) $(CFLAGS) -o data_analyzer data_analyzer.o

plotter: plotter.o
	$(CC) $(CFLAGS) -o plotter plotter.o -lSDL2

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f data_analyzer data_analyzer.o plotter plotter.o
