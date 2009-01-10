;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; cpu.inc
; Procedure for determining CPU type.
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
; get_cpu_type
;-----------------------------------------------------------------------------
; Determines CPU type.
; In:	---
; Out:	AX -- CPU type:
;		0  =  8086
;		1  =  8088
;		2  = 80286
;		3  = 80386
;		4  = 80486
;		7f = Advanced (with CPUID)
; Modf:	AX
; Call:	---
; Use:	---
;
get_cpu_type:
	cpu	486
		pushf
		push	bx
		push	cx
		pushf
		pop	ax
		and	ax,0fffh
		push	ax
		popf
		pushf
		pop	ax
		and	ax,0f000h
		cmp	ax,0f000h
		jne	@@cpu1
		xor	cx,cx
		mov	di,@@inc_cx
		mov	al,90h
		cli
		mov	[di],al
		nop
		nop
		nop
@@inc_cx:
 		inc	cx
		sti
		mov	ax,1
		sub	ax,cx
		jmp	@@quit
@@cpu1:
		or	ax,0f000h
		push	ax
		popf
		pushf
		pop	ax
		and	ax,0f000h
		jnz	@@cpu2
		mov	al,2
		jmp	@@quit
@@cpu2:
		mov	bx,sp
		and	sp,~3
		pushfd
		pop	eax
		mov	ecx,eax
		xor	eax,40000h
		push	eax
		popfd
		pushfd
		pop	eax
		mov	sp,bx
		xor	eax,ecx
		mov	al,3
		jz	@@quit
		mov	ebx,200000h
		mov	eax,ecx
		xor	eax,ebx
		push	eax
		popfd
		pushfd
		pop	eax
		push	ecx
		popfd
		and	eax,ebx
		and	ecx,ebx
		cmp	eax,ecx
		mov	al,4
		jz	@@quit
		mov	al,7fh
@@quit:
		cbw
		pop	cx
		pop	bx
		popf
		ret
	cpu	8086
;=============================================================================
; E0F
;=============================================================================


