#! /bin/sh --
#
# Insight 1.24 compilation script for Linux
# by pts@fazekas.hu at Wed Dec 14 23:50:57 CET 2022
#
set -ex

# https://github.com/pts/insight-mininasm/releases/download/nasm-0.98.39/nasm-0.98.39.li3
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

# https://github.com/pts/mininasm/releases/download/v6/mininasm.li3
if test -f mininasm.li3; then
  if ! test -x mininasm.li3; then
    chmod +x mininasm.li3
  fi
  MININASM=./mininasm.li3
elif type mininasm >/dev/null 2>&1; then
  MININASM=mininasm
else
  MININASM=
fi

test -d bin || mkdir bin

"$NASM" -O9 -w+orphan-labels -f bin -o bin/insight.com insight.asm
cmp orig/insight.com bin/insight.com

if test "$MININASM"; then
  "$MININASM" -O9 -w+orphan-labels -f bin -o bin/insightm.com insight.asm
  cmp orig/insight.com bin/insightm.com
fi

# https://github.com/pts/kvikdos
# https://github.com/pts/insight-mininasm/releases/download/nasm-0.98.39/nasm98.exe
if type kvikdos >/dev/null 2>&1 && test -f nasm98.exe; then
  kvikdos nasm98.exe -O9 -w+orphan-labels -f bin -o bin/insightd.com insight.asm
  cmp orig/insight.com bin/insightd.com
fi

# https://github.com/pts/mininasm/releases/download/v6/mininasm.com
if type kvikdos >/dev/null 2>&1 && test -f mininasm.com; then
  kvikdos mininasm.com -O9 -f bin -o bin/insighte.com insight.asm
  cmp orig/insight.com bin/insighte.com
fi

: "$0" OK.
