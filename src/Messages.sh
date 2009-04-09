#! /usr/bin/env bash
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$XGETTEXT *.cpp -o $podir/mplayerthumbs.pot
rm -f rc.cpp
