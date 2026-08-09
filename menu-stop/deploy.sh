#!/bin/bash

if ! [ -z "$(git status --porcelain)" ]; then
	echo Cannot deploy dirty repo build.
	exit 1
fi

SRC=$( find . -name menu-stop.exe )
TXT=$( git log -1 --format=menu-stop---%cd_%s_%h---exe.txt --date=format:%Y-%m-%dT%H-%M HEAD | sed 's/ /-/g' )
DST='/c/EXE/qt-apps/menu-stop.exe'

echo cp $SRC $TXT
cp -v $SRC $TXT

echo
echo cp $SRC $DST
cp -v $SRC $DST

echo
echo DONE