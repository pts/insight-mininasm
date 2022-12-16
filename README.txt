compiling the Insight real-mode DOS 16-bit debugger with mininasm
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This project is a case study of porting NASM assembly source code to
mininasm, more specifically making the NASM source code of the Insight
debugger compilable with mininasm (and also remaining compilable with NASM),
with the goal of producing the same insight.com executable program with NASM
and mininasm, both identical to the file officially released.

This goal has been reached, the commit history of this project shows the
journey, and the remaining of this README explains the steps needed and
their reasons.

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

  The mininasm executable programs that are able to compile the modified and
  simplified Insight source code can be downloaded from
  https://github.com/pts/mininasm/releases/tag/v6

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

The now unused source files data/colors_b.inc and data/colors_c.inc were
removed.

Step 4. Change the source code so that it builds with mininasm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Local labels with prefix `@@' were renamed to prefix `.'. This made it
possible to drop the `-t' command-line switch of NASM, which mininasm
doesn't support. A small Perl scipt was used to automate this:

  $ perl -pi -e 's/[@][@]/[.]/g' *.asm *.inc

Source code of non-8086 code was changed:

* Very little part of Insight optionally takes advantage of CPU features 386
  and 586 (Pentium) processors, and it has corresponding blocks of assembly
  code under `cpu 386' and `cpu 586', something like this:

    cpu 386
    pushfd  ; db 0x66, 0x99
    pop eax  ; db 0x66, 0x58
    mov ecx, eax  ; db 0x66, 0x98, 0xc1
    cpu 8086

  Please note that Insight doesn't switch to 32-bit protected mode (`bits
  32'), so the `db 0x66' prefix is still used with these instructions.

  The example above was replaced with:

    db 0x66
    pushf  ; db 0x99
    db 0x66
    pop ax  ; 0x58
    db 0x66
    mov cx, ax  ; db 0x98, 0xc1

  All these instances were replaced manually, looking at the output of
  `ndisasm -b 16' for the actual `db' values.

  Most of the `db' values were instances of the 386 size prefix `db 0x66',
  and there was also the 586 `cpuid' instruction `db 0x0f, 0xa2'.

Multi-line macro `reg_pos' was replaced half-manually (using a short, costom
Perl script) with its expansions. That was needed because mininasm doesn't
support multi-line macros.

Single-line macros `BUILD' and `SERIAL_NUMBER' were manually replaced with
their string literal expansions. That was needed because mininasm supports
only integer-valued single-line macros.

Single-line macros in colors_k.inc were replaced with `equ' labels:

* Example:

    %define MAIN_BG BLACK ; Old, doesn't work in mininasm.
    MAIN_BG equ BLACK     ; New, works in mininasm
    %assign MAIN_BG BLACK ; This would also work in mininasm.

* That was necessary, because in mininasm, the value of a single-line macro
  defined with `%define' must be an integer literal (possibly prefixed with
  unary operators). This restriction is there because single-line macro
  facilities in mininasm are very limited, and they are implemented using
  (multiple) labels, and this restriction is only sane way to prevent
  deviation from NASM macro expansion semantics.

The empty value of the single-line macro BORLAND_MENU was replaced with the
value `1', because of the above restriction on `%define'.

Moved the non-integer part of the single-line macro `follow_stack' to the
expansion locations:

* Original:

    %define PaletteBuffer (window_buffer + STRING_BUF_SIZE)
    %define follow_stack PaletteBuffer
    cmp si, follow_stack  ; A few more like this in the source.

* Replacement:

    follow_stack_mmm equ window_buffer + STRING_BUF_SIZE
    cmp si, follow_stack_mmm

* The reason is the above restriction on `%define'.

Similarly to `follow_stack', the `+buffer' part of the macro values of
`CMD_X' and `OPER_X' was moved to the expansion locations, because of the
above restriction on `%define'.

The non-integer-valued single-line macros `jmpn' and `jmps' (convenience
shorthand for `jmp short') were repleaced with their expansions, because of
the above restrictions on `%define'.

Operator `==' in `%if' expressions was replaced with `-':

* Example original:

    %if SCR_WIDTH==80
      ...A
    %else
      ...B
    %endif

* Replacement:

    %if SCR_WIDTH-80  ; True if not equals.
      ...B
    %else
      ...A
    %endif

* That was necessary because mininasm doesn't support comparison or logical
  operators in `%if' expressions.

Default code when SCR_WIDTH has an unsupported value was removed.

The multi-line macros `save_act_regs', `restore_act_regs', `save_user_regs'
and `restore_user_regs' were manually replaced with their expansions. That
was necessary because mininasm doesn't support multi-line macros. The
actual manual replacement was quick and straightforward, because these were
simple macros without arguments, and there were only a few expansions.

The unnecessary `section .text' directive, which is not supported by
mininasm, was removed.

The `absolute udata_start' directive, was replaced with `absolute $',
because mininasm supports `absolute' only with `$'. Actually, this was one
of the few instances where esupport for a feature (`absolute $' this time)
was added to mininasm during this effort (of porting Insight to mininasm).

The source code of Insight modified as above compiles with mininasm.

Compilation with mininasm (and comparison of the output with the original
insight.com) was added to compile.sh.

During these steps several bugs were discovered and fixed in mininasm. Some
of the more interesting ones:

1. mininasm was using the global variable `global_label' for two different
   purposes, thus using local labels from a local label definition
   didn't work. First a workaround was implemented in
   https://github.com/pts/mininasm/commit/1312306f70a983a2c90f2945c15f561fe9289215
   which makes it an explicit failure to use local labels. Then the bug was
   fixed in
   https://github.com/pts/mininasm/commit/cc9afbe5ec38b69e30b61527496466d0f80b3ec7
   , which actually made the code shorter. This bugfix involved adding a
   helper function named strcatcmp (my_strcatcmp_far), which compares the
   concatenation of strings S1A and S1B to the string S2. By using this
   unusual comparison, S1A and S1B doesn't have to be concatenated in a
   temporary buffer (used to be `global_label'), but they can be used in
   their original location.

2. There was a bug in the convergence detection of the output size
   optimizer. mininasm reads the same input file in multiple pass, each time
   using the label values from the previous pass in forward-referenced
   labels. This if there are many unrelated size changes between the
   previous pass and the current pass before the current jump, then the
   jmp target of a strict short relative jump (8-bit signed delta, such as
   the `jcxz' instruction), may temporarily appear as if it doesn't fit the
   8 bits. In subsequent passes it would fit, but the buggy mininasm aborted
   too early, in the first pass where it didn't fit. The bug was fixed by
   moving these 8-bit jump target delte error reports to the last pass, like
   this:
   https://github.com/pts/mininasm/commit/a368ab6bc6854b154604d44cad11f39698a82522
   . A unit test was also added with more explanation.

   Discovering and fixing such bugs were one of the primary reasons for
   choosing a large program to be ported to mininasm. By using only
   boot-sector-size (at most 512 byte) test programs this bug wouldn't have
   been discovered.

   See
   https://retrocomputing.stackexchange.com/questions/25808/looking-for-an-open-source-dos-com-program-written-in-assembly
   for the quest of finding programs to port to mininasm.

   The first large test program to be ported to mininasm was
   minnnasm (with 3 `n's in the middle):
   https://github.com/pts/mininasm/blob/master/minnnasm.nasm , a port of
   mininasm itself, the DOS 8086 executable program minnnasm.com being ~15
   KiB at the time (now, with the new features and the bugfixes, it's closer
   to 21 KiB). It uncovered not only minnnasm bugs, but also quirks of NASM
   0.98.39, most of which mininasm now carefully reproduces by default.
   However, it was unlikely that minnnasm leads to discovery of many
   surprising bugs, because its source code was generated by the OpenWatcom
   C compiler, and thus its use of instructions and directives was very
   limited compared to assembly programs written organically.

__END__
