;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; system.inc
; Misc. system procedures.
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
; Changelog
;-----------------------------------------------------------------------------
; 2007-02-17	0leg	Added comments to all procedures. All variables moved
;			to act[u]data.inc.
; 2007-12-02	0leg	ascii_to_bin added.
;=============================================================================


;=============================================================================
; pushr
;-----------------------------------------------------------------------------
; Saves all registers.
; In:	AX, BX, CX, DX, SI, DI, BP, DS, ES -- registers to save
; Out:	---
; Modf:	---
; Call:	---
; Use:	keep_ip
;
pushr:
		pop	word [cs:keep_ip]
		push	ax
		push	bx
		push	cx
		push	dx
		push	si
		push	di
		push	bp
		push	ds
		push	es
		jmp	word [cs:keep_ip]
;=============================================================================
; popr
;-----------------------------------------------------------------------------
; Restores all registers.
; In:	---
; Out:	AX, BX, CX, DX, SI, DI, BP, DS, ES restored
; Modf:	AX, BX, CX, DX, SI, DI, BP, DS, ES
; Call:	---
; Use:	keep_ip
;
popr:
		pop	word [cs:keep_ip]
		pop	es
		pop	ds
		pop	bp
		pop	di
		pop	si
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		jmp	word [cs:keep_ip]
;=============================================================================
; calc_prog_size
;-----------------------------------------------------------------------------
; Calculates the size of program.
; In:	CX -- amount of additional memory
; Out:	CY if error
;	NC if no errors:
;		AX -- program end
;		DX -- program size in paragraphs
; Modf:	AX, DX
; Call:	---
; Use:	code_end, STACK_SIZE, MAX_PROGRAM_SIZE
;
calc_prog_size:
		push	cx
		mov	ax,[code_end]
		add	ax,cx
		jc	@@exit
		test	ax,0fh
		jz	@@calc_size
		and	ax,0fff0h
		add	ax,10h
		jc	@@exit
@@calc_size:
		add	ax,STACK_SIZE
		jc	@@exit
		cmp	ax,MAX_PROGRAM_SIZE+1
		cmc
		jc	@@exit
		mov	cl,4
		mov	dx,ax
		shr	dx,cl
		clc
@@exit:
		pop	cx
		ret
;=============================================================================
; action_short_jump
;-----------------------------------------------------------------------------
; Searches table for specified key. Format of table: key: word; value: byte,
; end of table: key == 0.
; In:	DI -> table - 3
;	AX -- key
; Out:	CY if key not found
;	NC if key found:
;		DI -- value
; Modf:	DI
; Call:	---
; Use:	---
;
action_short_jump:
@@next:
		add	di,3
		cmp	word [di],0
		je	@@exit
		cmp	word [di],ax
		jne	@@next
		push	ax
		xor	ax,ax
		mov	al,[di+2]
		xchg	ax,di
		pop	ax
		clc
		ret
@@exit:
		stc
		ret
;=============================================================================
; action_long_jump
;-----------------------------------------------------------------------------
; Searches table for specified key. Format of table: key: word; value: word,
; end of table: key == 0.
; In:	DI -> table - 4
;	AX -- key
; Out:	CY if key not found
;	NC if key found:
;		DI -- value
; Modf:	DI
; Call:	---
; Use:	---
;
action_long_jump:
@@next:
		add	di,4
		cmp	word [di],0
		je	@@exit
		cmp	word [di],ax
		jne	@@next
;		push	ax
;		xor	ax,ax
;		mov	al,[di+2]
;		xchg	ax,di
;		pop	ax
		mov	di,[di+2]
		clc
		ret
@@exit:
		stc
		ret
;=============================================================================
; fill_byte_str, fill_word_str, fill_dword_str
;-----------------------------------------------------------------------------
; Stores hexadecimal representation of byte, word or double word in string.
; In:	fill_byte_str:
;		AL -- byte
;	fill_word_str:
;		AX -- word
;	fill_dword_str:
;		DS:SI -> dword
;	ES:DI -> string
; Out:	---
; Modf:	AX?, SI?, DI
; Call:	fill_word_str, fill_byte_str
; Use:	---
;
fill_dword_str:
		lodsw
		push	ax
		lodsw
		call	fill_word_str
		pop	ax

fill_word_str:
		xchg	al,ah
		call	fill_byte_str
		xchg	al,ah

fill_byte_str:
		push	ax
		push	cx
		mov	cl,4
@@next:
		push	ax
		shr	al,cl
		and	al,0fh
		cmp	al,10
		sbb	al,69h
		das
		stosb
		pop	ax
		sub	cl,4
		jz	@@next

		pop	cx
		pop	ax
		ret
;=============================================================================
; strlen
;-----------------------------------------------------------------------------
; Returns length of string. Terminating \0 not counted.
; In:	DS:SI -> string
;	DF == 0
; Out:	CX -- length
; Modf:	CX
; Call:	---
; Use:	---
;
strlen:
		push	ax
		push	si
		xor	cx,cx
@@next:
		inc	cx
		lodsb
		or	al,al
		jnz	@@next
		dec	cx
		pop	si
		pop	ax
		ret
;=============================================================================
; upcase
;-----------------------------------------------------------------------------
; Translates character to uppercase.
; In:	AL -- char
; Out:	AL -- uppercased char
; Modf:	AL
; Call:	---
; Use:	---
;
upcase:
		cmp	al,'a'
		jb	@@quit
		cmp	al,'z'
		ja	@@quit
		sub	al,20h
@@quit:
		ret
;=============================================================================
; lowcase
;-----------------------------------------------------------------------------
; Translates character to lowercase.
; In:	AL -- char
; Out:	AL -- lowercased char
; Modf:	AL
; Call:	---
; Use:	---
;
lowcase:
		cmp	al,'A'
		jb	@@quit
		cmp	al,'Z'
		ja	@@quit
		add	al,20h
@@quit:
		ret
;=============================================================================
; upline
;-----------------------------------------------------------------------------
; Translates string to uppercase.
; In:	DS:SI -> string
;	ES == DS
; Out:	---
; Modf:	AL
; Call:	---
; Use:	---
;
upline:
		push	si
		push	di
		mov	di,si
@@next:
		lodsb
		cmp	al,0
		je	@@quit
		call	upcase
		stosb
		jmp	@@next
@@quit:
		pop	di
		pop	si
		ret
;=============================================================================
; beep
;-----------------------------------------------------------------------------
; Produces short beep.
; In:	---
; Out:	---
; Modf:	---
; Call:	---
; Use:	---
;
beep:
		push	ax
		push	cx
		in	al,61h
		or	al,3
		out	61h,al
		mov	al,10110110b
		out	43h,al
		mov	ax,700
		out	42h,al
		mov	al,ah
		out	42h,al

		mov	cx,0A000h
		loop	$

		in	al,61h
		and	al,11111100b
		out	61h,al
		pop	cx
		pop	ax
		ret
;=============================================================================
; ascii_to_bin
;-----------------------------------------------------------------------------
; Converts ascii char '0'..'9', 'A'..'F', 'a'..'f' to binary value.
; In:	AL -- char
; Out:	NC if no errors:
;		AL -- binary value
;	CY if AL contains invalid char
;		AL may be uppercased
; Modf:	AX, BX, CL
; Call:	upcase
; Use:	---
;
ascii_to_bin:
		call	upcase
		cmp	al,'0'
		jb	@@error
		cmp	al,'9'
		jbe	@@digit
		cmp	al,'A'
		jb	@@error
		cmp	al,'G'
		cmc
		jc	@@error
		sub	al,7
@@digit:
		sub	al,'0'
@@error:
		ret
;=============================================================================
; E0F
;=============================================================================


