#!/bin/bash

# Export build variables
echo "Exporting variables..."
export PREFIX=$1
echo "Prefix: $PREFIX"
export TARGET=$2
echo "Target: $TARGET"
export PATH="$PREFIX/bin:$PATH"

# Download packages to current directory

echo "Downloading GCC..."
wget "http://mirrors-usa.go-parts.com/gcc/releases/gcc-7.1.0/gcc-7.1.0.tar.gz"
if [ ! "$?" = 0]; then
	echo "Failed to download GCC"
	exit
fi
	

echo "Downloading Binutils..."
wget "http://ftp.gnu.org/gnu/binutils/binutils-2.28.tar.gz"
if [ ! "$?" = 0]; then
	echo "Failed to download Bintuils"
	exit
fi

echo "Packages downloaded"

# Build binutils

tar -xvf binutils-2.28.tar.gz
if [ ! "$?" = 0]; then
	echo "Failed to untar Bintuils"
	exit
fi

echo "Configuring binutils..."

mkdir binutils_build && cd binutils_build
../binutils-2.28/configure --target=$TARGET --prefix="$PREFIX" \ 
                           --with-sysroot --disable-nls --disable-werror
						   
if [ ! "$?" = 0]; then
	echo "Failed binutils config"
	exit
fi

echo "Building binutils"

make
if [ ! "$?" = 0]; then
	echo "Failed to make bintuils"
	exit
fi

echo "Installing binutils"

make install
if [ ! "$?" = 0]; then
	echo "Failed to install bintuils"
	exit
fi

echo "Binutils complete"

#build GCC

tar -xvf gcc-7.1.0.tar.gz

echo "Building GCC"
which -- $TARGET-as || echo $TARGET-as is not in the PATH

mkdir gcc_build && cd gcc_build
../gcc-7.1.0/configure --target=$TARGET --prefix="$PREFIX"    \ 
                       --disable-nls --enable-languages=c,c++ \
					   --without-headers

if [ ! "$?" = 0]; then
	echo "Failed to configure gcc"
	exit
fi

make all-gcc
if [ ! "$?" = 0]; then
	echo "Failed to make gcc-all"
	exit
fi
make all-target-libgcc
if [ ! "$?" = 0]; then
	echo "Failed to make gcc-target-libgcc"
	exit
fi
make install-gcc
if [ ! "$?" = 0]; then
	echo "Failed to install gcc"
	exit
fi
make install-target-libgcc
if [ ! "$?" = 0]; then
	echo "Failed to install gcc"
	exit
fi

# clean up

echo "Build is complete"

echo "Cleaing up..."

rm gcc-7.1.0.tar.gz binutils-2.28.tar.gz


