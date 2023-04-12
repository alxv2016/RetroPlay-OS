#!/bin/sh
progdir=`cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P`

"$progdir/setup-env.sh"

wget https://www.sqlite.org/2022/sqlite-autoconf-3390000.tar.gz
tar xvzf sqlite-autoconf-3390000.tar.gz

cd sqlite-autoconf-3390000/
./configure --host=arm-linux --prefix=$PREFIX CC=$(echo $CROSS_COMPILE)gcc
make
make install
