
#################### Package Configuration    #########################
CPPUNIT_BASE="cppunit-1.12.1"
#-libs
#UUID_URL="http://surfnet.dl.sourceforge.net/sourceforge/e2fsprogs/e2fsprogs-libs-1.41.3.tar.gz"
CPPUNIT_URL="http://dfn.dl.sourceforge.net/sourceforge/cppunit/$CPPUNIT_BASE.tar.gz"

register_build "cppunit" "$CPPUNIT_URL" "$CPPUNIT_BASE" "func_build_cppunit" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_cppunit {

    ./configure --prefix=$INSTALL_PREFIX 
    if test "x$?" != "x0" ; then
      if test "$BUILDSYSTEM" == "macosx" ; then
        echo "Ignoring error reported by cppunit configure on Mac OS X, and continuing."
      else
        return 1;
      fi
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
    
    addConf "CPPUNITPATH" "${builds_dirs[$build_nr]}"

    return 0;
}


