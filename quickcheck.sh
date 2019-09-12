#!/bin/bash
#
# Copyright (C) 2015-2017 Yann Pouillon <devops@materialsevolution.es>
#
# This file is part of ESCDF.
#
# ESCDF is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, version 2.1 of the License, or (at your option) any
# later version.
#
# ESCDF is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with ESCDF.  If not, see <http://www.gnu.org/licenses/> or write to the
# Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301  USA.

# Note: this script is temporary and will be removed upon release.

# Stop at first error and echo commands
set -ev

# Check that we are in the correct directory
test -s "configure.ac" -a -s "src/escdf.h" || exit 0

# Init build parameters
export DBGFLAGS="-O0 -g3 -ggdb -Wall -Wextra -fbounds-check -fno-inline"

# Prepare source tree
./wipeout.sh
./autogen.sh

# Check configure script
mkdir tmp-config
cd tmp-config
echo ""
echo "### BASIC ###"
../configure
sleep 3
echo ""
echo "### SERIAL ###"
../configure \
  CC="gcc" CFLAGS="${DBGFLAGS}"
sleep 3
#echo ""
#echo "### MPI(def) ###"
#../configure \
#  CC="mpicc" CFLAGS="${DBGFLAGS}"
#sleep 3
#echo ""
#echo "### MPI(dir) ###"
#../configure \
#  --with-mpi=/usr CFLAGS="${DBGFLAGS}"
#sleep 3
#echo ""
#echo "### MPI(env) ###"
#../configure \
#  MPICC="mpicc" CFLAGS="${DBGFLAGS}"
#sleep 3
#echo ""
#echo "### MPI(wrap) ###"
#../configure \
#  CC="gcc" MPICC="mpicc" \
#  CFLAGS="${DBGFLAGS}"
#sleep 3
#echo ""
#echo "### MPI(yon) ###"
#../configure \
#  --with-mpi CFLAGS="${DBGFLAGS}"
#sleep 3
cd ..

# Check default build
mkdir tmp-minimal
cd tmp-minimal
../configure \
  CC="gcc" CFLAGS="${DBGFLAGS}"
sleep 3
make dist
make
make check
mkdir install-minimal
make install DESTDIR="${PWD}/install-minimal"
ls -lR install-minimal >install-minimal.log
cd ..

# Check parallel build
#mkdir tmp-mpi
#cd tmp-mpi
#../configure \
#  CC="mpicc" CFLAGS="${DBGFLAGS}"
#sleep 3
#make
#make clean && make -j4
#make check
#cd ..

# Make distcheck
mkdir tmp-distcheck
cd tmp-distcheck
../configure \
  CC="gcc" CFLAGS="${DBGFLAGS}"
sleep 3
make distcheck -j4
make distcleancheck

# Clean-up the mess
cd ..
rm -rf tmp-config tmp-minimal tmp-mpi tmp-distcheck
