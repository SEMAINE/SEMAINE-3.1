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
dnl find_activemqcpp.m4 : locate the ActiveMQ-CPP include files and libraries
dnl
dnl This macro file can be used by applications to find and use the ActiveMQ-CPP
dnl library. It provides a standardized mechanism for using ActiveMQ-CPP.
dnl It supports locating an installed copy of ActiveMQ-CPP.
dnl
dnl ACTIVEMQCPP_FIND_ACTIVEMQCPP(srcdir, builddir, implicit-install-check, acceptable-majors)
dnl
dnl   where srcdir is the location of the bundled ActiveMQ-CPP source directory, or
dnl   empty if source is not bundled.
dnl
dnl   where builddir is the location where the bundled ActiveMQ-CPP will will be built,
dnl   or empty if the build will occur in the srcdir.
dnl
dnl   where implicit-install-check set to 1 indicates if there is no
dnl   --with-activemqcpp option specified, we will look for installed copies.
dnl
dnl   where acceptable-majors is a space separated list of acceptable major
dnl   version numbers. Often only a single major version will be acceptable.
dnl   If multiple versions are specified, and --with-activemqcpp=PREFIX or the
dnl   implicit installed search are used, then the first (leftmost) version
dnl   in the list that is found will be used.  Currently defaults to [0 1].
dnl
dnl Sets the following variables on exit:
dnl
dnl   activemqcpp_found : "yes", "no"
dnl
dnl   activemqcpp_config : If the activemqcpp-config tool exists, this refers to it.
dnl
dnl Note: this macro file assumes that activemqcpp-config has been installed; it
dnl       is normally considered a required part of an ActiveMQ-CPP installation.
dnl
dnl If activemqcpp_found is "yes", then the caller should use the
dnl value of activemqcpp_config to fetch any necessary build/link information.
dnl

AC_DEFUN([ACTIVEMQCPP_FIND_ACTIVEMQCPP], [
  activemqcpp_found="no"

  if test "$target_os" = "os2-emx"; then
    # Scripts don't pass test -x on OS/2
    TEST_X="test -f"
  else
    TEST_X="test -x"
  fi

  ifelse([$4], [], [
         ifdef(AC_WARNING,AC_WARNING([$0: missing argument 4 (acceptable-majors): Defaulting to ACTIVEMQCPP 0.x then ACTIVEMQCPP 1.x]))
         acceptable_majors="0 1"],
         [acceptable_majors="$4"])

  activemqcpp_temp_acceptable_activemqcpp_config="activemqcpp-config"

  AC_MSG_CHECKING(for ActiveMQ-CPP)
  AC_ARG_WITH(activemqcpp,
  [  --with-activemqcpp=PATH         prefix for installed ActiveMQ-CPP, path to ActiveMQ-CPP build tree,
                               or the full path to activemqcpp-config],
  [
    if test "$withval" = "no" || test "$withval" = "yes"; then
      AC_MSG_ERROR([--with-activemqcpp requires a directory or file to be provided])
    fi

    for activemqcpp_temp_activemqcpp_config_file in $activemqcpp_temp_acceptable_activemqcpp_config
    do
      for lookdir in "$withval/bin" "$withval"
      do
        if $TEST_X "$lookdir/$activemqcpp_temp_activemqcpp_config_file"; then
          activemqcpp_found="yes"
          activemqcpp_config="$lookdir/$activemqcpp_temp_activemqcpp_config_file"
          break 2
        fi
      done
    done

    if test "$activemqcpp_found" != "yes" && $TEST_X "$withval" && $withval --help > /dev/null 2>&1 ; then
      activemqcpp_found="yes"
      activemqcpp_config="$withval"
    fi

    dnl if --with-activemqcpp is used, it is a fatal error for its argument
    dnl to be invalid
    if test "$activemqcpp_found" != "yes"; then
      AC_MSG_ERROR([the --with-activemqcpp parameter is incorrect. It must specify an install prefix, a build directory, or an activemqcpp-config file.])
    fi
  ],[
    dnl If we allow installed copies, check those before using bundled copy.
    if test -n "$3" && test "$3" = "1"; then
      for activemqcpp_temp_activemqcpp_config_file in $activemqcpp_temp_acceptable_activemqcpp_config
      do
        if $activemqcpp_temp_activemqcpp_config_file --help > /dev/null 2>&1 ; then
          activemqcpp_found="yes"
          activemqcpp_config="$activemqcpp_temp_activemqcpp_config_file"
          break
        else
          dnl look in some standard places
          for lookdir in /usr /usr/local /usr/local/activemqcpp /opt/activemqcpp; do
            if $TEST_X "$lookdir/bin/$activemqcpp_temp_activemqcpp_config_file"; then
              activemqcpp_found="yes"
              activemqcpp_config="$lookdir/bin/$activemqcpp_temp_activemqcpp_config_file"
              break 2
            fi
          done
        fi
      done
    fi
  ])

  AC_MSG_RESULT($activemqcpp_found)
])
