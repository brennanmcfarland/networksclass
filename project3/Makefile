McFarlandProject3: McFarlandProject3.o
	gcc -o proj3 McFarlandProject3.o
McFarlandProject3.o: McFarlandProject3.c McFarlandProject3.h IPHashtable.c IPHashtable.h ConnectionHashtable.c ConnectionHashtable.h McFarlandNetworks.h
	gcc -c -g -Werror -Wall IPHashtable.c
	gcc -c -g -Werror -Wall ConnectionHashtable.c
	gcc -c -g -Werror -Wall McFarlandProject3.c
	gcc -c -g -Werror -Wall McFarlandNetworks.c
clean:
	rm -f *.o
	rm -f proj3
