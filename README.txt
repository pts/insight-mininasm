compiling the Insight real-mode DOS 16-bit debugger with mininasm
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This project is a case study of making the NASM source code of the Insight
debugger compilable with mininasm (and also remaining compilable with NASM),
with the goal of producing the same insight.com executable program with NASM
and mininasm, both identical to the file distributed. This goal has been
reached, and the commit history of this project shows the journey.

Relevant software:

* NASM: the Netwide Assembler (NASM), x86 (16-bit, 32-bit and 64-bit)
  assembler written in C: https://www.nasm.us/ .

  Program size for NASM 0.98.39 (released on 2005-01-20) is ~220 KiB for DOS
  8086, ~274 KiB for Linux i386 and ~325 KiB for Win32. (The large spread is
  because of the different integer sizes, C libraries, compilers and program
  file formats.)

* mininasm: NASM-compatible mini assembler for 8086, 186 and 286, able to
  run on DOS and on modern systems, written in C (with alternative
  implementation fork written in itself): https://github.com/pts/mininasm

  mininasm is indeed mininalistic: compiled program size is ~21 KiB for DOS
  8086 and Linux i386, and ~25 KiB for Win32.

  mininasm aims for compatibility with NASM 0.98.39 (released on 2005-01-20).

* Insight real-mode DOS 16-bit debugger, version 1.24, released on
  2009-01-11, written mostly in NASM, released under the GPL v2 license.

  Insight is a very small debugger (insight.com is 32935 bytes) for
  analyzing real-mode DOS programs. It features a 486 disassembler, an 8086
  assembler, ``Trace into'' and ``Step over;; functions, simple breakpoint
  handling, extended code or data navigation, simple color-highlighting, and
  a nice menu-driven interface comparable to Borland's Turbo Debugger.

  Project page with download including source code:

  * http://www.bttr-software.de/products/insight/
  * http://www.bttr-software.de/products/insight/insig124.zip

  The same package also available in the FreeDOS repositories, with some
  additonal FreeDOS metadata files and the original Insight 1.24 files
  (program, documentation and source), labeled as version 1.24a:

  * https://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/distributions/1.2/repos/pkg-html/insight.html
  * https://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/distributions/1.2/repos/devel/insight.zip
  * https://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/repositories/latest/pkg-html/insight.html
  * https://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/repositories/latest/devel/insight.zip

  The difference between the original release (insig124.zip) and the FreeDOS
  archive (insight.zip):

  * Different capitalization of the filenames.
  * A bit different directory layout.
  * The `appinfo' directory is only in the FreeDOS archive, because it's
    specific to FreeDOS.

* kvikdos: a very fast headless DOS emulator for Linux (with KVM):
  https://github.com/pts/kvikdos

  kvikdos can optionally be used to run the DOS 8086 host version of NASM,
  to double check that it generates the same output as the Linux host
  version.

Step 1. Download the Insight release including the source code
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
These files were downloaded:

* http://www.bttr-software.de/products/insight/insig124.zip
* https://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/repositories/latest/devel/insight.zip

Downloaded files were saved to:
https://github.com/pts/insight-mininasm/releases/tag/orig

The target executable program insight.com (32935 bytes) was saved to:
https://github.com/pts/insight-mininasm/releases/download/orig/insight.com

Source code and build scripts were extracted from the downloaded .zip
files, and saved to https://github.com/pts/insight-mininasm/releases/download/orig/insight_1.24_src.zip
, and also to this Git repository.

Step 2. Run all the build steps manually which produce input for NASM
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The DOS tools mkhelp.com and uclpack.exe were run in DOSBox to generate data/page1.xlp and
data/page2.xlp.

The files unnecessary after the generation (e.g. mkhelp.com, uclpack.exe and
their source code, data/page1.hlp and data/page2.hlp) were removed.

Step 3. Change the source code so that it builds with NASM 0.98.39
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Backslashes (`\') were replaced with slashes (`/') in the pathnames in
actdata.inc, for Linux NASM compatibility.

The original insight.com (Insight version 1.24, 32935 bytes) was added, to
facilate comparisons in the future.

The DOS makefile was replaced with Linux compilation shell script
compile.sh. NASM optimization level was updated from `-O4' (buggy in NASM
0.98.39) to `-O9' (between to makefile and the shell script).

Compilation with NASM 0.98.39 `-O9' (tried on Linux and also on DOS)
produces insight.com identical to the release.

Detection and use of local ./nasm-0.98.39.li3 executable program was added
to compile.sh.

Detection and use of kvikdos and local nasm98.exe executable program was
added to compile.sh. This is for running the DOS 8086 host version of NASM
in a DOS emulator, to double check that it generates the same output as the
Linux host version.

NASM 0.98.39 executable programs for Linux i386 (also works on Linux amd64),
Win32 (works on 32-bit and 64-bit Windows) and DOS 8086 were added to
https://github.com/pts/insight-mininasm/releases/tag/nasm-0.98.39 .

Source files (and pregenerated source files) were moved from the data/
directory to the main repository directory.

The macro SCR_WIDTH was moved from the command line to insight.asm.

The macro COL_SCH was moved from the command line to insight.asm.

Step 4. Change the source code so that it builds with mininasm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The goal is to build the insight.com identical to the release.

__END__
