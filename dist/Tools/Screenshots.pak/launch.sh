#!/bin/sh

cd "$(dirname "$0")"
a=`ps | grep screenshot | grep -v grep`
if [ "$a" == "" ] ; then
    ./screenshot &
    sys-msg "Enabled screenshots"$'\n'"Press L2 + R2 to take"
	sleep 2
else
    killall screenshot
    sys-msg "Disabled screenshots"
	sleep 1
fi

