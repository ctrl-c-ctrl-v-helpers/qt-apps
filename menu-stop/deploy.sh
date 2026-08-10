#!/bin/bash

NSTEPS=5

if ! [ -z "$(git status --porcelain)" ]; then
	echo Cannot deploy dirty repo build.
	exit 1
fi

SRC=$( find . -name menu-stop.exe )
TXT=$( git log -1 --format=menu-stop---%cd_%s_%h---exe.txt --date=format:%Y-%m-%dT%H-%M HEAD | sed 's/ /-/g' )
DST='/c/EXE/qt-apps/menu-stop.exe'

echo "[1/$NSTEPS] Stopping menu-stop.exe:"
echo -n "      "
taskkill //f //im menu-stop.exe
echo

echo "[2/$NSTEPS] Copying sendable version:"
echo "      copy from $SRC"
echo "      copy to   $TXT"
cp $SRC $TXT
echo

echo "[3/$NSTEPS] Copying local version:"
echo "      copy from $SRC"
echo "      copy to   $DST"
cp $SRC $DST
echo

echo "[4/$NSTEPS] Starting menu-stop.exe:"
cmd.exe //c "cd /d C:\EXE\qt-apps && start menu-stop.exe"
tasklist | grep -E "Image Name|=====|menu-stop.exe" | awk '{print "      " $0}'


echo
echo "[5/$NSTEPS] DONE"