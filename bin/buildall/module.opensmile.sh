
#################### Package Configuration    #########################
OPENSMILEMODELS="models.opensmile_v1.zip"
OPENSMILEMODELS_URL="http://server/$SMILEMODELS"

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
    aux_download_nc "$OPENSMILEMODELS_URL" "$OPENSMILEMODELS"

    # install models if present in download directory
    if test -f $DOWNLOAD_PREFIX/$OPENSMILEMODELS ; then
      if test ! -f models/sal/arousal.model || test ! -f models/avic2/interest.model || test "x$1" = "xclean" ; then
        echo "unzipping SMILE models to $PWD/models"
        unzip -o $DOWNLOAD_PREFIX/$OPENSMILEMODELS -d .
        if test "x$?" != "x0" ; then
          echo "Error extracting models.... !!!"
          return 1;
        fi
     fi
    fi

    if test "x$doconf" = "xyes" ; then
      #echo "Using PortAudio (?) $PORTAUDIOPATH"
    ./autogen.sh &&
    if test "x$PORTAUDIOPATH" != "x" ; then
      echo ""
      echo "./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src --with-portaudio=$INSTALL_PREFIX CXXFLAGS="-DHAVE_SPEEXLIB -DHAVE_JULIUSLIB -DEXTERNAL_FV  -I$SEMAINE_ROOT/thirdparty/inst/include" LDFLAGS="-L$SEMAINE_ROOT/thirdparty/inst/lib -lspeex -lspeexdsp -ljulius -lsent" --without-atklib"
      echo ""
      ./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src --with-portaudio=$INSTALL_PREFIX CXXFLAGS="-DHAVE_SPEEXLIB -DHAVE_JULIUSLIB -DEXTERNAL_FV  -I$SEMAINE_ROOT/thirdparty/inst/include" LDFLAGS="-L$SEMAINE_ROOT/thirdparty/inst/lib -lspeex -lspeexdsp -ljulius -lsent" --without-atklib
    else
      #./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src
      ./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src CXXFLAGS="-DHAVE_SPEEXLIB -DHAVE_JULIUSLIB -DEXTERNAL_FV  -I$SEMAINE_ROOT/thirdparty/inst/include" LDFLAGS="-L$SEMAINE_ROOT/thirdparty/inst/lib -lspeex -lspeexdsp -ljulius -lsent" --without-atklib
    fi
    if test "x$?" != "x0" ; then
      return 1;
    fi
    fi

    if test "x$ATKPATH" != "x" ; then
      cp $ATKPATH/Resources/UK_SI_ZMFCC/cepmean .
    fi

  #  if test "x$1" = "xclean" ; then
  #    make clean 
  #    if test "x$?" != "x0" ; then
  #      return 1;
  #    fi
  #  fi
    
    # do not install components.. at least for now...
    make -j4 ; make
    if test "x$?" != "x0" ; then
      return 1;
    fi
    
    addConf "SMILEPATH" "${builds_dirs[$build_nr]}"
    
    # this will create a script that sets the library paths,
    # changes to the build directory of the component
    # and runs the command specified as first parameter
    # the script will be in located in bin/run_components
    createRunScript "start_smile_component.sh" passpath

    return 0;
}



