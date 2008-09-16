dnl -------------------------------------------------------- -*- autoconf -*-
dnl Licensed to the Apache Software Foundation (ASF) under one or more
dnl contributor license agreements.  See the NOTICE file distributed with
dnl this work for additional information regarding copyright ownership.
dnl The ASF licenses this file to You under the Apache License, Version 2.0
dnl (the "License"); you may not use this file except in compliance with
dnl the License.  You may obtain a copy of the License at
dnl
dnl     http://www.apache.org/licenses/LICENSE-2.0
dnl
dnl Unless required by applicable law or agreed to in writing, software
dnl distributed under the License is distributed on an "AS IS" BASIS,
dnl WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl See the License for the specific language governing permissions and
dnl limitations under the License.
dnl -------------------------------------------------------- -*- autoconf -*-
dnl
dnl  This Macro File defines functions that help to manage the libraries
dnl  dependance on the Apache ActiveMQ-CPP Library.
dnl
dnl  ACTIVEMQCPP_CONFIGURE_ACTIVEMQCPP()
dnl
dnl  Check configure options and assign variables related to
dnl  the Apache ActiveMQ-CPP Library.
dnl

AC_DEFUN([ACTIVEMQCPP_CONFIGURE_ACTIVEMQCPP],
[
  AC_MSG_NOTICE([Apache ActiveMQ-CPP Library configuration])
  ACTIVEMQCPP_FIND_ACTIVEMQCPP([], [], 2, [2])

  if test $activemqcpp_found = "no"; then
    AC_MSG_WARN([ActiveMQ-CPP not found])
    ACTIVEMQCPP_DOWNLOAD_ACTIVEMQCPP
  fi

  dnl Get build information from ActiveMQ-CPP

  ACTIVEMQCPP_CFLAGS="`$activemqcpp_config --cflags`"
  if test $? -ne 0; then
    AC_MSG_ERROR([activemqcpp-config --cflags failed])
  fi

  ACTIVEMQCPP_INCLUDES="`$activemqcpp_config --includes`"
  if test $? -ne 0; then
    AC_MSG_ERROR([activemqcpp-config --includes failed])
  fi

  ACTIVEMQCPP_LIBS="`$activemqcpp_config --libs`"
  if test $? -ne 0; then
    AC_MSG_ERROR([activemqcpp-config --libs failed])
  fi

  AC_SUBST([ACTIVEMQCPP_LIBS])
  AC_SUBST([ACTIVEMQCPP_INCLUDES])
  AC_SUBST([ACTIVEMQCPP_CFLAGS])
])

dnl ACTIVEMQCPP_DOWNLOAD_ACTIVEMQCPP()
dnl no ActiveMQ-CPP found, print out a message telling the user what to do
AC_DEFUN([ACTIVEMQCPP_DOWNLOAD_ACTIVEMQCPP],
[
  echo "The Apache ActiveMQ-CPP library cannot be found."
  echo "Please install ActiveMQ-CPP on this system and supply the appropriate"
  echo "--with-activemqcpp option to 'configure'."
  echo "You can download ActiveMQ-CPP from http://activemq.apache.org/cms/"

  AC_MSG_ERROR([no suitable ActiveMQ-CPP Install found])
])
