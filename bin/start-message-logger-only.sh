#!/bin/sh

BINDIR=`dirname "$0"`
BINDIR=`(cd $BINDIR ; pwd)`

JARDIR=`(cd $BINDIR/../java/lib ; pwd)`

if [ ! -e $JARDIR/semaine.jar ] ; then
  echo "No semaine.jar -- you need to do 'ant jars' in the java/ folder first\!"
  exit 1;
fi

CONFIG=$1
if [ -z "$CONFIG" ] ; then
  CONFDIR=`(cd $BINDIR/../java/config ; pwd)`
  CONFIG=$CONFDIR/message-logger-only.config
fi

echo "Starting SEMAINE ComponentRunner as: 'java -jar $JARDIR/semaine.jar $CONFIG'"

java -jar $JARDIR/semaine.jar $CONFIG


