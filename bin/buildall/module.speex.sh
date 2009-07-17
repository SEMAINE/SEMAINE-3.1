
#################### Package Configuration    #########################
SP_BASE="speex-1.2rc1"
#SP_URL="http://www.portaudio.com/archives/pa_stable_v19_20071207.tar.gz"
#SP_URL="http://www.portaudio.com/archives/pa_snapshot.tgz"
SP_URL="http://downloads.xiph.org/releases/speex/speex-1.2rc1.tar.gz"

register_build "speex" "$SP_URL" "$SP_BASE" "func_build_speex" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_speex {

    ./configure --prefix=$INSTALL_PREFIX 
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
    
    addConf "SPEEXPATH" "${builds_dirs[$build_nr]}"

    return 0;
}


