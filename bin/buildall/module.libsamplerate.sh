
#################### Package Configuration    #########################
SRC_BASE="libsamplerate-0.1.7"
#SP_URL="http://www.portaudio.com/archives/pa_stable_v19_20071207.tar.gz"
#SP_URL="http://www.portaudio.com/archives/pa_snapshot.tgz"
SRC_URL="http://www.mega-nerd.com/SRC/libsamplerate-0.1.7.tar.gz"

register_build "libsamplerate" "$SRC_URL" "$SRC_BASE" "func_build_libsamplerate" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_libsamplerate {

    ./configure --prefix=$INSTALL_PREFIX CFLAGS="$OFLAGS $DBGFLAGS" 
    if test "x$?" != "x0" ; then
      return 1;
    fi

    if test "x$1" = "xclean" ; then
      make clean 
      if test "x$?" != "x0" ; then
        return 1;
      fi
    fi
    
    make && make install
    if test "x$?" != "x0" ; then
      return 1;
    fi
    
    addConf "LIBSAMPLERATEPATH" "${builds_dirs[$build_nr]}"

    return 0;
}


