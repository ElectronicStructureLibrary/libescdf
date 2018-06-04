
default: all

groups_example.o: groups_example.c 
	gcc -g -c -Wall groups_example.c -o groups_example.o

groups_read_example.o: groups_read_example.c 
	gcc -g -c -Wall groups_read_example.c -o groups_read_example.o

groups_example: groups_example.o ../src/libescdf.la
	../libtool --mode=link gcc -Wall -g -o groups_example groups_example.o ../src/libescdf.la \
	../src/escdf_lookuptable.o  

groups_read_example: groups_read_example.o ../src/libescdf.la
	../libtool --mode=link gcc -Wall -g -o groups_read_example groups_read_example.o ../src/libescdf.la \
	../src/escdf_lookuptable.o  

all: groups_example groups_read_example

clean:
	rm *.o groups_example groups_read_example

