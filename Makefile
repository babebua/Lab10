FLAGS = -Wall -pthread
INCLUDES = ./include

all: make_cake

clean:
	rm -f make_cake make_car *~

make_cake: make_cake.c
	gcc -I $(INCLUDES) -o make_cake make_cake.c $(FLAGS)
	