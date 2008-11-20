#!/bin/sh

if test "x$1" = "x" ; then
  models="$PWD/models"
else
  models="$1/models"
fi

# which models to use for arousal and valence 
# (sub-dirs in models directory)
corpusA="sal";
corpusV="sal";

./featumamq -silthresh 0.001 -svmmodelV $models/$corpusV/valence.model -svmmodelA $models/$corpusA/arousal.model -sildet -svmscaleA $models/$corpusA/arousal.scale -svmscaleV $models/$corpusV/valence.scale -svmpredfselA $models/$corpusA/arousal.fsel -svmpredfselV $models/$corpusV/valence.fsel $*

exit $?

