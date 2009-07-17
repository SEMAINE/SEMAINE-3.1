
#################### Package Configuration    #########################
JU_BASE="julius-4.1.2"
#SP_URL="http://www.portaudio.com/archives/pa_stable_v19_20071207.tar.gz"
#SP_URL="http://www.portaudio.com/archives/pa_snapshot.tgz"
JU_URL="http://osdn.dl.sourceforge.jp/julius/37582/julius-4.1.2.tar.gz"

register_build "julius" "$JU_URL" "$JU_BASE" "func_build_julius" $1

#######################################################################

# build_nr var must be set before this function is called
function func_build_julius {


    cat Makefile.in | sed 's/^CONFIG_SUBDIRS.\+$/CONFIG_SUBDIRS=julius libjulius libsent/' > Makefile.in2
    cat Makefile.in2 | sed 's/^SUBDIRS.\+$/SUBDIRS=julius libjulius libsent/' > Makefile.in3
    rm Makefile.in2
    mv Makefile.in Makefile.in.bak
    mv Makefile.in3 Makefile.in
    cat configure | sed 's/^subdirs=.\+$/subdirs="libsent libjulius julius"/' > configure2
    cat configure2 | sed 's/^  for ac_config_dir in .\+$/  for ac_config_dir in julius libjulius libsent; do/' > configure3
    rm configure2
    mv configure configure.bak
    mv configure3 configure
    chmod +x configure


#    cp $DOWNLOAD_PREFIX/julius.semaine.patch .
#    patch -p0 < julius.semaine.patch
    JROOT="$SEMAINE_ROOT/c++/src/tum";
    cp $JROOT/julius4.mod/vocabulary.h libsent/include/sent
    cp $JROOT/julius4.mod/adin.h libsent/include/sent
    cp $JROOT/julius4.mod/recog.h libjulius/include/julius
    cp $JROOT/julius4.mod/jconf.h libjulius/include/julius
    cp $JROOT/julius4.mod/plugin.h libjulius/include/julius
    cp $JROOT/julius4.mod/realtime-1stpass.c libjulius/src
    cp $JROOT/julius4.mod/recogmain.c libjulius/src
    cp $JROOT/julius4.mod/plugin.c libjulius/src

    export CFLAGS="-DEXTERNAL_FV -g -ggdb -fPIC"
    ./configure --prefix=$INSTALL_PREFIX  

    cat libsent/include/sent/config.h | sed 's/.define HAVE_ESD_H 1$//' > libsent/include/sent/config.h2
    cat libsent/include/sent/config.h2 | sed 's/.define HAVE_ZLIB 1$//' > libsent/include/sent/config.h3
    echo "#define ZCAT \"gzip -d -c\"" >> libsent/include/sent/config.h3

    mv libsent/include/sent/config.h libsent/include/sent/config.h.bak
    rm libsent/include/sent/config.h2
    mv libsent/include/sent/config.h3 libsent/include/sent/config.h

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
    
    addConf "JULIUSPATH" "${builds_dirs[$build_nr]}"

    return 0;
}


