
#################### Package Configuration    #########################
PA_BASE="portaudio"
#PA_URL="http://www.portaudio.com/archives/pa_stable_v19_20071207.tar.gz"
PA_URL="http://www.portaudio.com/archives/pa_snapshot.tgz"

register_build "portaudio" "$PA_URL" "$PA_BASE" "func_build_portaudio" $1

#######################################################################


if [ -e "$SEMAINE_ROOT/bin/buildall/portaudio_working_2.1.tgz" ]; then
  cp $SEMAINE_ROOT/bin/buildall/portaudio_working_2.1.tgz $DOWNLOAD_PREFIX/pa_snapshot.tgz
else
if [ -e "$SEMAINE_ROOT/bin/buildall/portaudio_working.tgz" ]; then
  cp $SEMAINE_ROOT/bin/buildall/portaudio_working.tgz $DOWNLOAD_PREFIX/pa_snapshot.tgz
fi
fi

# build_nr var must be set before this function is called
function func_build_portaudio {

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

    # OBSOLETE: do not install portaudio, since it may conflict with locally installed portaudio
    make && make install
    if test "x$?" != "x0" ; then
      return 1;
    fi
    
    addConf "PORTAUDIOPATH" "${builds_dirs[$build_nr]}"

    return 0;
}


