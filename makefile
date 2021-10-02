pong: pong.o
	g++ pong.o -lSDL2 -o pong

pong.o: pong.cpp
	g++ -c pong.cpp

clean:
	rm *.o pong