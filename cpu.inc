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
		jne	.cpu1
		xor	cx,cx
		mov	di,.inc_cx
		mov	al,90h
		cli
		mov	[di],al
		nop
		nop
		nop
.inc_cx:
 		inc	cx
		sti
		mov	ax,1
		sub	ax,cx
		jmp	.quit
.cpu1:
		or	ax,0f000h
		push	ax
		popf
		pushf
		pop	ax
		and	ax,0f000h
		jnz	.cpu2
		mov	al,2
		jmp	.quit
.cpu2:
		mov	bx,sp
		and	sp,~3
		; Start of `cpu 486' emulation.
		db 0x66
		pushf                   ; 669C              pushfd
		db 0x66
		pop	ax              ; 6658              pop eax
		db 0x66
		mov	cx,ax           ; 6689C1            mov ecx,eax
		db 0x66, 0x35
		dd 40000h               ; 663500000400      xor eax,0x40000
		db 0x66
		push	ax              ; 6650              push eax
		db 0x66
		popf                    ; 669D              popfd
		db 0x66
		pushf                   ; 669C              pushfd
		db 0x66
		pop	ax              ; 6658              pop eax
		mov	sp,bx
		db 0x66
		xor	ax,cx           ; 6631C8            xor eax,ecx
		mov	al,3
		jz	.quit
		db 0x66, 0xbb
		dd 200000h              ; 66BB00002000      mov ebx,0x200000
		db 0x66
		mov	ax,cx           ; 6689C8            mov eax,ecx
		db 0x66
		xor	ax,bx           ; 6631D8            xor eax,ebx
		db 0x66
		push	ax              ; 6650              push eax
		db 0x66
		popf                    ; 669D              popfd
		db 0x66
		pushf                   ; 669C              pushfd
		db 0x66
		pop	ax              ; 6658              pop eax
		db 0x66
		push	cx              ; 6651              push ecx
		db 0x66
		popf                    ; 669D              popfd
		db 0x66
		and	ax,bx           ; 6621D8            and eax,ebx
		db 0x66
		and	cx,bx           ; 6621D9            and ecx,ebx
		db 0x66
		cmp	ax,cx           ; 6639C8            cmp eax,ecx
		; End of `cpu 486' emulation.
		mov	al,4
		jz	.quit
		mov	al,7fh
.quit:
		cbw
		pop	cx
		pop	bx
		popf
		ret
;=============================================================================
; E0F
;=============================================================================


