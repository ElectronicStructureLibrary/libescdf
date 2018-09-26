HDF5_DIR=/opt/hdf5_1.10.1-intel

default: all

groups_example.o: groups_example.c 
	mpicc -g -c -I$(HDF5_DIR)/include -Wall groups_example.c -o groups_example.o

groups_read_example.o: groups_read_example.c 
	mpicc -g -c -I$(HDF5_DIR)/include -Wall groups_read_example.c -o groups_read_example.o

groups_example: groups_example.o ../src/libescdf.la
	mpicc -Wall -g -o groups_example groups_example.o -L../src/.libs/ -lescdf \
	../src/escdf_lookuptable.o  

groups_read_example: groups_read_example.o ../src/libescdf.la
	mpicc -Wall -g -o groups_read_example groups_read_example.o -L../src/.libs/ -lescdf \
	../src/escdf_lookuptable.o  

all: groups_example groups_read_example

clean:
	rm *.o groups_example groups_read_example

