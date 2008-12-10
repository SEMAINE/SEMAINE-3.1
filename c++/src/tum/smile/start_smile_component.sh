#!/bin/sh

if test "x$1" = "x" ; then
  models="$PWD/models"
else
  models="$1/models"
  shift
fi

# which models to use for arousal and valence 
# (sub-dirs in models directory)
corpusA="sal";
corpusV="sal";

P=`cat ../asr/asr.cfg | grep PAUDIO`;
if test "x$P" != "x"; then

rm -f /tmp/semaine.tum.pipe.raw
mkfifo /tmp/semaine.tum.pipe.raw
if test "x$?" != "x0" ; then
  echo "ERROR creating pipe... smile component will not pipe audio data!"
  PIPE=""
else
  PIPE="-pipeaudio /tmp/semaine.tum.pipe.raw"
fi

else
  PIPE=""
fi

./featumamq $PIPE -silthresh 0.001 -svmmodelV $models/$corpusV/valence.model -svmmodelA $models/$corpusA/arousal.model -sildet -svmscaleA $models/$corpusA/arousal.scale -svmscaleV $models/$corpusV/valence.scale -svmpredfselA $models/$corpusA/arousal.fsel -svmpredfselV $models/$corpusV/valence.fsel $*

rm -f /tmp/semaine.tum.pipe.raw 2>/dev/null

exit $?

