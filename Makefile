CFLAGS= -Wall -g  
SDL= `sdl-config --libs --cflags`

all: animation.c audio.o
	gcc -o animation animation.c audio.o $(SDL)

clean:
	rm -f *~