
# file that, if exists, indicated sucessful and complete build (including installation)
BUILDDONE='.semaine.build.success'

# file that contains build configuration necessary for dependent packages
BUILDCONF='.semaine.build.config'

# initialized with 0
builds_regCounter=0

## arrays:
# builds_names
# builds_urls
# builds_dirs
# builds_funcs

## functions for registering components:

#register_build ( name, URL, directory(untared), build_function )
function register_build {
  builds_regCounter=`expr $builds_regCounter + 1`;
  builds_names[$builds_regCounter]=$1;
  builds_urls[$builds_regCounter]=$2;
  builds_dirs[$builds_regCounter]=$3;
  builds_funcs[$builds_regCounter]=$4;
  builds_clean[$builds_regCounter]='no'
  if test "x$5" = "xenabled" ; then
    builds_enabled[$builds_regCounter]='yes'
  else
    builds_enabled[$builds_regCounter]='no'
  fi
  if test "x$5" = "xclean" ; then
    builds_enabled[$builds_regCounter]='yes'
    builds_clean[$builds_regCounter]='yes'
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

  for myI in `seq 1 $builds_regCounter`;
  do
    if test "x${builds_enabled[$myI]}" = "xyes" ; then
      dnURL=${builds_urls[$myI]};
      if test -f `basename "$dnURL"` ; then
        echo "download file for ${builds_names[$myI]} already exists, skipping."
      else 
        wget "$dnURL"
        if test "x$?" != "x0" ; then
          builderror "Download failed!"
        fi
      fi 
    fi
  done
}

function unpack_missing {
  for myI in `seq 1 $builds_regCounter`;
  do
    if test "x${builds_enabled[$myI]}" = "xyes" ; then
      dnBASE=`basename ${builds_urls[$myI]}`;
      ext=`echo $dnBASE | grep .gz`;
      if test "x$ext" != "x" ; then
        UNZ='-z'
      fi
      ext=`echo $dnBASE | grep .bz2`;
      if test "x$ext" != "x" ; then
        UNZ='-j'
      fi
      # TODO add support for ZIP files!
      ext=`echo $dnBASE | grep .zip`;
      if test "x$ext" != "x" ; then
        builderror "ZIP decompression not yet implemented! (build_funcs.sh)"
      fi

      #if clean option is given, untar always
      if test "x${builds_clean[$myI]}" = "xyes" ; then
        tar -C $BUILD_PREFIX $UNZ -xvf $DOWNLOAD_PREFIX/$dnBASE
        if test "x$?" != "x0" ; then
          builderror "Unpack failed!"
        fi
      else
        if test ! -d "$BUILD_PREFIX/${builds_dirs[$myI]}" ; then
          tar -C $BUILD_PREFIX $UNZ -xvkf $DOWNLOAD_PREFIX/$dnBASE 2>/dev/null
        else
          echo "already unpacked. not unpacking again."
        fi
      fi
    fi
  done

}

## addConf( var_name, value )
# build_nr must be set!
function addConf {
  local bdir="$BUILD_PREFIX/${builds_dirs[$build_nr]}"
  cd "$bdir"
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
  local bdir="$BUILD_PREFIX/${builds_dirs[$build_nr]}"
  cd "$bdir"
  if test -f $BUILDCONF ; then
    . $BUILDCONF
  fi
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
    bdir="$BUILD_PREFIX/${builds_dirs[$build_nr]}"
    cd "$bdir"
    # check for complete build...  (and not clean option given)
    noBuild='no'
    if test "x$doclean" != "xyes" ; then
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
        if test -f "$BUILDDONE" ; then 
          rm -f $BUILDDONE
        fi
        builderror
      else
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
