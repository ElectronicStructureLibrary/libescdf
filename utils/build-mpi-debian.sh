   # Prepare source tree
./wipeout.sh
./autogen.sh
CC=mpicc HDF5_INCLUDES="-I/usr/include/hdf5/openmpi" HDF5_LIBS="-L/usr/lib/x86_64-linux-gnu/hdf5/openmpi -lhdf5" CFLAGS="-O3" ./configure 
make -j4
