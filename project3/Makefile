McFarlandProject2: McFarlandProject3.o
	gcc -o proj3 McFarlandProject3.o
McFarlandProject2.o: McFarlandProject3.c McFarlandProject3.h IPHashtable.c IPHashtable.h TrafficMatrix.c TrafficMatrix.h McFarlandNetworks.h
	gcc -c -g -Werror -Wall IPHashtable.c
	gcc -c -g -Werror -Wall TrafficMatrix.c
	gcc -c -g -Werror -Wall McFarlandProject3.c
clean:
	rm -f *.o
	rm -f proj3
