all:
	g++ -I src/include -L src/lib -o main main.cpp Game.cpp CustomClasses.cpp Physic2D.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image