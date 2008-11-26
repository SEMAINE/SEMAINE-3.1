
#################### Package Configuration    #########################

register_semaine_build "tum.asr" "c++/src/tum/asr" "func_build_asr" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_asr {
    local doconf='yes'

    if test "x$ATKPATH" = "x" ; then
      echo "ERROR: ATKlib not found. You must download and install it manually first!"
      echo "  enable the module \"module_atklib.sh\" to get detailed instructions what to do" 
      builderror
    fi

    # apply pipe input patches, if present
    if test "x$ATKPATCHES" != "x" ; then
      MYPWD=$PWD;
      cp $ATKPATH/ATKLib/ASource.cpp ./src/
      cp $ATKPATH/ATKLib/ASource.h ./src/
      cp $ATKPATCHES/pipesource*.patch ./src/
      cd src/
      echo "Applying patches to ASource.cpp to add pipe input support"
      patch -N -p0 < pipesource.cpp.patch
      patch -N -p0 < pipesource.h.patch
      mv ASource.cpp APipeSource.cpp
      mv ASource.h APipeSource.h
      cd $MYPWD
    else
      echo "WARNING: pipe input patches not found!! tum.asr module might not compile!"
    fi

    if test "x$1" = "xrebuild" ; then
      if test -f Makefile ; then
        doconf='no'
      fi
    fi

    if test "x$doconf" = "xyes" ; then
    ./autogen.sh &&
    if test "x$ATKPATH" != "x" ; then
      ./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src --with-atklib=$ATKPATH
    else
      ./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src
    fi
    if test "x$?" != "x0" ; then
      return 1;
    fi
    fi

    if test "x$1" = "xclean" ; then
      make clean 
      if test "x$?" != "x0" ; then
        return 1;
      fi
    fi
    
    # do not install components.. at least for now...
    make # && make install
    if test "x$?" != "x0" ; then
      return 1;
    fi
    
    addConf "ASRPATH" "${builds_dirs[$build_nr]}"
    
    # this will create a script that sets the library paths,
    # changes to the build directory of the component
    # and runs the command specified as first parameter
    # the script will be in located in bin/run_components
    createRunScript "start_asr_component.sh" passpath

    return 0;
}



