all:
	g++ -I src/include -L src/lib -o main main.cpp Game.cpp -lmingw32 -lSDL2main -lSDL2