;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; keyboard.inc
; Procedures for working with keyboard.
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
; 2007-02-10	0leg	Added comments to all procedures. All variables moved
;			to act[u]data.inc
;=============================================================================


;=============================================================================
; key_loop
;-----------------------------------------------------------------------------
; Reads keystrokes and calls corresponding procedures in loop.
; On end of table key_loop calls corresponding proc, if it is not 0.
; If it is 0 or if it return CY, [test_key] called. If [test_key]
; return NC, key_loop terminates, otherwise beep called.
; In:	[keys] -> structure: key-code: word, proc: word,
;		end of table: key-code == 0, exit: proc == 0
; Out:	AX -- key codes
; Modf:	AX, BX, CX, DX, SI, DI, BP
; Call:	flush_key, read_key, [test_key], [key_loop_sub], beep
; Use:	keys, key_loop_sub, cur_test_code, test_key
;
key_loop:

@@loop:
		push	ax
		push	bx
		push	si
		mov	si,[keys]
		call	flush_key
		call	read_key
		xchg	ax,bx
@@next:
		lodsw
		or	ax,ax			;End of table?
		jz	@@not_found		;Yes
		cmp	ax,bx
		je	@@found
		lodsw
		jmp	@@next

@@found:
		lodsw
		or	ax,ax			;Exit?
		jz	@@quit			;Yes
		mov	[key_loop_sub],ax
		pop	si
		pop	bx
		pop	ax
		call	word [key_loop_sub]	;Call procedure
		jmp	@@loop

@@not_found:
		mov	[cur_test_code],bx	;Key codes
		lodsw
		mov	[key_loop_sub],ax	;Procedure for any key
		or	ax,ax			;Exit?
		xchg	ax,bx
		pop	si
		pop	bx
		jz	@@test_key		;Yes
		call	word [key_loop_sub]	;AX == codes on entry
		jc	@@test_key
		pop	ax
		jmp	@@loop			;Next...

@@test_key:
		pop	ax
		call	word [test_key]
		jnc	@@quit_1
		call	beep
		jmp	@@loop
@@quit:
		mov	ax,bx			;Key codes
		pop	si
		pop	bx
		inc	sp
		inc	sp
@@quit_1:
		ret
;=============================================================================
; def_test_key
;-----------------------------------------------------------------------------
; Default key testing procedure.
; In:	---
; Out:	CY
; Modf:	---
; Call:	---
; Use:	---
;
def_test_key:
		stc
		ret
;=============================================================================
; read_key
;-----------------------------------------------------------------------------
; Reads keystroke.
; In:	---
; Out:	AX -- ASCII/scan codes
; Modf:	AX
; Call:	---
; Use:	---
;
read_key:
		push	bx
		xor	bx,bx
@@next:
		mov	ah,1
		int	16h
		jnz	@@get
		mov	ah,2
		int	16h
		test	al,8
		jnz	@@alt
		cmp	bl,2
		je	@@ret
		mov	bl,1
		jmp	@@next
@@ret:
		mov	ax,kbAltAlone
		jmp	@@quit
@@get:
		mov	ah,0
		int	16h
@@quit:
		pop	bx
		ret
@@alt:
		or	bx,bx
		jz	@@next
		mov	bl,2
		jmp	@@next
;=============================================================================
; in_key
;-----------------------------------------------------------------------------
; Checks if key pressed.
; In:	---
; Out:	ZR if no key pressed
;	NZ if key pressed:
;		AX -- ASCII/scan codes
; Modf:	AX
; Call:	---
; Use:	---
;
%if 0
in_key:

		mov	ah,1
		int	16h
		ret
%endif
;=============================================================================
; flush_key
;-----------------------------------------------------------------------------
; Flushes keyboard buffer.
; In:	---
; Out:	---
; Modf:	AX
; Call:	---
; Use:	---
;
flush_key:
@@next:
		mov	ah,1
		int	16h
		jz	@@quit
		mov	ah,0
		int	16h
		jmp	@@next
@@quit:
		ret
;=============================================================================
; check_shift
;-----------------------------------------------------------------------------
; Checks if any shift key is pressed.
; In:	---
; Out:	ZR if shift key is not pressed
;	NZ if shift key is pressed
; Modf:	---
; Call:	---
; Use:	---
;
check_shift:
		push	ax
		push	ds
		xor	ax,ax
		mov	ds,ax
		test	byte [417h],11b
		pop	ds
		pop	ax
		ret
;=============================================================================
; E0F
;=============================================================================


