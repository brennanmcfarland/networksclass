McFarlandProject2: McFarlandProject2.o
	gcc -o proj2 McFarlandProject2.o
McFarlandProject2.o: McFarlandProject2.c McFarlandProject2.h IPHashtable.c IPHashtable.h TrafficMatrix.c TrafficMatrix.h McFarlandNetworks.h
	gcc -c -g -Werror -Wall IPHashtable.c
	gcc -c -g -Werror -Wall TrafficMatrix.c
	gcc -c -g -Werror -Wall McFarlandProject2.c
clean:
	rm -f *.o
	rm -f proj2
