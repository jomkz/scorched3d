#!/bin/sh -x
aclocal
automake --foreign
autoconf
./configure $*
