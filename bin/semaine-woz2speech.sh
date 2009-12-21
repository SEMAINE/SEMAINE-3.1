#!/bin/sh

BINDIR=`dirname "$0"`
BINDIR=`(cd $BINDIR ; pwd)`

MARYDIR="$BINDIR/../MARY"

if [ ! -e $MARYDIR ] ; then
  echo "MARY directory not found in $MARYDIR. Cannot start."
  exit 1
fi

JARDIR=`(cd $BINDIR/../java/lib ; pwd)`

if [ ! -e $JARDIR/semaine.jar ] ; then
  echo "No semaine.jar -- you need to do 'ant jars' in the java/ folder first\!"
  exit 1
fi

CONFIG=$1
if [ -z "$CONFIG" ] ; then
  CONFDIR=`(cd $BINDIR/../java/config ; pwd)`
  CONFIG=$CONFDIR/woz2speech.config
fi

JMS_URL_SETTING=""
if [ -n "$CMS_URL" ] ; then
  echo "Connecting to JMS server at $CMS_URL"
  JMS_URL_SETTING="-Djms.url=$CMS_URL"
fi

echo "Starting SEMAINE system as:"
echo "java -Xmx1g $JMS_URL_SETTING -classpath $JARDIR/semaine.jar:$JARDIR/semaine-mary.jar:$JARDIR/semaine-dialogue.jar -Dmary.base="$MARYDIR" eu.semaine.system.ComponentRunner $CONFIG" 

java -Xmx1g $JMS_URL_SETTING -classpath $JARDIR/semaine.jar:$JARDIR/semaine-mary.jar:$JARDIR/semaine-dialogue.jar -Dmary.base="$MARYDIR" eu.semaine.system.ComponentRunner $CONFIG 



