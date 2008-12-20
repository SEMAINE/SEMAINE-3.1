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
DISTFILELIST="$TMPDIR/distfiles-java.txt"
if [ -x "$DISTFILELIST" ] ; then
  rm "$DISTFILELIST"
fi

# Prepare the actual list of files from $FILELIST:
# prepend $INST_ROOTDIR in front of each file, 
# and expand directories.
(cd "$TMPDIR" 
for f in `cat "$FILELIST"` ; do
  if [ -d "$INST_ROOTDIR/$f" ] ; then
    find "$INST_ROOTDIR/$f" -print | egrep -v "^.svn|/.svn|~$" >> "$DISTFILELIST"
  elif [ -f "$INST_ROOTDIR/$f" ] ; then
    echo "$INST_ROOTDIR/$f" >> "$DISTFILELIST"
  else
    echo "Warning: $ROOTDIR/$f does not exist."
  fi
done

 cat "$DISTFILELIST" | zip -q "$DISTFILE" -@
 )
 echo "Created $DISTFILE"

