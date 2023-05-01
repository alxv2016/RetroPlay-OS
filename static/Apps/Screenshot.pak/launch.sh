#!/bin/sh

cd "$(dirname "$0")"
a=`ps | grep screenshot | grep -v grep`
if [ "$a" == "" ] ; then
    ./screenshot &
    sys-msg "Screenshot has been enabled."$'\n'"Press L2 + R2 to take a screenshot."
	sleep 2
else
    killall screenshot
    sys-msg "Screenshot has been disabled."
	sleep 1
fi

