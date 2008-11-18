
#################### Package Configuration    #########################
AMQCPP_BASE="activemq-cpp-2.2.1"

AMQCPP_URL="http://apache.osuosl.org/activemq/activemq-cpp/source/activemq-cpp-2.2.1-src.tar.gz"

register_build "activemqcpp" "$AMQCPP_URL" "$AMQCPP_BASE" "func_build_activemqcpp" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_activemqcpp {
  
    cd $BUILD_PREFIX/${builds_dirs[$build_nr]}

    #ACTIVEMQCPPPATH=$PWD

        # cygwin:  requires activemq patch!!   error in some thread file...
    ./autogen.sh
    if test "x$?" != "x0" ; then
      return 1;
    fi

    ./configure --with-apr=$INSTALL_PREFIX --with-apr-util=$INSTALL_PREFIX --prefix=$INSTALL_PREFIX 
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
    
    addConf "ACTIVEMQCPPPATH" "$BUILD_PREFIX/${builds_dirs[$build_nr]}"

    return 0;
}


