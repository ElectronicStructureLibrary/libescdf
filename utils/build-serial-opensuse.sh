#!/usr/bin/env bash

ulimit s unlimited

source /etc/profile.d/lmod.sh
module load gnu/7  hdf5/1.10.1
   # Prepare source tree
./wipeout.sh
./autogen.sh
CC=gcc CFLAGS="-O3" ./configure --without-mpi
make -j4
make check
