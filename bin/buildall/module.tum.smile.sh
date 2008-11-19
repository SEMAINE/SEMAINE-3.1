
#################### Package Configuration    #########################

register_semaine_build "tum.smile" "c++/src/tum/smile" "func_build_smile" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_smile {
    local doconf='yes'

    if test "x$1" = "xrebuild" ; then
      if test -f Makefile ; then
        doconf='no'
      fi
    fi

    if test "x$doconf" = "xyes" ; then
echo "PA: $PORTAUDIOPATH"
    ./autogen.sh &&
    if test "x$PORTAUDIOPATH" != "x" ; then
      ./configure --prefix=$INSTALL_PREFIX --with-semaineapi=$SEMAINE_ROOT/c++/src --with-portaudio=$INSTALL_PREFIX
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
    
    addConf "SMILEPATH" "${builds_dirs[$build_nr]}"
    
    # this will create a script that sets the library paths,
    # changes to the build directory of the component
    # and runs the command specified as first parameter
    # the script will be in located in bin/run_components
    createRunScript "start_smile_component.sh" passpath

    return 0;
}



