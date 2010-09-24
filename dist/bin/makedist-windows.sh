#!/bin/sh

SCRIPTDIR=`dirname $0`
ROOTDIR=`(cd $SCRIPTDIR/../.. ; pwd)`
VERSION=`cat VERSION.txt | sed 's/^ *//;s/ *$//'` # make sure to trim whitespace

INST_ROOTDIR="SEMAINE-$VERSION"

FILELIST="$ROOTDIR/dist/releasefiles-windows.txt"
mkdir -p "$ROOTDIR/dist/$INST_ROOTDIR"
DISTFILE="$ROOTDIR/dist/$INST_ROOTDIR/$INST_ROOTDIR-windows.zip"

TMPDIR="$ROOTDIR/dist/tmp"
mkdir -p "$TMPDIR"
if [ -x "$TMPDIR/$INST_ROOTDIR" ] ; then
  rm "$TMPDIR/$INST_ROOTDIR"
fi
ln -s "$ROOTDIR" "$TMPDIR/$INST_ROOTDIR"
DISTFILELIST="$TMPDIR/distfiles-windows.txt"
if [ -e "$DISTFILELIST" ] ; then
  rm "$DISTFILELIST"
fi

# Prepare the actual list of files from $FILELIST:
# prepend $INST_ROOTDIR in front of each file, 
# and expand directories.

# In argument of for loop, split at newline only,
# in order to allow file names including spaces:
OLDIFS="$IFS"
IFS='
'

(cd "$TMPDIR" 
for f in `cat "$FILELIST"` ; do
  if [ -d "$INST_ROOTDIR/$f" ] ; then
    find "$INST_ROOTDIR/$f" -print | egrep -v "^\.svn|/\.svn|~$|/\.libs|\.o$|\.lo$|\.loT$|.DS_Store|/\.deps|\.a$|/\.dirstamp$" >> "$DISTFILELIST"
  elif [ -f "$INST_ROOTDIR/$f" ] ; then
    echo "$INST_ROOTDIR/$f" >> "$DISTFILELIST"
  else
    echo "Warning: $ROOTDIR/$f does not exist."
  fi
done

cat "$DISTFILELIST" | zip -q "$DISTFILE" -@
 
)

IFS="$OLDIFS"

echo "Created $DISTFILE"

