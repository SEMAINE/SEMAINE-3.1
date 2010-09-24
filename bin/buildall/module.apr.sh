
#################### Package Configuration    #########################
APR_BASE="apr-1.3.12"

APR_URL="http://apache.linux-mirror.org/apr/$APR_BASE.tar.gz"
#APR_URL="http://apache.autinity.de/apr/$APR_BASE.tar.gz"

register_build "apr" "$APR_URL" "$APR_BASE" "func_build_apr" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_apr {
  
    ./configure --prefix=$INSTALL_PREFIX --enable-threads
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
    
    # APRPATH=`pwd`
    addConf "APRPATH" "${builds_dirs[$build_nr]}"
    #echo "APRPATH=\"$APRPATH\"" >> conf/semaine-api.conf

    return 0;
}



#if [ "$WINDOWS" = "yes" ]; then
#cp $MYPWD/apr.h.in.cygwin.patch ./include
#cd include/
#patch -N -p0 < apr.h.in.cygwin.patch
# apply APR cygwin patches
# 1. apr.h  without winsock and windows.h !
# 2. struct in_addr in apr_network_....h  is duplicate!
# APR_HAVE_IN_ADDR 1
# APR_HAVE_WINDOWS_H 0
# APR_HAVE_WINSOCK2_H 0
# APR_PTHREAD   everything 1
# APR_HAS_THREADS  everything 1
#echo "#ifdef WIN32" >> apr.h.in
#echo "#undef WIN32" >> apr.h.in
#echo "#endif" >> apr.h.in
#ifdef WIN32
#undef WIN32
#endif
#cd ..
#fi
