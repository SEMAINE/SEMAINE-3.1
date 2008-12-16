
#################### Package Configuration    #########################
ATK_BASE="atk160"
ATK_URL="atk160.tgz"  # you must download this .tgz manually and put it into
                      # the download/ folder in BASEDIR
ATK_PATCHES_BASE="atkpatches"
ATK_PATCHES="atkpatches.tgz"  # download this file manually from the wiki and put it
                              # into the download/ folder in BASEDIR (thirdparty/)

register_build "atk" "$ATK_URL" "$ATK_BASE" "func_build_atk" $1

#######################################################################

if test ! "x$1" = "xdisabled" ; then


if test ! -f $DOWNLOAD_PREFIX/$ATK_PATCHES ; then
  echo ""
  echo "ERROR: ATKpatches ($ATK_URL) were not found in $DOWNLOAD_PREFIX. Please download the file from the wiki and place it in the download directory!"
  echo ""
  builderror
fi

# check if atk download exists, if not display instructions on how to download:
if test ! -f $DOWNLOAD_PREFIX/$ATK_URL ; then
  echo ""
  echo "ERROR: ATK tarball ($ATK_URL) was not found in $DOWNLOAD_PREFIX"
  echo ""
  echo "In order to build the ASR component, you must download the ATK toolkit"
  echo "You can get it from: http://htk.eng.cam.ac.uk/develop/atk.shtml "
  echo "Then place the tarball in $DOWNLOAD_PREFIX"
  echo ""
  echo "Prior to the download you must register and apply for a UserID:"
  echo "  http://htk.eng.cam.ac.uk/register.shtml "
  echo "** Be sure to remember the UserID you choose! It will not be included in"
  echo "   the password e-mail you will get after registration! **"
  echo ""
  builderror
fi

if test "x`find /usr/include -name *.h | grep /Xlib.h`" = "x" ; then
   echo "X11 headers not found in /usr/include !"
   echo "Install the libx11-dev package first. "
   echo "on Debian/Ubuntu: sudo apt-get install libx11-dev"
   builderror
fi

#"find /usr/include -name *.h | grep /asound.h"
if test "x`find /usr/include -name *.h | grep /asoundlib.h`" = "x" ; then
    echo "Alsasound headers not found in /usr/include ! "
    echo "Install the libasound2-dev package !"
    echo "on Debian/Ubuntu: sudo apt-get install libasound2-dev"
    builderror
fi

fi

# build_nr var must be set before this function is called
function func_build_atk {


    if test "x$1" = "xclean" ; then
      make clean 
      if test "x$?" != "x0" ; then
        return 1;
      fi
    fi

    # apply HTKLib patches
    MYPWD=$PWD
    if test -f $DOWNLOAD_PREFIX/$ATK_PATCHES ; then
      if test ! -d $BUILD_PREFIX/$ATK_PATCHES_BASE ; then
        tar -C $BUILD_PREFIX -zxvf $DOWNLOAD_PREFIX/$ATK_PATCHES
      else
        if test "x$1" = "xclean" ; then
          tar -C $BUILD_PREFIX -zxvf $DOWNLOAD_PREFIX/$ATK_PATCHES
        fi
      fi
#      cp $BUILD_PREFIX/$ATK_PATCHES_BASE/HWave*.patch $BUILD_PREFIX/$ATK_BASE/HTKLib
#      cp $BUILD_PREFIX/$ATK_PATCHES_BASE/ABuffer.patch $BUILD_PREFIX/$ATK_BASE/ATKLib
#      cd $BUILD_PREFIX/$ATK_BASE/HTKLib
#      echo -n "Patching HWave to add PAUDIO input type... "
#      patch -N -p0 < HWave.c.patch
#      patch -N -p0 < HWave.h.patch
#      echo "done."
      cd $BUILD_PREFIX/$ATK_BASE/ATKLib
      echo -n "Patching ABuffer to add DataAvailable() method for non-blocking operation... "
      patch -N -p0 < ABuffer.patch
      echo "done."
    fi
    cd $MYPWD

    # do not install portaudio, since it may conflict with locally installed portaudio
    make HTKLib && make ATKLib   #&& make install
    if test "x$?" != "x0" ; then
      return 1;
    fi
    
    addConf "ATKPATCHES" "$BUILD_PREFIX/$ATK_PATCHES_BASE"
    addConf "ATKPATH" "${builds_dirs[$build_nr]}"
    addConf "ATKCPPFLAGS" "-I${builds_dirs[$build_nr]}/ATKLib -I${builds_dirs[$build_nr]}/HTKLib"
    addConf "ATKLDFLAGS" "-L${builds_dirs[$build_nr]}/ATKLib -L${builds_dirs[$build_nr]}/HTKLib"

    return 0;
}


