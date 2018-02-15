source /etc/profile.d/lmod.sh
module load gnu/7 openmpi/3.0.0 phdf5/1.10.1
   # Prepare source tree
./wipeout.sh
./autogen.sh
    # this is a barbarism... shall be replaced with the line under, once hdf5 packages is fixed in opensuse
 CC=mpicc HDF5_INCLUDES="-I$HDF5_DIR/include" HDF5_LIBS="-L$HDF5_DIR/lib64 -lhdf5" ./configure
  # - CC=mpicc CFLAGS="-O3" ./configure 
make -j4
