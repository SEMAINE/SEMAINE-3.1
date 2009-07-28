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
  CONFIG=$CONFDIR/test-participantcontrol.config
fi

JMS_URL_SETTING=""
if [ -n "$CMS_URL" ] ; then
  echo "Connecting to JMS server at $CMS_URL"
  JMS_URL_SETTING="-Djms.url=$CMS_URL"
fi

echo "Starting SEMAINE ComponentRunner as: 'java $JMS_URL_SETTING -jar $JARDIR/semaine.jar $CONFIG'"

java $JMS_URL_SETTING -jar $JARDIR/semaine.jar $CONFIG

