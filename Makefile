all: ./xs

./xs: ./main.c ./list.h ./table.h
	$(CC) -gfull -std=c11 -Wall --pedantic -o ./xs ./main.c 

run: ./xs
	xs

clean: 
	rm -rf ./xs
