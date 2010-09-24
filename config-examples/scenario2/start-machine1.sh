#!/bin/sh
# This assumes that activemq is on the PATH; adapt call to activemq if necessary.

SEMAINEDIR=`dirname "$0"`
SEMAINEDIR=`(cd $SEMAINEDIR ; pwd)`

# Which ActiveMQ to connect to:
export CMS_URL=tcp://192.168.1.10:61616

activemq &

$SEMAINEDIR/bin/semaine-speech2face.sh &

$SEMAINEDIR/bin/run_components/start_component_tum.opensmile &

