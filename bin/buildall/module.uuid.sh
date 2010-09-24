
#################### Package Configuration    #########################
UUID_BASE="e2fsprogs-libs-1.41.2"
#-libs
#UUID_URL="http://surfnet.dl.sourceforge.net/sourceforge/e2fsprogs/e2fsprogs-libs-1.41.3.tar.gz"
#UUID_URL="http://heanet.dl.sourceforge.net/sourceforge/e2fsprogs/$UUID_BASE.tar.gz"
UUID_URL="http://puzzle.dl.sourceforge.net/project/e2fsprogs/e2fsprogs/1.41.2/e2fsprogs-libs-1.41.2.tar.gz"
register_build "libuuid" "$UUID_URL" "$UUID_BASE" "func_build_uuid" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_uuid {


    ./configure --prefix=$INSTALL_PREFIX --disable-rpath --disable-debugfs --disable-imager --disable-resizer --disable-testio-debug

    if test "x$?" != "x0" ; then
      return 1;
    fi
    if test "x$1" = "xclean" ; then
      make clean 
      if test "x$?" != "x0" ; then
        return 1;
      fi
    fi
    
    make #&& make install
    if test "x$?" != "x0" ; then
      return 1;
    fi
    
    cd lib/uuid
    make install
    if test "x$?" != "x0" ; then
      return 1;
    fi

    addConf "UUIDPATH" "${builds_dirs[$build_nr]}/lib/uuid"

    return 0;
}


