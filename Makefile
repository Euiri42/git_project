##Makefile for project Aero1. Team 5

.PHONY : clean

RM = rm -f

project : ppend.c
	gcc -o ppend -w ppend.c -lcurses -lpthread -Wall

clean:
	$(RM) *.o
