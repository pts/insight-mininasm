;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; insight.asm
; Main module.
;-----------------------------------------------------------------------------
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
; 02110-1301 USA
;=============================================================================



;=============================================================================
; Constants.
;=============================================================================
%include	"insight.inc"		;Constants
%include	"COL_SCH"		;Color scheme
%include	"colors.inc"		;Color scheme
;=============================================================================


;=============================================================================
; Code.
;=============================================================================
		section	.text
		cpu	8086
		org	100h
offset_0100h:
		jmp	install

;=============================================================================
; Included modules.
;=============================================================================
%include	"iface_1.inc"		;Main module: part 1
%include	"iface_2.inc"		;Main module: part 2
%include	"block.inc"		;Read/write/copy/fill blocks

%include	"system.inc"		;Misc. procedures
%include	"keyboard.inc"		;Keyboard procedures
%include	"windows.inc"		;Windows and menus
%include	"dialogs.inc"		;Dialog windows
%include	"video.inc"		;Saving/restoring screen

%include	"inst.inc"		;
%include	"unasm.inc"		;Disassembler
%include	"asm.inc"		;Assembler
%include	"trace.inc"		;

%include	"follow.inc"		;
%include	"search.inc"		;
%include	"resident.inc"		;

%include	"tools.inc"		;

;=============================================================================
; Initialized data.
;=============================================================================
%include	"actdlgs.inc"		;Dialog windows
%include	"menus.inc"		;Menus
%include	"actdata.inc"		;Initialized variables

;=============================================================================
; Code and initialized data.
;=============================================================================
follow_stack_btm:
shared_data_area:
string_buffer:
window_buffer:

%include	"cpu.inc"		;CPU detection code
%include	"cmdline.inc"		;Command line parsing procedures
%include	"install.inc"		;Initialization code

SHARED_DATA_SIZE 	equ	$-shared_data_area

;=============================================================================
; Uninitialized data.
;=============================================================================
udata_start:

absolute	udata_start

		resb	WINDOW_BUF_SIZE - SHARED_DATA_SIZE
code_mark_buff	resw	10 * 4

%include	"actudata.inc"		;Uninitialized data

udata_end:
program_end:

;=============================================================================
; E0F
;=============================================================================

