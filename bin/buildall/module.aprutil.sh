
#################### Package Configuration    #########################
APRUTIL_BASE="apr-util-1.3.9"

APRUTIL_URL="http://apache.linux-mirror.org/apr/$APRUTIL_BASE.tar.gz"

register_build "aprutil" "$APRUTIL_URL" "$APRUTIL_BASE" "func_build_aprutil" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_aprutil {
  
    ./configure --prefix=$INSTALL_PREFIX --with-apr=$INSTALL_PREFIX 
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
    
    addConf "APRUTILPATH" "${builds_dirs[$build_nr]}"

    return 0;
}



