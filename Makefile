all:
	@gcc -Wall -ggdb -I./include -o binalyze src/*.c -lbfd -lcapstone

.PHONY: clean
clean:
	@rm -f binalyze
