;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
; Some procedures based on code by M.Martynov, A.Ilyushin, S.Gorokhov.
;-----------------------------------------------------------------------------
; video.inc
; Procedures for saving/restoring screen and font.
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
; 2007-02-27	0leg	{save,restore}_screen: SCR_ROWS -> MAX_SCR_ROWS.
;=============================================================================


;=============================================================================
; Format of table used by save_mode/set_mode
;-----------------------------------------------------------------------------
;  V G A   r e g i s t e r s
;Sequencer registers		05h bytes	+00h
;CRT controller registers	1ah bytes	+05h
;Attribute controller registers	15h bytes	+1fh
;Graphics controller registers	0ah bytes	+34h
;DAC registers			30h bytes	+3eh
;Feature register		01h byte	+6eh
;Miscellaneous			01h byte	+6fh
;PEL mask			01h byte	+70h
;-----------------------------------------------------------------------------
;  B I O S   v a r i a b l e s
;Cursor position for page 0	02h bytes	+71h
;Screen width in columns	02h bytes	+73h
;Lines per char			02h bytes	+75h
;Last line number		01h byte	+77h
;Video mode number		01h byte	+78h
;-----------------------------------------------------------------------------
; Total bytes: 79h (121 dec)
;-----------------------------------------------------------------------------


;=============================================================================
; xchg_planes
;-----------------------------------------------------------------------------
; Exchanges font, chars, and attributes
;
xchg_planes:
		cld
		push	ds
		push	es
		push	ds
		pop	es		;ES:DI -> data
		mov	di,font
		mov	ax,0a000h
		mov	ds,ax
		xor	si,si		;DS:SI -> screen
		mov	bx,0402h	;2nd plane (font)
		call	set_regs
		mov	bx,256		;Number of characters
@@10:
		mov	cx,16		;Bytes per character
@@20:
		call	xchg_esdi_dssi
		cmpsb			;OPTIMIZE: instead of INC SI; INC DI
		loop	@@20
		add	si,16
		dec	bx
		jnz	@@10

		mov	di,old_screen
		mov	bx,0100h
		call	set_regs
		call	video_xchg

		mov	di,old_screen+1
		mov	bx,0201h
		call	set_regs
		call	video_xchg

		pop	es
		pop	ds
		ret
;=============================================================================
; xchg_esdi_dssi
;-----------------------------------------------------------------------------
xchg_esdi_dssi:
		mov	al,[si]
		xchg	al,[es:di]
		mov	[si],al
		ret
;=============================================================================
; video_xchg
;-----------------------------------------------------------------------------
; Exchanges or saves chars or attributes
;
video_xchg:
		mov	cx,SCR_COLS*SCR_ROWS
		xor	si,si
@@30:
		call	xchg_esdi_dssi
		cmpsw			;OPTIMIZE: instead of
		loop	@@30		;2*[INC SI; INC DI]
		ret
;=============================================================================
; set_regs
;-----------------------------------------------------------------------------
; Performs VGA registers setting to read/write font/chars
;
set_regs:
		mov	dx,03c4h
		mov	al,02h		;Map enable
		mov	ah,bh
		out	dx,ax
		call	dummy
		mov	ax,0704h
		out	dx,ax
		call	dummy
		mov	al,01h
		out	dx,al
		call	dummy
		inc	dx
		in	al,dx
		call	dummy
		or	al,20h
		out	dx,al
		call	dummy
	
		mov	dx,03ceh
		mov	al,04h
		mov	ah,bl		;Plane to read
		out	dx,ax
		call	dummy
		mov	ax,0005h
		out	dx,ax
		call	dummy
		mov	ax,0406h
		out	dx,ax
		call	dummy
		mov	ax,0001h
		out	dx,ax
		call	dummy
		mov	al,03h
		out	dx,ax
		call	dummy
		mov	ax,0ff08h
		out	dx,ax
		ret
;=============================================================================
; save_mode
;-----------------------------------------------------------------------------
; Saves video mode
;
save_mode:
		cld
;-----------------------------------------------------------------------------
; MISCELLANEOUS
;-----------------------------------------------------------------------------
		mov	dx,03cch	;for reading
		in	al,dx
		call	dummy
		mov	[es:di+6fh],al
		or	al,1		;color adapter (3d?h)
		mov	dl,0c2h		;for writing
		out	dx,al
		call	dummy
;-----------------------------------------------------------------------------
; Wait for vertical retrace
;-----------------------------------------------------------------------------
		mov	dl,0dah
@@90:
		in	al,dx
		test	al,00001000b
		jnz	@@90
@@91:
		in	al,dx
		test	al,00001000b
		jz	@@91
;-----------------------------------------------------------------------------
; SEQUENCER
;-----------------------------------------------------------------------------
		mov	dl,0c4h
		mov	cx,4
		mov	ah,01
		call	save_vga_regs
;-----------------------------------------------------------------------------
; CRTC
;-----------------------------------------------------------------------------
		mov	dl,0d4h
		mov	cl,19h
		mov	ah,00
		call	save_vga_regs
;-----------------------------------------------------------------------------
; AC
;-----------------------------------------------------------------------------
		mov	dl,0c0h
		mov	cl,15h
		mov	ah,00
@@30:
		push	dx
		mov	dl,0dah
		in	al,dx		; clear flip/flop
		pop	dx
		mov	al,ah
		out	dx,al
		call	dummy
		inc	dx
		inc	ah
		in	al,dx
		dec	dx
		stosb
		loop	@@30

		mov	dl,0dah
		in	al,dx		; clear flip/flop
		call	dummy
		mov	dl,0c0h
		mov	al,20h
		out	dx,al
		call	dummy
;-----------------------------------------------------------------------------
; GC
;-----------------------------------------------------------------------------
		mov	dl,0ceh
		mov	cl,9
		mov	ah,00
		call	save_vga_regs
;-----------------------------------------------------------------------------
; DAC
;-----------------------------------------------------------------------------
		mov	dl,0c7h
		mov	cl,10h
		mov	ah,00
@@50:
		mov	al,ah
		out	dx,al
		call	dummy
		inc	dx
		inc	dx

		in	al,dx
		stosb
		in	al,dx
		stosb
		in	al,dx
		stosb

		dec	dx
		dec	dx
		inc	ah
		loop	@@50
;-----------------------------------------------------------------------------
; FEATURE
;-----------------------------------------------------------------------------
		mov	dl,0cah
		in	al,dx
		stosb
;-----------------------------------------------------------------------------
; PEL MASK
;-----------------------------------------------------------------------------
		inc	di		;skip MISC reg
		mov	dl,0c6h
		in	al,dx
		stosb
;-----------------------------------------------------------------------------
; BIOS variables
;-----------------------------------------------------------------------------
		push	ds
		xor	ax,ax
		mov	ds,ax
		mov	ax,[450h]
		stosw
		mov	ax,[44ah]
		stosw
		mov	ax,[485h]
		stosw
		mov	al,[484h]
		mov	ah,[449h]
		stosw
		pop	ds
dummy:
		ret
;=============================================================================
; save_vga_regs
;-----------------------------------------------------------------------------
save_vga_regs:
		in	al,dx
		stosb
@@20:
		mov	al,ah
		out	dx,al
		call	dummy
		inc	dx
		inc	ah
		in	al,dx
		dec	dx
		stosb
		loop	@@20
		ret
;=============================================================================
; set_mode
;-----------------------------------------------------------------------------
; Sets video mode
;
set_mode:
		cld
		mov	dx,03cch
		in	al,dx
		call	dummy
		or	al,1		;Miscellaneous ( 3D? )
		mov	dl,0c2h
		out	dx,al
		call	dummy
;-----------------------------------------------------------------------------
; Wait for retrace
;-----------------------------------------------------------------------------
		mov	dl,0dah
@@90l:
		in	al,dx
		test	al,00001000b
		jnz	@@90l
@@91l:
		in	al,dx
		test	al,00001000b
		jz	@@91l
;-----------------------------------------------------------------------------
		in	al,dx		;Clear flip/flop
		call	dummy
		xor	al,al
		mov	dl,0c0h
		out	dx,al		;Disable screen
		call	dummy
;-----------------------------------------------------------------------------
; SEQUENCER
;-----------------------------------------------------------------------------
		cli
		mov	ax,0100h
		mov	dl,0c4h
		out	dx,ax		;Sequencer synchronous reset
		call	dummy
		mov	cx,4
		call	set_vga_regs	;!! AH == 01h !!
;-----------------------------------------------------------------------------
		mov	ax,0300h
		mov	dl,0c4h
		out	dx,ax		;Enable sequencer
		call	dummy
		sti
;-----------------------------------------------------------------------------
; CRTC
;-----------------------------------------------------------------------------
		mov	dl,0d4h
		mov	ax,0011h
		out	dx,ax		;Enable write to CRT controller
		call	dummy
		mov	cl,19h
		call	set_vga_regs	;!! AH == 00h !!
;-----------------------------------------------------------------------------
; AC
;-----------------------------------------------------------------------------
		mov	dl,0dah
		in	al,dx		;Clear flip/flop
		xor	ax,ax
		mov	cl,15h
		mov	dl,0c0h
@@40:
		mov	al,ah		;Attribute controller registers
		out	dx,al		;Register number
		call	dummy
		inc	ah
		lodsb
		out	dx,al		;Register data
		call	dummy
		loop	@@40
;-----------------------------------------------------------------------------
		xor	ax,ax
		mov	dl,0cch
		out	dx,al		;(03CC) <- 00
		call	dummy
		inc	ax
		mov	dl,0cah
		out	dx,al		;(03CA) <- 01
		call	dummy
;-----------------------------------------------------------------------------
; GC
;-----------------------------------------------------------------------------
		mov	dl,0ceh
		mov	cl,9
		call	set_vga_regs	;!! AH == 00h !!
;-----------------------------------------------------------------------------
; DAC
;-----------------------------------------------------------------------------
		mov	dl,0c8h
		xor	ax,ax
		out	dx,al
		call	dummy
		mov	cl,30h
		inc	dx
@@80:
		lodsb
		out	dx,al		;DAC registers
		loop	@@80		;RGB
;-----------------------------------------------------------------------------
; FEATURE
;-----------------------------------------------------------------------------
		lodsb
		mov	dl,0dah
		out	dx,al
		call	dummy
;-----------------------------------------------------------------------------
; MISCELLANEOUS
;-----------------------------------------------------------------------------
		lodsb
		mov	dl,0c2h
		out	dx,al
		call	dummy
;-----------------------------------------------------------------------------
; PEL MASK
;-----------------------------------------------------------------------------
		lodsb
		mov	dl,0c6h
		out	dx,al
		call	dummy
;-----------------------------------------------------------------------------
		mov	dl,0dah
		in	al,dx
		mov	dl,0c0h
		mov	al,20h
		out	dx,al		;Enable screen
;-----------------------------------------------------------------------------
; BIOS variables
;-----------------------------------------------------------------------------
		push	es
		xor	ax,ax
		mov	es,ax
		lodsw
		mov	[es:450h],ax
		lodsw
		mov	[es:44ah],ax
		lodsw
		mov	[es:485h],ax
		lodsw
		mov	[es:484h],al
		mov	[es:449h],ah
		pop	es
		ret
;=============================================================================
; set_vga_regs
;-----------------------------------------------------------------------------
set_vga_regs:
		lodsb
		push	ax
@@30:
		lodsb
		xchg	al,ah
		out	dx,ax
		inc	ax
		mov	ah,al
		loop	@@30
		pop	ax
		out	dx,al
		ret
;=============================================================================
; save_screen
;-----------------------------------------------------------------------------
; Saves user screen and font; sets mode 03h and loads new font
;
save_screen:
		cmp	byte [user_screen_mode],0
		je	@@exit
		mov	byte [user_screen_mode],0

		call	pushr
		cmp	byte [video_type],1	;1 == VGA
		jne	@@not_vga
;-----------------------------------------------------------------------------
; VGA
;-----------------------------------------------------------------------------
		mov	di,old_video_regs
		call	save_mode
		call	xchg_planes
		mov	si,mode_80x25
		call	set_mode
		jmp	@@quit
;-----------------------------------------------------------------------------
; EGA and older
;-----------------------------------------------------------------------------
@@not_vga:
		call	get_cursor
		mov	[old_cursor],dx
		mov	[old_cur_shape],cx
		mov	ah,0fh
		int	10h
		and	al,7fh
		mov	[old_mode_num],al
		cmp	al,2
		je	@@mode_ok
		cmp	al,3
		je	@@mode_ok
		mov	ax,83h
		int	10h
@@mode_ok:
		mov	ds,[video_seg]
		xor	si,si
		mov	di,old_screen
		mov	cx,SCR_COLS*MAX_SCR_ROWS
		cld
	rep	movsw
		xor	ax,ax
		mov	ds,ax
		and	byte [487h],7fh
		mov	al,[484h]	;Number of last line on the screen
		cbw
		mov	[cs:screen_height],ax
		sub	ax,CPU_HEIGHT+2
		mov	[cs:dump_height],ax
		mov	[cs:e_dump_height],ax
@@quit:
		call	popr
@@exit:
		ret
;=============================================================================
; restore_screen
;-----------------------------------------------------------------------------
; Restores user screen, font, and video mode
;
restore_screen:
		mov	byte [user_screen_mode],1
		call	pushr
		cmp	byte [video_type],1
		jne	@@not_vga
;-----------------------------------------------------------------------------
; VGA
;-----------------------------------------------------------------------------
		call	xchg_planes
		mov	si,old_video_regs
		call	set_mode
		jmp	@@quit
;-----------------------------------------------------------------------------
; EGA and older
;-----------------------------------------------------------------------------
@@not_vga:
		mov	es,[video_seg]
		xor	di,di
		mov	si,old_screen
		mov	cx,SCR_COLS*MAX_SCR_ROWS
		cld
	rep	movsw
		mov	ah,0
		mov	al,[old_mode_num]
		cmp	al,2
		je	@@mode_ok
		cmp	al,3
		je	@@mode_ok
		or	al,80h
		int	10h
@@mode_ok:
		mov	dx,[old_cursor]
		call	gotoxy
		mov	cx,[old_cur_shape]
		call	set_cursor
		xor	ax,ax
		mov	ds,ax
		and	byte [487h],7fh
@@quit:
		call	popr
		ret
;=============================================================================
; save_font
;-----------------------------------------------------------------------------
save_font:
		cmp	byte [video_type],1
		jne	@@quit
		push	ds
		push	es
		mov	ax,1130h
		mov	bh,06h
		int	10h
		push	es
		pop	ds
		mov	si,bp
		mov	di,font
		push	cs
		pop	es
		mov	cx,4096
	rep	movsb
		mov	di,old_screen
		mov	si,0
		mov	ds,[es:video_seg]
		mov	cx,SCR_COLS*SCR_ROWS
	rep	movsw
		pop	es
		pop	ds
@@quit:
		ret
;
;-----------------------------------------------------------------------------


;-----------------------------------------------------------------------------
mode_80x25:

%if SCR_WIDTH==80

;Sequencer registers
		db	0
		db	000h, 003h, 000h, 002h
;CRT controller registers
		db	0
		db	05fh, 04fh, 050h, 082h, 055h
		db	081h, 0bfh, 01fh, 000h,	04fh
		db	00dh, 00eh, 000h, 000h,	000h
		db	000h, 09ch, 08eh, 08fh, 028h
		db	01fh, 096h, 0b9h, 0a3h, 0ffh
;Attribute controller registers
		db	000h, 001h, 002h, 003h,	004h, 005h, 006h, 007h
		db	008h, 009h, 00ah, 00bh, 00ch, 00dh, 00eh, 00fh
		db	00ch, 000h, 00fh, 008h, 000h
;Graphics controller registers
		db	0
		db	000h, 000h, 000h, 000h, 000h, 010h, 00eh, 000h, 0ffh
;DAC registers
		db	00h, 00h, 00h	;00
		db	00h, 00h, 2ah	;01
		db	00h, 2ah, 00h	;02
		db	00h, 2ah, 2ah	;03
		db	2ah, 00h, 00h	;04
		db	2ah, 00h, 2ah	;05
		db	2ah, 2ah, 00h	;14
		db	2ah, 2ah, 2ah	;07

		db	15h, 15h, 15h	;38
		db	15h, 15h, 3fh	;39
		db	15h, 3fh, 15h	;3A
		db	15h, 3fh, 3fh	;3B
		db	3fh, 15h, 15h	;3C
		db	3fh, 15h, 3fh	;3D
		db	3fh, 3fh, 15h	;3E
		db	3fh, 3fh, 3fh	;3F
;Feature
		db	000h
;Miscelianeous
		db	067h
;Pel mask
		db	0ffh
;BIOS variables
word_0450h	dw	0		;Cursor position for 1st page
word_044ah	dw	80		;Screen width in columns
word_0485h	dw	16		;Lines per char
byte_0484h	db	24		;Last line number
byte_0449h	db	3		;Video mode

%elif SCR_WIDTH==90

;Sequencer registers
		db	0
		db	001h, 003h, 000h, 002h
;CRT controller registers
		db	0
		db	06bh, 059h, 05ah, 08eh, 060h
		db	08dh, 0bfh, 01fh, 000h,	04fh
		db	00dh, 00eh, 000h, 000h,	008h
		db	070h, 09ch, 093h, 08fh, 02dh
		db	01fh, 096h, 0b9h, 0a3h, 0ffh
;Attribute controller registers
		db	000h, 001h, 002h, 003h,	004h, 005h, 006h, 007h
		db	008h, 009h, 00ah, 00bh, 00ch, 00dh, 00eh, 00fh
		db	00ch, 000h, 00fh, 000h, 000h
;Graphics controller registers
		db	0
		db	000h, 000h, 000h, 000h, 000h, 010h, 00eh, 000h, 0ffh
;DAC registers
		db	00h, 00h, 00h	;00
		db	00h, 00h, 2ah	;01
		db	00h, 2ah, 00h	;02
		db	00h, 2ah, 2ah	;03
		db	2ah, 00h, 00h	;04
		db	2ah, 00h, 2ah	;05
		db	2ah, 2ah, 00h	;14
		db	2ah, 2ah, 2ah	;07

		db	15h, 15h, 15h	;38
		db	15h, 15h, 3fh	;39
		db	15h, 3fh, 15h	;3A
		db	15h, 3fh, 3fh	;3B
		db	3fh, 15h, 15h	;3C
		db	3fh, 15h, 3fh	;3D
		db	3fh, 3fh, 15h	;3E
		db	3fh, 3fh, 3fh	;3F
;Feature
		db	000h
;Miscelianeous
		db	067h
;Pel mask
		db	0ffh
;BIOS variables
word_0450h	dw	0		;Cursor position for 1st page
word_044ah	dw	90		;Screen width in columns
word_0485h	dw	16		;Lines per char
byte_0484h	db	24		;Last line number
byte_0449h	db	3		;Video mode

%else

;Sequencer registers
		db	0
		db	000h, 003h, 000h, 002h
;CRT controller registers
		db	0
		db	05fh, 04fh, 050h, 082h, 055h
		db	081h, 0bfh, 01fh, 000h,	04fh
		db	00dh, 00eh, 000h, 000h,	000h
		db	000h, 09ch, 08eh, 08fh, 028h
		db	01fh, 096h, 0b9h, 0a3h, 0ffh
;Attribute controller registers
		db	000h, 001h, 002h, 003h,	004h, 005h, 006h, 007h
		db	008h, 009h, 00ah, 00bh, 00ch, 00dh, 00eh, 00fh
		db	00ch, 000h, 00fh, 008h, 000h
;Graphics controller registers
		db	0
		db	000h, 000h, 000h, 000h, 000h, 010h, 00eh, 000h, 0ffh
;DAC registers
		db	00h, 00h, 00h	;00
		db	00h, 00h, 2ah	;01
		db	00h, 2ah, 00h	;02
		db	00h, 2ah, 2ah	;03
		db	2ah, 00h, 00h	;04
		db	2ah, 00h, 2ah	;05
		db	2ah, 2ah, 00h	;14
		db	2ah, 2ah, 2ah	;07

		db	15h, 15h, 15h	;38
		db	15h, 15h, 3fh	;39
		db	15h, 3fh, 15h	;3A
		db	15h, 3fh, 3fh	;3B
		db	3fh, 15h, 15h	;3C
		db	3fh, 15h, 3fh	;3D
		db	3fh, 3fh, 15h	;3E
		db	3fh, 3fh, 3fh	;3F
;Feature
		db	000h
;Miscelianeous
		db	067h
;Pel mask
		db	0ffh
;BIOS variables
word_0450h	dw	0		;Cursor position for 1st page
word_044ah	dw	80		;Screen width in columns
word_0485h	dw	16		;Lines per char
byte_0484h	db	24		;Last line number
byte_0449h	db	3		;Video mode

%endif
;
;-----------------------------------------------------------------------------


;=============================================================================
; E0F
;=============================================================================

