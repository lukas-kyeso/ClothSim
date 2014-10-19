CC = g++
LPATH = -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib 
CFLAGS=-g -Wall
LIBS=-lGL -lglut -lGLU -ljpeg -lpng16 -lm -lpng
IPATH= -I/usr/X11/include -I/usr/pkg/include -I./include

all: Main
Main : Main.o G308_ImageLoader.o 
	$(CC) -o Main Main.o G308_ImageLoader.o $(LIBS) $(LPATH) $(LDPATH)
Main.o :
	$(CC) -c $(CFLAGS) Main.cpp $(IPATH)  
G308_ImageLoader.o :
	$(CC) -c $(CFLAGS) G308_ImageLoader.cpp $(IPATH)

clean :
	rm -rf *.o
