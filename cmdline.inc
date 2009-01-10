;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; cmdline.inc
; Procedures for parsing command line.
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
; get_cmd_line
;-----------------------------------------------------------------------------
get_cmd_line:
		mov	si,81h
		cld
		call	process_options

		mov	ah,' '
		mov	di,filename
		call	process_param

		mov	ah,13
		mov	di,cmd_line_string
		jmp	process_param
;=============================================================================
; process_options
;-----------------------------------------------------------------------------
process_options:
@@next_char:
		lodsb
		cmp	al,' '
		je	@@next_char
		cmp	al,9
		je	@@next_char
		cmp	al,'/'
		je	@@option
		dec	si
		ret

@@option:
		lodsb
		call	upcase
		mov	di,available_options

@@next_option:
		scasb
		mov	ah,[di + (options_bits - available_options) - 1]
		je	@@set_option
		cmp	byte [di-1],0
		jne	@@next_option
		mov	ah,CMD_ERROR

@@set_option:
		mov	[cmd_line_options],ah
		jmp	@@next_char
;=============================================================================
; process_param
;-----------------------------------------------------------------------------
process_param:
@@next_char:
		lodsb
		cmp	al,' '
		je	@@next_char
		cmp	al,9
		je	@@next_char

@@save_char:
		cmp	al,13
		je	@@exit
		cmp	al,ah
		je	@@exit
;		call	upcase
		stosb
		lodsb
		jmp	@@save_char

@@exit:
		mov	al,0
		stosb
		dec	si
		ret
;=============================================================================
; E0F
;=============================================================================


