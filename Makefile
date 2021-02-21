all: main

main:
	gcc -g -lm -Wall -lncursesw  main.c -o curses
