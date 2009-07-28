
#################### Package Configuration    #########################
XQILLA_BASE="XQilla-2.2.0"

XQILLA_URL="http://downloads.sourceforge.net/project/xqilla/xqilla/2.2.0/$XQILLA_BASE.tar.gz"

register_build "xqilla" "$XQILLA_URL" "$XQILLA_BASE" "func_build_xqilla" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_xqilla {

    ./configure --prefix=$INSTALL_PREFIX --with-xerces=$XERCESCROOT --enable-debug


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
    
    addConf "XQILLAROOT" "${builds_dirs[$build_nr]}"

    return 0;
}



