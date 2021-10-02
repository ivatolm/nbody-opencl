all:
	g++ src/main.cpp src/cl.c -lOpenCL -lsfml-graphics -lsfml-window -lsfml-system
	clear
	./a.out
