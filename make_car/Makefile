FLAGS = -Wall -pthread
INCLUDES = ./include

all: make_car

clean:
	rm -f make_cake make_car *~
	
make_car: make_car.c
	gcc -I $(INCLUDES) -o make_car make_car.c $(FLAGS)