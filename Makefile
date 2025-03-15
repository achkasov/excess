all: ./xs

./xs: ./main.c ./list.h
	$(CC) -std=c2x -Wall -Wextra -pedantic -o ./xs -s ./main.c -I./vendor

clean: 
	rm -rf ./xs
