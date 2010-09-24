#!/bin/sh

BINDIR=`dirname "$0"`
BINDIR=`(cd $BINDIR ; pwd)`

JARDIR=`(cd $BINDIR/../java/lib ; pwd)`

if [ ! -e $JARDIR/semaine.jar ] ; then
  echo "No semaine.jar -- you need to do 'ant jars' in the java/ folder first\!"
  exit 1;
fi


java -cp $JARDIR/semaine.jar eu.semaine.util.MessageLogInspectorGUI $1



