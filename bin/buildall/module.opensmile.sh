
#################### Package Configuration    #########################

 ## set this to "true" to enable building of opensmile from the latest svn revision
USE_SVN="false"
 ####################

OPENSMILEMODELS="models-21-12-2009.zip"
OPENSMILEMODELS_URL="http://www.mmk.ei.tum.de/~eyb/semaine/$OPENSMILEMODELS"
# use this line in combination with USE_SVN="true" for the bleeding edge development version:
OPENSMILE_SVN="https://opensmile.svn.sourceforge.net/svnroot/opensmile/trunk"
# enable this to get the semaine-2.0-fixes only
#OPENSMILE_SVN="https://opensmile.svn.sourceforge.net/svnroot/opensmile/branches/semaine-2.0-fixes"

### this i the openSMILE svn revision used in the 2.0 release
#OPENSMILE_SVN_20="https://opensmile.svn.sourceforge.net/svnroot/opensmile"
#OPENSMILE_REV_20="238"

### the tarball should be included in the release, 
### in case it is not, we include the download here
OPENSMILE_TARBALL="openSMILE_SEMAINE_2.0.tar.gz"
OPENSMILE_TARBALL_URL="http://www.mmk.ei.tum.de/~eyb/semaine/$OPENSMILE_TARBALL"


############################################################################

if test -d "$SEMAINE_ROOT/c++/src/tum/opensmile"; then
  true
else
  mkdir "$SEMAINE_ROOT/c++/src/tum/opensmile"
fi


register_semaine_build "tum.opensmile" "c++/src/tum/opensmile" "func_build_opensmile" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_opensmile {
    local doconf='yes'

    if test "x$1" = "xrebuild" ; then
      if test -f Makefile ; then
        doconf='no'
      fi
    fi

    # download the SMILE models
    # the nc version of the download function is used here, 
    # since smile runs without models (feature extractor only)
    if test ! -f $DOWNLOAD_PREFIX/$OPENSMILEMODELS || "x$1" = "xclean" ; then
      echo "DOWNLOADING openSMILE ASR and affect models"
      aux_download_nc "$OPENSMILEMODELS_URL" "$OPENSMILEMODELS"
    fi

    # install models if present in download directory
    if test -f $DOWNLOAD_PREFIX/$OPENSMILEMODELS ; then
      if test ! -f models/emo/salaroc/allft.model.bin || test "x$1" = "xclean" ; then
        echo "unzipping openSMILE SEMAINE models to $PWD/models"
        unzip -o $DOWNLOAD_PREFIX/$OPENSMILEMODELS -d .
        if test "x$?" != "x0" ; then
          echo "Error extracting models.... !!!"
          return 1;
        fi
     fi
    fi

    if test "$USE_SVN" = "true" ; then
      if test -f autogen.sh ; then
        # update an existing tree
        svn update
        # abort on SVN errors (maybe no .svn directory found if you attempt to svn update the tarball release...)
        if test "$?" != 0 ; then
          echo "ERROR during svn update. Are you attempting to update the release tarball which has no .svn directories? Remove the opensmile directory completely. This script will do a clean checkout from the svn trunk then. If you do not want the HEAD revision, edit the file bin/buildall/module.opensmile.sh."
         return 1;
        fi 
      else
        # check out a new tree
        svn co $OPENSMILE_SVN .
      fi
    else

      # check for the tarball in thirdparty/download, and unpack if necessary
      if test -f autogen.sh ; then
        echo "$PWD/autogen.sh was found, assuming the tarball has already been unpacked...";
      else
        P=`pwd`;
        if test ! -f $DOWNLOAD_PREFIX/$OPENSMILE_TARBALL ; then
          echo "DOWNLOADING openSMILE tarball package"
          aux_download_nc "$OPENSMILE_TARBALL_URL" "$OPENSMILE_TARBALL"
        fi
        echo "unpacking openSMILE tarball..."
        tar --overwrite -C $P/../ -zxvf $DOWNLOAD_PREFIX/$OPENSMILE_TARBALL 
      fi
    fi


    if test "x$doconf" = "xyes" ; then
      #echo "Using PortAudio (?) $PORTAUDIOPATH"
    ./autogen.sh ; ./autogen.sh &&
    export LD_LIBRARY_PATH="$INSTALL_PREFIX/lib"
    if test "x$PORTAUDIOPATH" != "x" ; then
      echo ""
      echo "./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src --with-portaudio=$INSTALL_PREFIX CXXFLAGS="-DHAVE_SPEEXLIB -DHAVE_JULIUSLIB -DEXTERNAL_FV  -I$INSTALL_PREFIX/include" LDFLAGS="-L$INSTALL_PREFIX/lib -lspeex -lspeexdsp -ljulius -lsent" --with-rtnnllib="$INSTALL_PREFIX" --without-atklib" > runconfigure.sh
      echo ""

      ./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src --with-portaudio=$INSTALL_PREFIX CXXFLAGS="-DHAVE_JULIUSLIB -DEXTERNAL_FV  -I$INSTALL_PREFIX/include $OFLAGS" LDFLAGS="-L$INSTALL_PREFIX/lib -ljulius -lsent" --without-atklib
      
	#./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src --with-portaudio=$INSTALL_PREFIX CXXFLAGS="-DHAVE_SPEEXLIB -DHAVE_LIBSAMPLERATE -DHAVE_JULIUSLIB -DEXTERNAL_FV  -I$INSTALL_PREFIX/include $OFLAGS" LDFLAGS="-L$INSTALL_PREFIX/lib -lsamplerate -lspeex -lspeexdsp -ljulius -lsent" --without-atklib
      #./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src --with-portaudio=$INSTALL_PREFIX CXXFLAGS="-DHAVE_SPEEXLIB -DHAVE_JULIUSLIB -DEXTERNAL_FV  -I$INSTALL_PREFIX/include" LDFLAGS="-L$INSTALL_PREFIX/lib -lspeex -lspeexdsp -ljulius -lsent" --with-rtnnllib="$INSTALL_PREFIX" --without-atklib
    else
      #./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src
      ./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src CXXFLAGS="-DHAVE_SPEEXLIB -DHAVE_JULIUSLIB -DEXTERNAL_FV  -I$SEMAINE_ROOT/thirdparty/inst/include $OFLAGS" LDFLAGS="-L$SEMAINE_ROOT/thirdparty/inst/lib -lspeex -lspeexdsp -ljulius -lsent" --without-atklib
    fi
    if test "x$?" != "x0" ; then
      return 1;
    fi
    fi

#    if test "x$ATKPATH" != "x" ; then
#      cp $ATKPATH/Resources/UK_SI_ZMFCC/cepmean .
#    fi

  #  if test "x$1" = "xclean" ; then
  #    make clean 
  #    if test "x$?" != "x0" ; then
  #      return 1;
  #    fi
  #  fi
    
    # copy stateinfo.config from java directory
    cp $SEMAINE_ROOT/java/src/eu/semaine/datatypes/stateinfo/stateinfo.config .

    if test "x$1" = "xclean" ; then
      if test -f Makefile ; then
        make clean
      fi
    fi


    # do not install components.. at least for now...
    make -j4 ; make
    if test "x$?" != "x0" ; then
      return 1;
    fi
    
    make install
    if test "x$?" != "x0" ; then
      return 1;
    fi

    addConf "SMILEPATH" "${builds_dirs[$build_nr]}"
    
    # this will create a script that sets the library paths,
    # changes to the build directory of the component
    # and runs the command specified as first parameter
    # the script will be in located in bin/run_components
    createRunScript "SEMAINExtract -C ../auxiliary/opensmileSemaine.conf" prefix "$INSTALL_PREFIX/bin/"

    # create a second version, for debugging the component with gdb
    createRunScript "SEMAINExtract -C ../auxiliary/opensmileSemaine.conf -l 3 -d" prefix "gdb --args $INSTALL_PREFIX/bin/" debug

    return 0;
}



