#!/usr/bin/env bash

if [ -e snapshot_version ] ; then
	VER="$(cat snapshot_version)"
else

git rev-list HEAD | sort > config.git-hash
LOCALVER=`wc -l config.git-hash | awk '{print $1}'`
if [ $LOCALVER -gt 0 ] ; then
	VER=`git rev-list origin/master | sort | join config.git-hash - | wc -l | awk '{print $1}'`
	if [ $VER != $LOCALVER ] ; then
		VER="$VER+$(($LOCALVER-$VER))"
	fi
	if git status | grep -q "modified:" ; then
		VER="${VER}M"
	fi
	VER=" r$VER $(git rev-list HEAD -n 1)"
fi
rm -f config.git-hash

fi

echo "$VER"
