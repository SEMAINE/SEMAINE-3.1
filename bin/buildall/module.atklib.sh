
#################### Package Configuration    #########################
ATK_BASE="atk160"
ATK_URL="atk160.tgz"  # you must download this .tgz manually and put it into
                      # the download/ folder in BASEDIR
#PA_URL="http://www.portaudio.com/archives/pa_snapshot.tgz"

register_build "atk" "$ATK_URL" "$ATK_BASE" "func_build_atk" $1

#######################################################################

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
if test "x`find /usr/include -name *.h | grep /asound.h`" = "x" ; then
    echo "Alsasound headers not found in /usr/include ! "
    echo "Install the libasound2-dev package !"
    echo "on Debian/Ubuntu: sudo apt-get install libasound2-dev"
    builderror
fi

# build_nr var must be set before this function is called
function func_build_atk {


    if test "x$1" = "xclean" ; then
      make clean 
      if test "x$?" != "x0" ; then
        return 1;
      fi
    fi
    
    # do not install portaudio, since it may conflict with locally installed portaudio
    make ATKLib && make HTKLib  #&& make install
    if test "x$?" != "x0" ; then
      return 1;
    fi
    
    addConf "ATKPATH" "${builds_dirs[$build_nr]}"
    addConf "ATKCPPFLAGS" "-I${builds_dirs[$build_nr]}/ATKLib -I${builds_dirs[$build_nr]}/HTKLib"
    addConf "ATKLDFLAGS" "-L${builds_dirs[$build_nr]}/ATKLib -L${builds_dirs[$build_nr]}/HTKLib"

    return 0;
}


