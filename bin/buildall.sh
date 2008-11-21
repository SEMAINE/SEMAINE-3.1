#!/bin/bash

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
#  SEMAINE_ROOT    : top level root directory of SEMAINE build tree
#  bdir            : current build directory root
#
# To build the semaine system just type:  ./buildall.sh    (or "bash buildall.sh")
#
# If you get download errors (wget) then you might need to change the 
# download URLs. They are defined in each module's shell script
#

cd `dirname $0`

# directory where all the builds will be performed
# everything will be installed in $BASE/inst by default
# this path can be absolute or relative
BASE="../thirdparty"
#######################################################


######### SYSTEM CONFIGURATION ###
BUILDSYSTEM="linux"   # supported: linux, macosx, cygwin (currently not working)
#default is linux/unix
case "`uname`" in
  CYGWIN*) BUILDSYSTEM="cygwin" ;;
  Darwin*) BUILDSYSTEM="macosx"
esac
##################################


# directory where modules config is located and various includes
INCDIR=$PWD/buildall

# create build directory, if it doesn't exist
MYPWD=$PWD
SEMAINE_ROOT=$PWD/..
cd $SEMAINE_ROOT
SEMAINE_ROOT=$PWD
cd $MYPWD
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
for ((i=1; i<=$builds_regCounter; i+=1));
do
  dobuild $i
done

echo ""
echo " buildall.sh has finished successfully!"

exit;



