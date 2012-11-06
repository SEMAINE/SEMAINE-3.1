#!/bin/sh

## set this to the path of your julius installation directory (which contains the lib/ and include sub-directories):


Pwd=`pwd`;
JuliusPath="$Pwd/thirdparty/inst";




./autogen.sh ;
./autogen.sh ;
mkdir inst
export CXXFLAGS="-O0 -mfpmath=sse -march=native"
export CFLAGS="-O0 -mfpmath=sse -march=native"
export CPPFLAGS="-I$JuliusPath/include -DHAVE_JULIUSLIB -DEXTERNAL_FV -D_DEBUG"
export LDFLAGS="-L$JuliusPath/lib -ljulius -lsent"

./configure --prefix=$Pwd/inst --enable-static --disable-shared &&
make clean &&
make -j8 ; make install &&

echo ""
echo "build successfull. You can now use the inst/bin/SMILExtract binary."
echo ""

