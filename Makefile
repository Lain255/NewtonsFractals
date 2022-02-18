LIBS	:= -lSDL2main -lSDL2
SOURCE	:= NewtonFractals.cpp
FLAGS	:= -O3 -Wall

run:
	g++ $(FLAGS) $(SOURCE) $(LIBS)
	./a.out
	rm a.out
