McFarlandProject2: McFarlandProject2.o
	gcc -o proj2 McFarlandProject2.o
McFarlandProject2.o: McFarlandProject2.c McFarlandProject2.h Hashtable.c Hashtable.h List.h
	gcc -c -g -Werror -Wall Hashtable.c
	gcc -c -g -Werror -Wall McFarlandProject2.c
clean:
	rm -f *.o
	rm -f proj2
