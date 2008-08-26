#!/bin/sh

BINDIR=`dirname "$0"`

JARDIR=$BINDIR/../java/lib

if [ ! -e $JARDIR/semaine.jar ] ; then
  echo "No semaine.jar -- you need to do 'ant jars' in the java/ folder first\!"
  exit 1;
fi

echo "Starting SEMAINE LogReader as: 'java -cp $JARDIR/semaine.jar eu.semaine.jms.JMSLogReader $1 $2'"

java -cp $JARDIR/semaine.jar eu.semaine.jms.JMSLogReader $1 $2



