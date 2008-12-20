#!/bin/sh

SCRIPTDIR=`dirname $0`
ROOTDIR=`(cd $SCRIPTDIR/../.. ; pwd)`
VERSION=`cat VERSION.txt | sed 's/^ *//;s/ *$//'` # make sure to trim whitespace

INST_ROOTDIR="SEMAINE-$VERSION"

FILELIST="$ROOTDIR/dist/releasefiles-java.txt"
mkdir -p "$ROOTDIR/dist/$INST_ROOTDIR"
DISTFILE="$ROOTDIR/dist/$INST_ROOTDIR/$INST_ROOTDIR-java.zip"

TMPDIR="$ROOTDIR/dist/tmp"
mkdir -p "$TMPDIR"
if [ -x "$TMPDIR/$INST_ROOTDIR" ] ; then
  rm "$TMPDIR/$INST_ROOTDIR"
fi
ln -s "$ROOTDIR" "$TMPDIR/$INST_ROOTDIR"

# Now, prepend $INST_ROOTDIR in front of each file in $FILELIST, and ask zip to include it:
(cd "$TMPDIR" ; cat "$FILELIST" | awk "{ print \"$INST_ROOTDIR/\"\$1; }" | zip -r "$DISTFILE" -@)

