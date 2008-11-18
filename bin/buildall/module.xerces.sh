
#################### Package Configuration    #########################
XERCES_BASE="xerces-c-src_2_8_0"

XERCES_URL="http://apache.linux-mirror.org/xerces/c/2/sources/$XERCES_BASE.tar.gz"

register_build "xerces-c" "$XERCES_URL" "$XERCES_BASE" "func_build_xerces" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_xerces {
  
    cd $BUILD_PREFIX/${builds_dirs[$build_nr]}
    export XERCESCROOT=$PWD
    cd src/xercesc

    ./runConfigure -p linux -P $INSTALL_PREFIX
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
    
    addConf "XERCESCROOT" "$BUILD_PREFIX/${builds_dirs[$build_nr]}"

    return 0;
}



