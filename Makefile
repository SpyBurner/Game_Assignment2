all:
	g++ -I src/include -L src/lib -o main src/main.cpp src/Game.cpp src/CustomClasses.cpp src/Physic2D.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image