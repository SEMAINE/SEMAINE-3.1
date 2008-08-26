#!/bin/sh

BINDIR=`dirname "$0"`

JARDIR=$BINDIR/../java/lib
CONFIG=$1

if [ ! -e $JARDIR/semaine.jar ] ; then
  echo "No semaine.jar -- you need to do 'ant jars' in the java/ folder first\!"
  exit 1;
fi

if [ -n $CONFIG ] ; then
  CONFIG=$BINDIR/../java/config/dummy-system.config
fi

echo "Starting SEMAINE ComponentRunner as: 'java -jar $JARDIR/semaine.jar $CONFIG'"

java -jar $JARDIR/semaine.jar $CONFIG


