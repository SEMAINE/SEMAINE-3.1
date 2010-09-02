
#################### Package Configuration    #########################

register_semaine_build "libsemaineapi" "c++" "func_build_semaineapi" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_semaineapi {
    doconf='yes'

    if test "x$1" = "xrebuild" ; then
      if test -f Makefile ; then
        doconf='no'
      fi
    fi

    if test "x$doconf" = "xyes" ; then
 
    aclocal
    ./autogen.sh
    if test "x$?" != "x0" ; then
      return 1;
    fi

    echo "#!/bin/sh" > doconfig.sh
    echo "./configure --prefix=$INSTALL_PREFIX --with-apr=$INSTALL_PREFIX --with-apr-util=$INSTALL_PREFIX --with-xerces=$INSTALL_PREFIX --with-activemqcpp=$INSTALL_PREFIX --with-xqilla=$INSTALL_PREFIX" >> doconfig.sh

    sh doconfig.sh
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
    make && make install
    if test "x$?" != "x0" ; then
      return 1;
    fi
    
    addConf "SEMAINEAPIPATH" "${builds_dirs[$build_nr]}"

    return 0;
}



