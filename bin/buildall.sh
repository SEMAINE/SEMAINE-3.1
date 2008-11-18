#!/bin/sh

# SEMAINE C++ buildall unix shell script
# written 11/2008 by Florian Eyben, TUM
#
# this script will build the complete SEMAINE system on Unix systems
# all third party dependencies will be downloaded and built from source
#
# for each package that needs to be compiled a "module" can be created in 
# the buildall/ subdirectory. The module is a shell script. Look at any
# module to see what you need to do. Basically you must call the 
#   register_build 
# function and provide a function with your custom build code
# Additionally you must register your module by adding the corresponding line
# to the buildall/modules.config file
#
# The following variables are available within modules:
#  INSTALL_PREFIX  : prefix where everything gets installed to
#  BUILD_PREFIX    : Prefix where the unpacked build directories are
#
# To build the semaine system just type:  sh buildall.sh
#
# If you get download errors (wget) then you might need to change the 
# download URLs. They are defined in each module's shell script
#

# directory where all the builds will be performed
# everything will be installed in $BASE/inst by default
# this path can be absolute or relative
BASE="../thirdparty"
#######################################################


######### SYSTEM CONFIGURATION ###
WINDOWS="no"        # build on cygwin (NOT SUPPORTED YET)
##################################


# directory where modules config is located and various includes
INCDIR=$PWD/buildall

# create build directory, if it doesn't exist
MYPWD=$PWD
if [ ! -d $BASE ]; then
  mkdir $BASE 
fi
# get absolute path
cd $BASE
BASEDIR=$PWD
cd $MYPWD

##  subdirectories in the build base directory:
DOWNLOAD_PREFIX="$BASEDIR/download"
INSTALL_PREFIX="$BASEDIR/inst"
CONF_PREFIX="$BASEDIR/conf"
BUILD_PREFIX="$BASEDIR/build"
###############################################

if [ ! -d $CONF_PREFIX ]; then
  mkdir $CONF_PREFIX
fi
if [ ! -d $INSTALL_PREFIX ]; then
  mkdir $INSTALL_PREFIX
fi
if [ ! -d $DOWNLOAD_PREFIX ]; then
  mkdir $DOWNLOAD_PREFIX
fi
if [ ! -d $BUILD_PREFIX ]; then
  mkdir $BUILD_PREFIX
fi

# include build helper functions
# this script also loads the modules.config file 
# where the available build modules are configured
. $INCDIR/helper_functions.sh

# do the work:
download_missing 
unpack_missing
for i in `seq 1 $builds_regCounter`;
do
  dobuild $i
done

echo ""
echo " buildall.sh has finished without erros."

exit;

########## obsolete code:: ############################

if [ "$BUILD_SEMAINEAPI" = "yes" ]; then
#cd $MYPWD/..
# get svn code
#if [ ! -e semaine ]; then
#svn checkout --username florian.eyben https://semaine.opendfki.de/repos/trunk semaine
#fi
cd $MYPWD/../c++
./autogen.sh
./configure --with-apr=$INSTALL_PREFIX --with-apr-util=$INSTALL_PREFIX --with-xerces=$INSTALL_PREFIX --with-activemqcpp=$INSTALL_PREFIX
#export LD_LIBRARY_PATH=$LIB_PREFIX/lib
echo "#!/bin/sh" > config.sh
echo "./configure --with-apr=$INSTALL_PREFIX --with-apr-util=$INSTALL_PREFIX --with-xerces=$INSTALL_PREFIX --with-activemqcpp=$INSTALL_PREFIX" >> config.sh
chmod +x config.sh
echo "#!/bin/sh" > src/run.sh
echo "export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib" >> src/run.sh
echo "\$*" >> src/run.sh
if [ "$DO_CLEAN" = "yes" ]; then
  make clean 
fi
make && echo "YEAH! Semaine API has compiled!!"
SEMAINEAPIPATH="$MYPWD"
fi

