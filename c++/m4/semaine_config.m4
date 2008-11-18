dnl   M4 macro to save config for linking against the SemaineAPI
dnl            written by Florian Eyben, 11/2008
dnl

AC_DEFUN([SAVE_SEMAINE_CONFIG],
[
  AC_MSG_NOTICE(Saving Semaine API configuration in libsemaineapi.config)
  echo "#!/bin/sh" > libsemaineapi.config
  echo "export SEMAINE_CPPFLAGS=\"$SEMAINE_CXXFLAGS -I$PWD/src\"" >> libsemaineapi.config
  echo "export SEMAINE_LDFLAGS=\"$SEMAINE_LDFLAGS -lsemaineapi -L$PWD/src/.libs\"" >> libsemaineapi.config
])
