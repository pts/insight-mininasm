#! /bin/sh --
#
# Insight 1.24 compilation script for Linux
# by pts@fazekas.hu at Wed Dec 14 23:50:57 CET 2022
#
set -ex

if test -f nasm-0.98.39.li3; then
  if ! test -x nasm-0.98.39.li3; then
    chmod +x nasm-0.98.39.li3
  fi
  NASM=./nasm-0.98.39.li3
elif type nasm-0.98.39 >/dev/null 2>&1; then
  NASM=nasm-0.98.39
else
  NASM=nasm
fi

test -d bin || mkdir bin
"$NASM" -t -O9 -w+orphan-labels -f bin -DSCR_WIDTH=80 -DCOL_SCH=data/colors_k.inc -o bin/insight.com insight.asm
cmp orig/insight.com bin/insight.com

: "$0" OK.
