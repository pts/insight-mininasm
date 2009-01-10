;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; inst.inc
; ...
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


s_sw7		db	00000001b

S_SW78		equ	00000101b
S_SW7M		equ	00000010b
S_SW8		equ	00000100b
S_JMP		equ	00001000b
S_EM		equ	00010000b
;$em8		equ	00100000b
;$em78		equ	00110000b
S_PREFIX	equ	01000000b
S_GRP		equ	10000000b

;=============================================================================
; check_swap
;-----------------------------------------------------------------------------
; Expects: dl=0 (Single Step), dl=1 (Proc Trace), dl=2 (Other)
check_swap:
		push	ax
		push	bx
		push	si
		push	es

		cmp	byte [swap_mode],0
		je	@@not
		cmp	byte [swap_mode],2
		je	@@swap
		cmp	dl,2
		jae	@@swap

		mov	si,[cpu_regs+REG_IP]
		mov	es,[cpu_regs+REG_CS]
		call	get_inst_desc

		test	al,[s_sw7]
		jnz	@@swap
		test	al,S_SW8
		jnz	@@swap_8
		test	al,S_SW7M
		jz	@@not
	es	lodsb
		and	al,11000000b
		cmp	al,11000000b
		jne	@@swap
@@not:
		mov	byte [swap_flag],0
@@quit:
		pop	es
		pop	si
		pop	bx
		pop	ax
		ret
@@swap_8:
		or	dl,dl
		jz	@@not
@@swap:
		mov	byte [swap_flag],1
		jmp	@@quit

swap_flag	db	0
;=============================================================================
; check_jump
;-----------------------------------------------------------------------------
check_jump:
		push	ax
		push	bx
		push	si
		push	es
		mov	si,[cpu_regs+REG_IP]
		mov	es,[cpu_regs+REG_CS]
		call	get_inst_desc
		test	al,S_JMP
		jz	@@quit
		stc
@@quit:
		pop	es
		pop	si
		pop	bx
		pop	ax
		ret
;=============================================================================


;~;------------------------------------------------------------------------------
;~CheckEmul	proc
;~	; Expects: dl=0 (Single Step), dl=1 (Proc Trace)
;~	; Returns: CF set if emulation needed
;~		push	ax bx si es
;~		mov	si,_IP
;~		mov	es,_CS
;~		call	GetInstDesc
;~		test	al,S_EM78
;~		jz	@@Quit
;~		or	dl,dl
;~		jz	@@SS
;~		test	al,S_EM8
;~		jz	@@Quit
;~@@Emul:
;~		stc
;~@@Quit:
;~		pop	es si bx ax
;~		ret
;~@@SS:
;~		test	al,S_EM7
;~		jz	@@Quit
;~		jmp	@@Emul
;~endp
;~#

;=============================================================================
; get_inst_desc
;-----------------------------------------------------------------------------
get_inst_desc:
		mov	bx,inst_desc
		cld
@@pref:
	es	lodsb
		xlat
		test	al,S_PREFIX
		jnz	@@pref
		test	al,S_GRP
		jz	@@quit
@@grp:
		mov	al,[es:si]
		mov	bx,s_grjump
		mov	ah,al
		shr	al,1
		shr	al,1
		shr	al,1
		and	al,0111b
		xlat
@@quit:
		ret
;=============================================================================
inst_desc:
		db	S_SW7M		; 00
		db	S_SW7M
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	S_SW7M		; 08
		db	S_SW7M
		db	0
		db	0
		db	0
		db	0
		db	0
		db	S_JMP		;s_second

		db	S_SW7M		; 10
		db	S_SW7M
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	S_SW7M		; 18
		db	S_SW7M
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	S_SW7M		; 20
		db	S_SW7M
		db	0
		db	0
		db	0
		db	0
		db	S_PREFIX
		db	0

		db	S_SW7M		; 28
		db	S_SW7M
		db	0
		db	0
		db	0
		db	0
		db	S_PREFIX
		db	0

		db	S_SW7M		; 30
		db	S_SW7M
		db	0
		db	0
		db	0
		db	0
		db	S_PREFIX
		db	0

		db	S_SW7M		; 38
		db	S_SW7M
		db	0
		db	0
		db	0
		db	0
		db	S_PREFIX
		db	0

		db	0		; 40
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 48
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 50
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 58
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 60
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 68
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	S_JMP		; 70
		db	S_JMP
		db	S_JMP
		db	S_JMP
		db	S_JMP
		db	S_JMP
		db	S_JMP
		db	S_JMP

		db	S_JMP		; 78
		db	S_JMP
		db	S_JMP
		db	S_JMP
		db	S_JMP
		db	S_JMP
		db	S_JMP
		db	S_JMP

		db	S_SW7M		; 80
		db	S_SW7M
		db	S_SW7M
		db	S_SW7M
		db	0
		db	0
		db	S_SW7M
		db	S_SW7M

		db	S_SW7M		; 88
		db	S_SW7M
		db	0
		db	0
		db	S_SW7M
		db	0
		db	0
		db	S_SW7M

		db	0		; 90
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; 98
		db	0
		db	S_SW8
		db	0
		db	S_EM
		db	S_EM
		db	0
		db	0

		db	0		; A0
		db	0
		db	S_SW78
		db	S_SW78
		db	S_SW78
		db	S_SW78
		db	0
		db	0

		db	0		; A8
		db	0
		db	S_SW78
		db	S_SW78
		db	0
		db	0
		db	0
		db	0

		db	0		; B0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; B8
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	0		; C0
		db	0
		db	S_JMP
		db	S_JMP
		db	0
		db	0
		db	S_SW7M
		db	S_SW7M

		db	0		; C8
		db	0
		db	S_JMP
		db	S_JMP
		db	0
		db	S_EM+S_SW8
		db	S_EM+S_SW8
		db	S_EM+S_JMP

		db	S_SW7M		; D0
		db	S_SW7M
		db	S_SW7M
		db	S_SW7M
		db	0
		db	0
		db	0
		db	0

		db	0		; D8
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0
		db	0

		db	S_SW8		; E0
		db	S_SW8
		db	S_SW8
		db	S_JMP
		db	0
		db	0
		db	0
		db	0

		db	S_SW8		; E8
		db	S_JMP
		db	S_JMP
		db	S_JMP
		db	0
		db	0
		db	0
		db	0

		db	S_PREFIX		; F0
		db	S_PREFIX
		db	S_PREFIX
		db	S_PREFIX
		db	0
		db	0
		db	0
		db	0

		db	0		; F8
		db	0
		db	0
		db	0
		db	0
		db	0
		db	S_GRP
		db	S_GRP


s_grjump:	db	S_SW7M, S_SW7M, S_SW8, S_SW8, S_JMP, S_JMP, 0, 0

;=============================================================================
; E0F
;=============================================================================


