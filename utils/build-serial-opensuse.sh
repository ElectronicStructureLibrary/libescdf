#!/usr/bin/env bash


source /etc/profile.d/lmod.sh
module load gnu/7  
   # Prepare source tree
./wipeout.sh
./autogen.sh
CC=gcc CFLAGS="-O3" ./configure --without-mpi
make -j4
