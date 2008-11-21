
# file that, if exists, indicated sucessful and complete build (including installation)
BUILDDONE='.semaine.build.success'

# file that contains build configuration necessary for dependent packages
BUILDCONF="$CONF_PREFIX/semaine.build.config"

# initialized with 0
builds_regCounter=0

## arrays:
# builds_names
# builds_urls
# builds_dirs
# builds_funcs

## functions for registering components:

# register a third-party package build
#register_build ( name, URL, directory(untared), build_function )
function register_build {
  builds_regCounter=`expr $builds_regCounter + 1`;
  builds_names[$builds_regCounter]=$1;
  builds_urls[$builds_regCounter]=$2;
  builds_dirs[$builds_regCounter]=$BUILD_PREFIX/$3;
  builds_funcs[$builds_regCounter]=$4;
  builds_clean[$builds_regCounter]='no'
  if test "x$5" = "xenabled" ; then
    builds_enabled[$builds_regCounter]='yes'
  else
    builds_enabled[$builds_regCounter]='no'
  fi
  if test "x$5" = "xclean" ; then
    builds_enabled[$builds_regCounter]='yes'
    builds_clean[$builds_regCounter]='clean'
  fi
  if test "x$5" = "xdistclean" ; then
    builds_enabled[$builds_regCounter]='yes'
    builds_clean[$builds_regCounter]='distclean'
  fi
  if test "x$5" = "xrebuild" ; then
    builds_enabled[$builds_regCounter]='yes'
    builds_clean[$builds_regCounter]='rebuild'
  fi
}

# register a semaine internal build (i.e. code in c++/src/...)
#register_semaine_build ( name, directory(relative to SEMAINE_ROOT), build_function )
function register_semaine_build {
  builds_regCounter=`expr $builds_regCounter + 1`;
  builds_names[$builds_regCounter]=$1;
  builds_urls[$builds_regCounter]="";
  builds_dirs[$builds_regCounter]=$SEMAINE_ROOT/$2;
  builds_funcs[$builds_regCounter]=$3;
  builds_clean[$builds_regCounter]='no'
  if test "x$4" = "xenabled" ; then
    builds_enabled[$builds_regCounter]='yes'
  else
    builds_enabled[$builds_regCounter]='no'
  fi
  if test "x$4" = "xclean" ; then
    builds_enabled[$builds_regCounter]='yes'
    builds_clean[$builds_regCounter]='clean'
  fi
  if test "x$4" = "xdistclean" ; then
    builds_enabled[$builds_regCounter]='yes'
    builds_clean[$builds_regCounter]='distclean'
  fi
  if test "x$4" = "xrebuild" ; then
    builds_enabled[$builds_regCounter]='yes'
    builds_clean[$builds_regCounter]='rebuild'
  fi
}
function builderror {
  echo "[BUILD ERROR] stopping buildall.sh!!"
  echo "failed building ${builds_names[$build_nr]}"
  if test "x$1" != "x" ; then
    echo "details: $1"
  fi
  exit;
}

function download_missing {
  cd $DOWNLOAD_PREFIX

  for (( myI=1; myI <= $builds_regCounter; myI += 1 )) ;
  do
    if test "x${builds_enabled[$myI]}" = "xyes" ; then
      dnURL=${builds_urls[$myI]};
      if test "x`echo $dnURL | grep /`" != "x" ; then 
      #if test "x$dnURL" != "x" ; then
      if test -f `basename "$dnURL"` ; then
        echo "download file for ${builds_names[$myI]} already exists, skipping."
      else 
        if test -n "`which wget`"; then
          wget "$dnURL"
        elif test -n "`which curl`"; then
          curl -o `basename "$dnURL"` "$dnURL"
        else
          builderror "could not find any download tool"
        fi
        if test "x$?" != "x0" ; then
          builderror "Download failed!"
        fi
      fi 
      fi
    fi
  done
}

function unpack_missing {
  for (( myI=1; myI<=$builds_regCounter; myI+=1 )) ;
  do
    if test "x${builds_enabled[$myI]}" = "xyes" ; then
      if test "x${builds_urls[$myI]}" != "x" ; then
      dnBASE=`basename ${builds_urls[$myI]}`;

      ext=`echo $dnBASE | grep .tar.gz`;
      if test "x$ext" != "x" ; then
        UNZ='-z'
      fi
      ext=`echo $dnBASE | grep .tgz`;
      if test "x$ext" != "x" ; then
        UNZ='-z'
      fi
      ext=`echo $dnBASE | grep .tar.bz2`;
      if test "x$ext" != "x" ; then
        UNZ='-j'
      fi
      # TODO add support for ZIP files!
      ext=`echo $dnBASE | grep .zip`;
      if test "x$ext" != "x" ; then
        builderror "ZIP decompression not yet implemented! (build_funcs.sh)"
      fi

      #if clean option is given, untar always
      if test "x${builds_clean[$myI]}" = "xclean" ; then
        tar -C $BUILD_PREFIX $UNZ -xvf $DOWNLOAD_PREFIX/$dnBASE
        if test "x$?" != "x0" ; then
          builderror "Unpack failed!"
        fi
      else
        if test ! -d "${builds_dirs[$myI]}" ; then
          tar -C $BUILD_PREFIX $UNZ -xvkf $DOWNLOAD_PREFIX/$dnBASE 2>/dev/null
        else
          echo "${builds_names[$myI]} already unpacked. not unpacking again."
        fi
      fi
      fi
    fi
  done

}

## addConf( var_name, value )
# build_nr must be set!
function addConf {
  #local bddir="${builds_dirs[$build_nr]}"
  #cd "$bddir"
  if test -e $BUILDCONF ; then
    existing=`cat $BUILDCONF | grep "$1"`
    if test "x$existing" = "x" ; then
      echo "export $1=\"$2\"" >> $BUILDCONF
    else
      cat $BUILDCONF | grep -v "$1" > $BUILDCONF.tmp
      echo "export $1=\"$2\"" >> $BUILDCONF.tmp
      rm -f $BUILDCONF
      mv $BUILDCONF.tmp $BUILDCONF
    fi
  else
    echo "#!/bin/sh" > $BUILDCONF
    echo "export $1=\"$2\"" >> $BUILDCONF
  fi
}

# build_nr must be set!
function loadConf {
  #local bddir="${builds_dirs[$build_nr]}"
  #cd "$bddir"
  if test -f $BUILDCONF ; then
    . $BUILDCONF
  fi
}

function createRunScript {
  local rundir="${builds_dirs[$build_nr]}"
  local runcmd="$1"
  shift 1
  local runpath=''
  if test "x$1" = "xpasspath" ; then
    shift 1
    runpath=$rundir
  fi
  local scriptname="start_component_${builds_names[$build_nr]}"
  if test ! -d "$SEMAINE_ROOT/bin/run_components" ; then
    mkdir $SEMAINE_ROOT/bin/run_components
  fi 
  cd $SEMAINE_ROOT/bin/run_components
  echo "#!/bin/sh" > $scriptname
  echo "export LD_LIBRARY_PATH=\"$INSTALL_PREFIX/lib:$SEMAINE_ROOT/c++/src/.libs\"" >> $scriptname
  echo "cd $rundir" >> $scriptname
  echo "echo \"COMPONENT START SCRIPT: starting semaine c++ component \"${builds_names[$build_nr]}\"\"" >> $scriptname
  echo "./$runcmd $runpath \$*" >> $scriptname
  echo "echo \"COMPONENT START SCRIPT: component \"${builds_names[$build_nr]}\" exited with status \$?\"" >> $scriptname
  chmod +x $scriptname
}

. $INCDIR/modules.config

# dobuild <nr>
function dobuild {
  build_nr="$1"
  doclean=${builds_clean[$build_nr]}
  if test "x$2" != "x" ; then
    doclean="$2"
  fi
  if test "x${builds_enabled[$build_nr]}" = "xyes" ; then
    bdir="${builds_dirs[$build_nr]}"
    cd "$bdir"
    # check for complete build...  (and not clean option given)
    noBuild='no'
    if test "x$doclean" = "xno" ; then
      # check for build
      if test -f $BUILDDONE ; then
        noBuild='yes'
        echo ""
        echo "--->> NOT building ${builds_names[$build_nr]} , already built (use clean option to rebuild) <<<"
      fi 
    else
      rm -f $BUILDDONE
    fi
    if test "x$noBuild" != "xyes" ; then
      echo ""
      echo "---->>>> building ${builds_names[$build_nr]}"
      ${builds_funcs[$build_nr]} $doclean
      if test "x$?" != "x0" ; then
        cd $bdir
        if test -f "$BUILDDONE" ; then 
          rm -f $BUILDDONE
        fi
        builderror
      else
        cd $bdir
        echo "yes" > $BUILDDONE
      fi
    fi
  else
    if test "x${builds_enabled[$build_nr]}" = "xno" ; then
      echo ""
      echo "---->>>> skipping ${builds_names[$build_nr]} (disabled) <<<"
    fi
  fi
  loadConf 
}




#register_build "name1" "http://" "dir" "dummy"
#${builds_funcs[1]}
