all: ./bin/xs

./bin/xs: ./src/main.c
	mkdir -p ./bin/
	gcc -std=c2x -Wall -Wextra -pedantic -o ./bin/xs -s ./src/main.c -I./vendor

clean: 
	rm -rf ./bin/
