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
dnl  dependance on the XQilla Library.
dnl
dnl  XQILLA_SUPPORT()
dnl
dnl  Check configure options and assign variables related to
dnl  the XQilla Library.
dnl

AC_DEFUN([XQILLA_SUPPORT],
[
  AC_MSG_NOTICE([XQilla XPath Library configuration])


    AC_ARG_WITH([xqilla],
        AC_HELP_STRING([--with-xqilla=@<:@ARG@:>@],
            [use XQilla XPath library from given prefix (ARG=path); check standard prefixes (ARG=yes); disable (ARG=no)]
        ),
        [
        if test "$withval" = "yes"; then
            if test -d /usr/local/include/xqilla ; then
                xqilla_prefix=/usr/local
            elif test -d /usr/include/xqilla ; then
                xqilla_prefix=/usr
            else
                xqilla_prefix=""
            fi
            xqilla_requested="yes"
        elif test -d "$withval"; then
            xqilla_prefix="$withval"
            xqilla_requested="yes"
        else
            xqilla_prefix=""
            xqilla_requested="no"
        fi
        ],
        [
        dnl Default behavior is implicit yes
        if test -d /usr/local/include/xqilla ; then
            xqilla_prefix=/usr/local
        elif test -d /usr/include/xqilla ; then
            xqilla_prefix=/usr
        else
            xqilla_prefix=""
        fi
        ]
    )

    XQILLA_CPPFLAGS=""
    XQILLA_LDFLAGS=""
    XQILLA_VERSION=""

    dnl
    dnl Collect include/lib paths and flags
    dnl
    run_xqilla_test="no"

    if test -n "$xqilla_prefix"; then
        xqilla_include_dir="$xqilla_prefix/include"
        xqilla_ldflags="-L$xqilla_prefix/lib"
        run_xqilla_test="yes"
    elif test "$xqilla_requested" = "yes"; then
        if test -n "$xqilla_include_dir" -a -n "$xqilla_lib_flags"; then
            run_xqilla_test="yes"
        fi
    else
        run_xqilla_test="no"
    fi

    xqilla_libs="-lxqilla"

    dnl
    dnl Check XQilla files
    dnl
    if test "$run_xqilla_test" = "yes"; then

        saved_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS -I$xqilla_include_dir $XERCES_CPPFLAGS"

        saved_LDFLAGS="$LDFLAGS"
        LDFLAGS="$LDFLAGS $xqilla_ldflags $XERCES_LDFLAGS"

        saved_LIBS="$LIBS"
        LIBS="$xqilla_libs $LIBS $XERCES_LIBS"

        dnl
        dnl Check XQilla headers
        dnl
        AC_MSG_CHECKING([for XQilla headers in $xqilla_include_dir])

        AC_LANG_PUSH([C++])
        AC_COMPILE_IFELSE([
            AC_LANG_PROGRAM(
                [[
@%:@include <xqilla/utils/XQillaPlatformUtils.hpp>
                ]],
                [[]]
            )],
            [
            XQILLA_CPPFLAGS="-I$xqilla_include_dir"
            xqilla_header_found="yes"
            AC_MSG_RESULT([found])
            ],
            [
            xqilla_header_found="no"
            AC_MSG_RESULT([not found])
            ]
        )
        AC_LANG_POP([C++])

        dnl
        dnl Check XQilla libraries
        dnl
        if test "$xqilla_header_found" = "yes"; then

            AC_MSG_CHECKING([for XQilla libraries])

            AC_LANG_PUSH([C++])
            AC_LINK_IFELSE([
                AC_LANG_PROGRAM(
                    [[
@%:@include <xqilla/utils/XQillaPlatformUtils.hpp>
                    ]],
                    [[
XQillaPlatformUtils::initialize();
                    ]]
                )],
                [
                XQILLA_LDFLAGS="$xqilla_ldflags"
                XQILLA_LIBS="$xqilla_libs"
                xqilla_lib_found="yes"
                AC_MSG_RESULT([found])
                ],
                [
                xqilla_lib_found="no"
                AC_MSG_RESULT([not found])
                ]
            )
            AC_LANG_POP([C++])
        fi

        CPPFLAGS="$saved_CPPFLAGS"
        LDFLAGS="$saved_LDFLAGS"
        LIBS="$saved_LIBS"
    fi

    AC_MSG_CHECKING([for XQilla])

    if test "$run_xqilla_test" = "yes"; then
        if test "$xqilla_header_found" = "yes" -a "$xqilla_lib_found" = "yes"; then

            AC_SUBST([XQILLA_CPPFLAGS])
            AC_SUBST([XQILLA_LDFLAGS])
            AC_SUBST([XQILLA_LIBS])

            HAVE_XQILLA="yes"
        else
            HAVE_XQILLA="no"
        fi
	fi
    AC_MSG_RESULT([$HAVE_XQILLA])



	if test "$HAVE_XQILLA" != "yes"; then
	    AC_MSG_WARN([XQilla not found])
    	XQILLA_DOWNLOAD
	fi






])



dnl XQILLA_DOWNLOAD()
dnl no XQilla found, print out a message telling the user what to do
AC_DEFUN([XQILLA_DOWNLOAD],
[
  echo "The XQilla library cannot be found."
  echo "Please install XQilla on this system and supply the appropriate"
  echo "--with-xqilla option to 'configure'."
  echo "You can download XQilla from http://xqilla.sourceforge.net/"

  AC_MSG_ERROR([no suitable XQilla Install found])
])
