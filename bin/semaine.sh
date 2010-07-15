#!/bin/sh

BINDIR=`dirname "$0"`
BINDIR=`(cd "$BINDIR" ; pwd)`

# Change the following to set any custom class path:
MYCLASSPATH="$BINDIR/bin"

SEMAINE_JAVA_DIR=`(cd "$BINDIR/../../SEMAINE-2.0" ; pwd)`
MARYDIR="$SEMAINE_JAVA_DIR/MARY"
if [ ! -e $MARYDIR ] ; then  echo "MARY directory not found in $MARYDIR. Cannot start."  exit 1
fi
JARDIR="$SEMAINE_JAVA_DIR/java/lib"


CONFIG=$1
if [ -z "$CONFIG" ] ; then
  echo "No config file given! Cannot start."
  exit 1
fi

JMS_URL_SETTING=""
if [ -n "$CMS_URL" ] ; then
  echo "Connecting to JMS server at $CMS_URL"
  JMS_URL_SETTING="-Djms.url=$CMS_URL"
fi

ARGS="-Xmx1g $JMS_URL_SETTING -classpath \"$MYCLASSPATH:$JARDIR/semaine.jar:$JARDIR/semaine-mary.jar:$JARDIR/semaine-dialogue.jar\" -Dmary.base=\"$MARYDIR\" eu.semaine.system.ComponentRunner \"$CONFIG\""

echo "Starting SEMAINE system as:"
echo "java $ARGS" 

# Eval makes sure the quotes in ARGS are treated as shell quotes rather than passing that on to the java process:
eval java $ARGS



