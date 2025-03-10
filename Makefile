all: ./xs

./xs: ./src/main.c
	mkdir -p ./bin/
	$(CC) -std=c2x -Wall -Wextra -pedantic -o ./bin/xs -s ./src/main.c -I./vendor

clean: 
	rm -rf ./xs
