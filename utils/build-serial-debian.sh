#!/usr/bin/env bash

./wipeout.sh
./autogen.sh
CC=gcc HDF5_INCLUDES="-I/usr/include/hdf5/serial" HDF5_LIBS="-L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5" CFLAGS="-O3" ./configure --without-mpi
make -j4
