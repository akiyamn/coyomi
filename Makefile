all: main

main:
	gcc -g -Wall -lncursesw  main.c -o curses
