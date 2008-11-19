
#################### Package Configuration    #########################
XERCES_BASE="xerces-c-src_2_8_0"

XERCES_URL="http://apache.linux-mirror.org/xerces/c/2/sources/$XERCES_BASE.tar.gz"

register_build "xerces-c" "$XERCES_URL" "$XERCES_BASE" "func_build_xerces" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_xerces {
  
    export XERCESCROOT=$PWD
    cd src/xercesc

    if test "x$BUILDSYSTEM" = "xcygwin" then
      ./runConfigure -p cygwin -P $INSTALL_PREFIX
    elif test "x$BUILDSYSTEM" = "xmacosx" then
      ./runConfigure -p macosx -P $INSTALL_PREFIX
    else
      ./runConfigure -p linux -P $INSTALL_PREFIX
    fi
    
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
    
    addConf "XERCESCROOT" "${builds_dirs[$build_nr]}"

    return 0;
}



