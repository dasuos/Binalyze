all:
	@gcc -Wall -ggdb -I./include -o binalyze src/*.c -lbfd

.PHONY: clean
clean:
	@rm -f binalyze
