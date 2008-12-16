dnl m4 macro to configure smile build targets
dnl   written by Florian Eyben, 11/2008
dnl
dnl Configured the targets to be build based on the --enable/--disable options 
dnl Further, disables those targets that cannot be built due to missing dependencies
dnl
dnl IMPORTANT: call this macro AFTER the library checks!
dnl

AC_DEFUN([CONFIGURE_TARGETS],
[
  dnl the defaults:
  enable_liverec='yes'
  build_featum='yes'
  build_featumamq='yes'
  build_tumrec='no'
  build_tumsync='no'
  build_audiorecorder='no'
  build_vinylsplit='no'

  AC_ARG_ENABLE([liverec],[  --enable-liverec  enable live input feature.  default=yes],
          enable_liverec="$enableval", enable_liverec="$enable_liverec")

  if test x$enable_liverec = xyes; then
    AC_DEFINE([WITH_LIVEREC],[1],[Support live input from soundcard/microphone])
  fi
 
  AC_ARG_ENABLE([featum],[  --enable-featum  build standalone commandline feature extractor.  default=yes],
           build_featum="$enableval", build_featum="$build_featum")

  AC_ARG_ENABLE([semaine-ft],[  --enable-semaine  build semaine component (feature extractor).  default=yes],
            build_featumamq="$enableval", build_featumamq="$build_featumamq")

  AC_ARG_ENABLE([tumrec],[  --enable-tumrec  build commandline audio recorder (older c version).  default=no],
            build_tumrec="$enableval", build_tumrec="$build_tumrec")

  AC_ARG_ENABLE([tumsync],[  --enable-tumsync  build semaine audio/video sync tool.  default=no],
            build_tumsync="$enableval", build_tumsync="$build_tumsync")

  AC_ARG_ENABLE([audiorecorder],[  --enable-audiorecorder  build simple commandline audio-recorder (c++ implementation).  default=no],
            build_audiorecorder="$enableval", build_audiorecorder="$build_audiorecorder")

  AC_ARG_ENABLE([audiosplit],[  --enable-audiosplit  build intelligent commandline audio splitter.  default=no],
            build_audiosplit="$enableval", build_audiosplit="$build_audiosoplit")

  dnl disable all targets that cannot be built due to missing dependencies
  if test "x$SEMAINEAPI" != "xyes" ; then
    if test "x$build_featumamq" = "xyes" ; then
      AC_MSG_WARN(Disabling target semaine-ft beacuse libsemaineapi was not found or is not useable)
    fi
    build_featumamq='no'
  fi

  if test "x$PORTAUDIO" != "xyes" ; then
    if test "x$build_tumrec" = "xyes" ; then
      AC_MSG_WARN(Disabling target tumrec beacuse libportaudio was not found or is not useable)
    fi
    if test "x$build_audiorecorder" = "xyes" ; then
      AC_MSG_WARN(Disabling target audiorecorder beacuse libportaudio was not found or is not useable)
    fi
    build_tumrec='no'
    build_audiorecorder='no'
  fi

  AM_CONDITIONAL([BUILD_featum],[test x$build_featum = xyes])
  AM_CONDITIONAL([BUILD_featumamq],[test x$build_featumamq = xyes])
  AM_CONDITIONAL([BUILD_tumrec],[test x$build_tumrec = xyes])
  AM_CONDITIONAL([BUILD_tumsync],[test x$build_tumsync = xyes])
  AM_CONDITIONAL([BUILD_audiorecorder],[test x$build_audiorecorder = xyes])
  AM_CONDITIONAL([BUILD_audiosplit],[test x$build_audiosplit = xyes])

])
