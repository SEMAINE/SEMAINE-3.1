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
dnl  dependance on the Xerces Library.
dnl
dnl  XERCES_CONFIGURE_XERCES()
dnl
dnl  Check configure options and assign variables related to
dnl  the XERCES Library.
dnl

AC_DEFUN([XERCES_CONFIGURE_XERCES],
[
  AX_LIB_XERCES

  if test $HAVE_XERCES = "no" ; then
    XERCES_DOWNLOAD_XERCES
  fi
])

dnl XERCES_DOWNLOAD_XERCES()
dnl no Xerces-C found, print out a message telling the user what to do
AC_DEFUN([XERCES_DOWNLOAD_XERCES],
[
  echo "The Apache Xerces-C library cannot be found."
  echo "Please install Xerces-C on this system and supply the appropriate"
  echo "--with-xerces option to 'configure'."
  echo "You can download Xerces-C from http://xerces.apache.org/xerces-c/"

  AC_MSG_ERROR([no suitable Xerces-C install found])
])
