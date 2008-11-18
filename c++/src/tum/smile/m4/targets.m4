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
#  build_featum='yes'
  build_featumamq='yes'
  build_tumrec='no'
  build_tumsync='no'

#  AC_ARG_ENABLE([standalone-ft],[build standalone commandline feature extractor.  default=$BUILD_featum],
#            build_featum="$withval", build_featum="$build_featum")

  AC_ARG_ENABLE([semaine-ft],[build semaine component (feature extractor).  default=$BUILD_featumamq],
            build_featumamq="$withval", build_featumamq="$build_featumamq")

  AC_ARG_ENABLE([tumrec],[build commandline audio recorder.  default=$BUILD_tumrec],
            build_tumrec="$withval", build_tumrec="$build_tumrec")

  AC_ARG_ENABLE([tumsync],[build commandline av sync tool.  default=$BUILD_tumsync],
            build_tumsync="$withval", build_tumsync="$build_tumsync")

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
    build_tumrec='no'
  fi

#  AM_CONDITIONAL([BUILD_featum],[test x$build_featum = xyes])
  AM_CONDITIONAL([BUILD_featumamq],[test x$build_featumamq = xyes])
  AM_CONDITIONAL([BUILD_tumrec],[test x$build_tumrec = xyes])
  AM_CONDITIONAL([BUILD_tumsync],[test x$build_tumsync = xyes])

])
