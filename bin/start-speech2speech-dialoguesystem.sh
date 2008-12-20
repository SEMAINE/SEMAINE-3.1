#!/bin/sh

BINDIR=`dirname "$0"`
BINDIR=`(cd $BINDIR ; pwd)`

JARDIR=`(cd $BINDIR/../java/lib ; pwd)`

if [ $# -ne 2 ]
then
    echo "Usage: ./bin/start-speech2speech-dialoguesystem.sh java/config/Speech2Speech-system.config MARY_INST_PATH"
    exit 1;
fi

if [ ! -e $JARDIR/semaine.jar ] ; then
  echo "No semaine.jar -- you need to do 'ant jars' in the java/ folder first\!"
  exit 1;
fi

CONFIG=$1
if [ -z "$CONFIG" ] ; then
  CONFDIR=`(cd $BINDIR/../java/config ; pwd)`
  CONFIG=$CONFDIR/Speech2Speech-system.config
fi

MARY_BASE=$2
if [ -z "$MARY_BASE" ] ; then
  echo "Usage: ./bin/start-speech2speech-dialoguesystem.sh java/config/Speech2Speech-system.config MARY_INST_PATH"
  exit 1;
fi

SERVER=134.96.187.56

echo "Starting SEMAINE ComponentRunner :" # as: 'java -jar $JARDIR/semaine.jar $CONFIG'"


java -Xmx1024m -Djms.url=tcp://$SERVER:61616 -classpath $JARDIR/semaine.jar:$JARDIR/semaine-mary.jar:$JARDIR/semaine-dialogue.jar -Dmary.base="$MARY_BASE" eu.semaine.system.ComponentRunner $CONFIG 



