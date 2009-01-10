;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; windows.inc
; Procedures for creating windows and menus.
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
; 2007-02-06	0leg	Added comments to all (?) procedures.
; 2007-02-10	0leg	All variables moved to act[u]data.inc
; 2007-03-13	0leg	close_window and restore_window now accepts no
;			parameters (coordinates saved in window_buffer)
; 2007-11-17	0leg	write_point changed to support optimized menu structures
; 2008-03-22	0leg	Fixed incorrect shadow color
;=============================================================================


;=============================================================================
; exec_menu
;-----------------------------------------------------------------------------
; Creates horizontal menu.
; In:	DS:SI -> menu structure.
;		Format of menu structure:
;			dw	number-of-items
;			dw	ptr-to-list-of-strings
;			dw	ptr-to-list-of-submenus
;
;		List of strings (menu items):
;			db	'~F~oo', 0
;			db	'~B~ar', 0
;			db	'Baz', 0
;			...
;		Hot keys enclosed in tildes (~).
;
;		List of submenus:
;			dw	SUB_MENU, ptr-to-Foo-submenu-struc
;			dw	DISABLED, ptr-to-Bar-submenu-struc
;			dw	0,        ptr-to-Baz-procedure
;			...
;
;		Format of submenu structure:
;			db	left, top, right, bottom ;coordinates
;			dw	number-of-items
;			db	number-of-selected-item
;			db	width-of-cursor
;			dw	ptr-to-list-of-procedures
;			dw	ptr-to-list-of-strings
;
;		List of procedures:
;			dw	SUB_MENU, ptr-to-submenu-struc
;			dw	DISABLED, ptr-to-procedure
;			dw	0,        ptr-to-procedure
;			...
;
; Out:	AX -- exit code
; Modf:	AX, BX, CX, DX, SI, DI, BP
; Call:	---
; Use:	---
;
exec_menu:
		xor	bx,bx
		mov	dx,USED_COLS-1
		call	save_window

		cld
		lodsw			;Number of menu items
		mov	cx,ax
		mov	bx,[h_pos]
@@next:
		call	update_hmenu
@@read:
		call	read_key
		cmp	ax,kbEsc
		je	@@esc
		cmp	ax,kbAltAlone
		je	@@esc
		cmp	ax,kbEnter
		je	@@enter
		cmp	ax,kbDown
		je	@@enter
		cmp	ax,kbRight
		je	@@right
		cmp	ax,kbLeft
		je	@@left
		push	si
		mov	si,[si]		;Ptr to string list
		call	find_menu_letter
		pop	si
		jc	@@read
		mov	bx,ax		;Menu item
		call	update_hmenu
		jmp	@@enter
@@right:
		inc	bx
		cmp	bx,cx
		jb	@@next
		xor	bx,bx
		jmp	@@next
@@left:
		dec	bx
		jns	@@next
		mov	bx,cx
		dec	bx
		jmp	@@next
@@enter:
		push	si
		mov	si,[si+2]	;Ptr to list of submenus
		call	exec_sub_menu
		pop	si
		cmp	ax,2
		je	@@left_exp
		cmp	ax,3
		je	@@right_exp
		cmp	ax,100h
		jae	@@quit
@@esc:
		xor	ax,ax
@@quit:
		mov	word [point_options],0
		mov	[h_pos],bx
;		xor	bx,bx
;		mov	dx,79
		call	restore_window
		or	ax,ax
		jz	@@ret
		call	ax
@@ret:
		ret
@@left_exp:
		dec	bx
		jns	@@exp
		mov	bx,cx
		dec	bx
		jmp	@@exp
@@right_exp:
		inc	bx
		cmp	bx,cx
		jb	@@exp
		xor	bx,bx
@@exp:
		call	update_hmenu
		jmp	@@enter
;=============================================================================
; exec_sub_menu
;-----------------------------------------------------------------------------
; Creates submenu.
; In:	DS:SI -> list of submenus
;	BX -- number of current item
; Out:	AX -- result:
;		1 -- menu item disabled
;		>= 100h -- ptr to procedure
; Modf:	AX
; Call:	open_window_bord, vert_menu, exec_sub_menu, close_window
; Use:	sub_struc, sub_coord, empty_title, point_options
;
exec_sub_menu:
		push	bx
		push	cx
		push	dx
		push	si
		push	word [sub_struc]
		push	word [sub_coord]
		shl	bx,1
		shl	bx,1
		mov	ax,[bx+si]	;DISABLED, SUB_MENU, etc.
		test	al,DISABLED
		jz	@@enabled
		mov	ax,1
		jmp	@@quit_2
@@enabled:
		and	ax,~DISABLED
		or	ax,ax
		jnz	@@sub_menu	;Submenu if bit 0 is set
		mov	ax,[bx+si+2]	;Otherwise -- procedure
;		call	word ptr [bx+si+2]
;		xor	ax,ax
		jmp	@@quit_2
@@sub_menu:
		mov	si,[bx+si+2]	;Ptr to submenu structure
		lodsw
		mov	[sub_coord],ax	;Coordinates of top left corner
		mov	bx,ax
		lodsw
		mov	dx,ax		;Coordinates of lower right corner
		mov	ax,atMenuBorder*256+1
		push	si
		mov	si,empty_title
		mov	cx,single_border
		call	open_window_bord
		pop	si

		mov	[sub_struc],si
@@next_menu:
		lodsw
		mov	cx,ax		;Number of items
		lodsw
		mov	dx,ax		;Selected/width
		lodsw
		mov	bp,ax		;Ptr to list of procedures
		mov	[point_options],ax
		lodsw
		mov	si,ax		;Ptr to list of strings
		mov	ax,[sub_coord]
		add	ax,0101h
		mov	bl,atMenuNorm
		mov	bh,atMenuSel
		call	vert_menu
		mov	si,[sub_struc]
		mov	[si+2],ah
		cmp	al,-3		;???
		jae	@@quit_a	;???

		push	si
		mov	bl,al
		mov	bh,0
		mov	si,bp
		call	exec_sub_menu
		pop	si
		cmp	ax,1
		je	@@next_menu

@@quit:
		call	close_window
@@quit_2:
		pop	word [sub_coord]
		pop	word [sub_struc]
		pop	si
		pop	dx
		pop	cx
		pop	bx
		ret
@@quit_a:
		neg	al
		mov	ah,0
		jmp	@@quit
;=============================================================================
; update_hmenu
;-----------------------------------------------------------------------------
; Updates horizontal menu.
; In:	DS:SI -> pointer to string list (2nd word in menu structure)
;	BX -- number of active item
;	CX -- number of items
; Out:	---
; Modf:	---
; Call:	pushr, popr
; Use:	video_seg
;
update_hmenu:
		call	pushr
		mov	si,[si]
		mov	es,[video_seg]
		xor	di,di
		xor	bp,bp
		mov	ah,atHMenuNorm
		mov	al,' '
		stosw
@@next_point:
		mov	ah,atHMenuNorm
		mov	dh,atHMenuNormHot
		cmp	bx,bp
		jne	@@1
		mov	ah,atHMenuSel
		mov	dh,atHMenuSelHot
@@1:
		mov	al,' '
		stosw
@@next:
		lodsb
		cmp	al,0
		je	@@end
		cmp	al,'~'
		je	@@sw
		stosw
		jmp	@@next
@@sw:
		xchg	ah,dh
		jmp	@@next
@@end:
		mov	al,' '
		stosw
		inc	bp
		cmp	bp,cx
		jb	@@next_point

		mov	cx,160		;!!!
		sub	cx,di
		shr	cx,1
		mov	al,' '
		mov	ah,atHMenuNorm
	rep	stosw

		call	popr
		ret
;=============================================================================
; local_menu
;-----------------------------------------------------------------------------
; Creates local menu.
; In:	SI -> menu structure
; Out:	CY -- ESC pressed
;	NC -- Enter pressed:
;		AX -> procedure
; Modf:	AX
; Call:	pushr, get_cursor, no_cursor, exec_sub_menu, set_cursor, popr
; Use:	fake_table
;
local_menu:
		call	pushr

		call	get_cursor
		push	cx
		call	no_cursor
		xor	bx,bx			;Current item
		mov	[fake_table+2],si	;Ptr to submenu struc.
		mov	si,fake_table
		call	exec_sub_menu
		mov	[fake_table+2],ax	;Temp. variable
		pop	cx
		call	set_cursor

		call	popr
		mov	ax,[fake_table+2]
		cmp	ax,100h
		ret

;=============================================================================

;~ifndef __ACT__
;~ReadString	proc
;~	; Expects: al=x, ah=y, di=@buffer, cl=width, ch=maxlen, si=@title
;~		push	bx dx
;~		mov	bx,ax
;~		mov	dx,ax
;~		add	dl,cl
;~		add	dl,5
;~		add	dh,2
;~		push	bx dx
;~		push	ax
;~		mov	ah,atReadWindow
;~		call	OpenWindow
;~		pop	ax
;~		mov	bl,atReadString
;~		mov	bh,atsReadString
;~		call	ReadLine
;~		pop	dx bx
;~		call	CloseWindow
;~		pop	dx bx
;~		ret
;~endp
;~endif

;=============================================================================
; read_line
;-----------------------------------------------------------------------------
; Reads line.
; In:	AL -- column
;	AH -- row
;	BL -- color (normal)
;	BH -- color (selected)
;	CL -- length of string
;	CH -- max length of string
;	DI -> buffer
; Out:	AX -- ASCII/scan codes of key used to exit
; Modf:	AX
; Call:	pushr, get_addr, get_cursor, small_cursor, strlen, update_string,
;	read_key, check_exit_read, set_cursor, gotoxy, popr, [test_char]
; Use:	string_changed, read_line_attr, str_start, str_length,
;	max_str_length, str_buffer, read_line_cur, read_line_cur_c,
;	read_line_result
;
read_line:
		call	pushr
		push	cs
		pop	es
		mov	byte [string_changed],1
		mov	[read_line_attr],bx		;word!
		add	ax,0102h
		mov	[str_start],ax
		push	di
		call	get_addr
		pop	di
;		mov	bp,ax
		mov	[str_length],cl			;byte!
		mov	[max_str_length],ch		;byte!
		mov	[str_buffer],di
		mov	si,di
		add	si,[max_str_length]
		mov	byte [si],0
		push	cx
		call	get_cursor
		mov	[read_line_cur],cx
		mov	[read_line_cur_c],dx
		call	small_cursor
		pop	cx
;		mov	ch,0

		mov	si,di
		call	strlen

		xor	bx,bx
		xor	dx,dx
		jmp	@@next_1
@@next:
		mov	byte [string_changed],0
@@next_1:
		call	update_string
@@read:
		call	read_key
		call	check_exit_read
		jc	@@1
		jmp	@@ignore
@@1:
		cmp	ax,kbRight
		je	@@right
		cmp	ax,kbLeft
		je	@@left
		cmp	ax,kbHome
		jne	@@5
		jmp	@@home
@@5:
		cmp	ax,kbEnd
		jne	@@4
		jmp	@@end
@@4:
		cmp	ax,kbDel
		je	@@delete
		cmp	ax,kbBackSpace
		je	@@backspace
		cmp	bl,[max_str_length]		;byte!
		jae	@@next
		call	[test_char]
		jc	@@read
		cmp	byte [string_changed],1
		je	@@clear
@@cleared:
		push	cx
		mov	cx,[max_str_length]
		mov	di,[str_buffer]
		add	di,cx
		dec	di
		lea	si,[di-1]
		sub	cx,bx
		jz	@@3
		dec	cx
		std
	rep	movsb
@@3:
		cld
		mov	si,[str_buffer]
;		add	si,bx
		mov	[si+bx],al
		pop	cx
		cmp	cx,[max_str_length]
		jae	@@right
		inc	cx
@@right:
		cmp	bl,cl
		jne	@@2
@@loc_read:
		jmp	@@read
@@2:
		inc	bx
		mov	ax,bx
		sub	ax,dx
		cmp	ax,[str_length]
		jbe	@@loc_next
		inc	dx
		jmp	@@next
@@clear:
		mov	si,[str_buffer]
		mov	byte [si],0
		xor	cx,cx
		xor	bx,bx
		jmp	@@cleared
@@left:
		or	bl,bl
		je	@@loc_next
		dec	bx
		cmp	bx,dx
		jae	@@loc_next
		mov	dx,bx
@@loc_next:
		jmp	@@next
@@backspace:
		or	bx,bx
		je	@@loc_read
		dec	bx
@@delete:
		or	cx,cx
		je	@@loc_read
		cmp	bx,cx
		jae	@@loc_read
		mov	di,[str_buffer]
		add	di,bx
		lea	si,[di+1]
		push	cx
		mov	cx,[max_str_length]
		sub	cx,bx
		cld
	rep	movsb
		mov	al,0
		stosb
		pop	cx
		dec	cx
		or	dx,dx
		jz	@@loc_next
		mov	ax,cx
		sub	ax,dx
		cmp	ax,[str_length]
		jae	@@loc_next
		dec	dx
		jmp	@@next
@@home:
		xor	bx,bx
		xor	dx,dx
		jmp	@@next
@@end:
		mov	bx,cx
		cmp	bx,[str_length]
		jbe	@@loc_next
		mov	dx,bx
		sub	dx,[str_length]
		jmp	@@next

@@ignore:
;		mov	@@Result,0
;		jmp	@@Quit
@@enter:
		mov	[read_line_result],ax
@@quit:
		mov	cx,[read_line_cur]
		call	set_cursor
		mov	dx,[read_line_cur_c]
		call	gotoxy
		call	popr
		mov	ax,[read_line_result]
		ret
;=============================================================================
; update_string
;-----------------------------------------------------------------------------
; Updates string.
; In:	BX -- position of cursor
;	DX -- ???
; Out:	---
; Modf:	---
; Call:	pushr, gotoxy, popr
; Use:	str_start, video_seg, str_buffer, read_line_attr, string_changed,
;	read_line_sel_attr, str_length
;
update_string:
		call	pushr
		mov	ax,dx
		mov	dx,[str_start]
		add	dl,bl
		sub	dl,al
		inc	dl
		call	gotoxy
		mov	dx,ax
		mov	di,bp
		mov	es,[video_seg]
		mov	si,[str_buffer]
		add	si,ax
		mov	ah,[read_line_attr]
		xor	dx,dx
		mov	al,' '
		stosw
		cmp	byte [string_changed],0
		je	@@not_sel_1
		mov	ah,[read_line_sel_attr]
@@not_sel_1:
		jcxz	@@skip
		cmp	cx,[str_length]
		jbe	@@next
		mov	cx,[str_length]
@@next:
		lodsb
		stosw
		inc	dx
		loop	@@next
@@skip:
		mov	ah,[read_line_attr]
		sub	dx,[str_length]
		ja	@@quit
		mov	cx,dx
		neg	cx
		inc	cx
		mov	al,' '
	rep	stosw
@@quit:
		call	popr
		ret
;=============================================================================
; default_test_char
;-----------------------------------------------------------------------------
; ???
; In:	---
; Out:	---
; Modf:	AX, BX, CX, DX, SI, DI, BP
; Call:	---
; Use:	---
;
default_test_char:
;		cmp	al,0
;		je	@@1
;		clc
;		ret
;@@1:
;		stc
;		ret

		cmp	al,' '
		jb	@@exit
		cmp	ax,kbAltAlone
		cmc
@@exit:
		ret
;=============================================================================
; check_exit_read
;-----------------------------------------------------------------------------
; ???
; In:	---
; Out:	---
; Modf:	AX, BX, CX, DX, SI, DI, BP
; Call:	---
; Use:	---
;
check_exit_read:
		push	ax
		push	bx
		push	si
		mov	bx,ax
		mov	si,[read_exit_keys]
@@next:
		lodsw
		cmp	ax,bx
		je	@@quit
		or	ax,ax
		jnz	@@next
@@end:
		stc
@@quit:
		pop	si
		pop	bx
		pop	ax
		ret
;=============================================================================
; vert_menu
;-----------------------------------------------------------------------------
; Creates vertical menu.
; In:	AH/AL -- row/column of top left corner
;	CX -- number of items
;	BL -- normal color
;	BH -- selected color
;	DL -- selected item
;	DH -- width
;	DS:SI -> list of items (empty string -- separator)
; Out:	AL -- number of selected item:
;		>= 0 -- number of item
;		-1 -- ESC pressed
;		-2 -- Left pressed
;		-3 -- Right pressed
;	AH -- ???
; Modf:	AX
; Call:	---
; Use:	---
;
vert_menu:
		call	pushr
		push	word [keys]
		push	word [test_key]
		call	get_addr
		mov	es,[video_seg]
;		mov	bp,ax
		call	write_items
		mov	word [keys],menu_keys
		mov	word [test_key],vm_test_key
		call	key_loop
		cmp	ax,kbEsc
		je	@@cancel
		cmp	ax,kbLeft
		je	@@left
		cmp	ax,kbRight
		je	@@right
		mov	al,dl
		jmp	@@quit
@@cancel:
		mov	al,-1
@@quit:
		mov	[vert_menu_result],al
		mov	[vert_menu_result+1],dl
		pop	word [test_key]
		pop	word [keys]
		call	popr
		mov	ax,[vert_menu_result]
		ret
@@left:
		mov	al,-2
		jmp	@@quit
@@right:
		mov	al,-3
		jmp	@@quit

vert_menu_down:
		inc	dl
		cmp	dl,cl
		jb	vert_menu_ret
vert_menu_home:
		mov	dl,0
vert_menu_ret:
		jmp	write_items

vert_menu_up:
		dec	dl
		cmp	dl,-1
		jne	vert_menu_ret
vert_menu_end:
		mov	dl,cl
		dec	dl
		jmp	vert_menu_ret
;=============================================================================
; vm_test_key
;-----------------------------------------------------------------------------
; ???
; In:	---
; Out:	---
; Modf:	AX, BX, CX, DX, SI, DI, BP
; Call:	---
; Use:	---
;
vm_test_key:
		mov	al,[cur_test_code]		;byte!
		call	find_menu_letter
		jc	@@quit
		mov	dl,al
		call	write_items
		mov	ax,kbEnter
@@quit:
		ret
;=============================================================================
; write_items
;-----------------------------------------------------------------------------
; Writes item of vertical menu.
; In:	ES:BP -> top left byte of menu in video memory
;	CX -- number of items
;	BL -- normal color
;	BH -- selected color
;	DL -- selected item
;	DH -- width
;	DS:SI -> list of items (empty string -- separator)
; Out:	---
; Modf:	BX, DX, DI
; Call:	write_point
; Use:	point_options, normal_letter, accent_letter
;
write_items:
		push	ax
		push	cx
		push	si
		push	bp
		mov	al,0
@@next:
		push	cx
@@next_str:
		mov	di,bp
		mov	ah,bl
		cmp	byte [si],0
		je	@@separator
		cmp	word [point_options],0
		je	@@no_opt
		push	bx
		mov	bl,al
		mov	bh,0
		add	bx,bx
		add	bx,bx
		add	bx,[point_options]
		test	byte [bx],DISABLED
		pop	bx
		jz	@@no_opt
		mov	ch,atMenuNormDis
		mov	ah,atMenuNormDis
		cmp	al,dl
		jne	@@write
		mov	ch,atMenuSelDis
		mov	ah,atMenuSelDis
		jmp	@@write
@@no_opt:
		mov	ch,atMenuNormHot
		cmp	al,dl
		jne	@@write
		mov	ah,bh
		mov	ch,atMenuSelHot
@@write:
		call	write_point
		add	bp,SCR_COLS*2		;160
		inc	ax
		pop	cx
		loop	@@next

		pop	bp
		pop	si
		pop	cx
		pop	ax
		ret

@@separator:
;-----------------------------------------------------------------------------
; Write separator.
;  Borland-style:	Norton-style:
;  (old, as in TC 2.01)
;       . . . 		     . . .
;  ³ Item N      ³	³ Item N      ³
;  ÃÄÄÄÄÄÄÄÄÄÄÄÄÄ´	³ ÄÄÄÄÄÄÄÄÄÄÄ ³
;  ³ Item N+1    ³	³ Item N+1    ³
;       . . .		     . . .
;-----------------------------------------------------------------------------
		push	ax
		inc	si
%ifdef	CONFIG_BORLAND_MENU
		dec	di
		dec	di
		mov	ax,atMenuBorder*256+'Ã'
		stosw
		mov	al,'Ä'
		mov	cl,dh
		inc	cx
	rep	stosw
		mov	al,'´'
		stosw
%else	;CONFIG_BORLAND_MENU
		scasw			;Instead of ADD DI,2
		mov	ax,atMenuBorder*256+'Ä'
		mov	cl,dh
		dec	cx
	rep	stosw
%endif	;CONFIG_BORLAND_MENU
		add	bp,SCR_COLS*2
		pop	ax
		jmp	@@next_str
;=============================================================================
; write_point
;-----------------------------------------------------------------------------
; Writes menu item.
; In:	DS:SI -> string
;	ES:DI -> video memory
;	AH -- normal color
;	CH -- color of hot-key
;	DH -- width (0 if don't care)
; Out:	SI -> next string
; Modf:	SI, DI
; Call:	---
; Use:	---
;
write_point:

		push	ax
		push	bx
		push	cx
		cld
		mov	bl,ch
		mov	cl,dh
		mov	ch,0
		mov	al,' '
		stosw
@@next:
		lodsb
		cmp	al,0
		je	@@adjust
		cmp	al,'~'
		je	@@sw
;-----------------------------------------------------------------------------
		cmp	al,'`'
		jne	@@stosw
		push	bx
		mov	bx,cx
		call	point_len
		inc	cx
		inc	cx
		neg	cl
		add	cl,bl
		sub	bl,cl
		push	bx
		mov	al,' '
	rep	stosw
		pop	cx
		pop	bx
@@stosw:
;-----------------------------------------------------------------------------
		stosw
		dec	cx
		jmp	@@next
@@sw:
		xchg	ah,bl
		jmp	@@next
@@adjust:
		cmp	cx,0
		jle	@@quit
		mov	al,' '
	rep	stosw
@@quit:
		pop	cx
		pop	bx
		pop	ax
		ret
;=============================================================================
; find_menu_letter
;------------------------------------------------------------------------------
; Searches list of strings for hot key.
; In:	DS:SI -> list of strings
;	CX -- number of items
;	AL -- hot key to found
; Out:	CY -- hot key not found
;	NC -- hot key found:
;		AX -- number of item
; Modf:	AX
; Call:	upcase
; Use:	---
;
find_menu_letter:
		push	bx
		push	si
		call	upcase
		mov	ah,al
		xor	bx,bx
@@next:
		lodsb
		cmp	al,0
		je	@@end_point
		cmp	al,'~'
		jne	@@next
		lodsb
		inc	si
		call	upcase
		cmp	al,ah
		jne	@@next
		mov	ax,bx
		jmp	@@quit
@@end_point:
		cmp	byte [si],0
		jne	@@1
		inc	si
@@1:
		inc	bx
		cmp	bx,cx
		jb	@@next
		stc
@@quit:
		pop	si
		pop	bx
		ret
;=============================================================================
; error_message
;-----------------------------------------------------------------------------
; Creates window w/ error message and waits for a key.
; In:	DS:SI -> message
;	[exec_msg_type] -- options:
;		0 -- open window, wait for a key, close window
;		1 -- open window only
;		2 -- close window only
; Out:	---
; Modf:	---
; Call:	exec_message
; Use:	msg_title, error_title, msg_box_attr, message_attr
;
error_message:
		mov	word [msg_title],error_title
		mov	byte [msg_box_attr],atErrorBox
		mov	byte [message_attr],atErrorMsg
		jmp	exec_message
;=============================================================================
; message_box
;-----------------------------------------------------------------------------
; Creates window w/ message and waits for a key.
; In:	DS:SI -> message
;	[exec_msg_type] -- options:
;		0 -- open window, wait for a key, close window
;		1 -- open window only
;		2 -- close window only
; Out:	---
; Modf:	---
; Call:	exec_message
; Use:	msg_title, message_title, msg_box_attr, message_attr
;
message_box:
		mov	word [msg_title],message_title
		mov	byte [msg_box_attr],atMessageBox
		mov	byte [message_attr],atMessage
;=============================================================================
; exec_message
;------------------------------------------------------------------------------
; Creates window w/ message. If length of the message less than 20 chars,
; width of window is set to 23 chars.
; In:	DS:SI -> message
;	[msg_title] -> title (see open_window for details)
;	[msg_box_attr] -- color of window
;	[message_attr] -- color of message
;	[exec_msg_type] -- options:
;		0 -- open window, wait for a key, close window
;		1 -- open window only
;		2 -- close window only
; Out:	---
; Modf:	---
; Call:	beep, pushr, get_cursor, no_sursor, strlen, open_window, write_string,
;	flush_key, read_key, close_window, set_cursor, popr
; Use:	msg_title, message_title, msg_box_attr, message_attr
;
exec_message:
		call	beep
		call	pushr

		cmp	byte [exec_msg_type],1
		ja	@@skip_open

		call	get_cursor
		mov	[exec_message_cx],cx
		call	no_cursor
		mov	bh,10
		mov	dh,12
		call	strlen
		shr	cx,1
		mov	al,39
		sub	al,cl		; al=message X
		cmp	cl,10
		jae	@@1
		mov	cl,10

@@1:
		add	cl,2
		mov	bl,39
		sub	bl,cl
		mov	dl,39
		add	dl,cl

		mov	ah,[msg_box_attr]

		mov	bp,si
		mov	si,[msg_title]

		push	ax
		mov	al,1

		call	open_window
		pop	ax

		mov	ah,11
		mov	bh,[message_attr]
		mov	si,bp
		call	write_string

@@skip_open:
		cmp	byte [exec_msg_type],1
		je	@@skip_close

		cmp	byte [exec_msg_type],2
		je	@@skip_read_key

		call	flush_key
		call	read_key

@@skip_read_key:
		call	close_window
		mov	cx,[exec_message_cx]
		call	set_cursor

@@skip_close:
		call	popr
		ret
;=============================================================================
; open_window
;-----------------------------------------------------------------------------
; Creates window w/ double border.
; In:	BH/BL -- row/column of top left corner
;	DH/DL -- row/column of lower right corner
;	AH -- color of window
;	AL -- mode:
;		0 -- do not save text under window in buffer
;		any other value -- save text under window in buffer
;	SI -> title. Format of title:
;		length-of-string: byte, color: byte, string: ? bytes
; Out:	---
; Modf:	---
; Call:	pushr, open_window_2
; Use:	---
;
open_window:
		call	pushr
		mov	cx,double_border
		jmp	open_window_2
;=============================================================================
; open_window_bord
;-----------------------------------------------------------------------------
; Creates window w/ arbitary border.
; In:	BH/BL -- row/column of top left corner
;	DH/DL -- row/column of lower right corner
;	AH -- color of window
;	AL -- mode:
;		0 -- do not save text under window in buffer
;		any other value -- save text under window in buffer
;	SI -> title. Format of title:
;		length-of-string: byte, color: byte, string: ? bytes
;	CX -> border. Format of border:
;		8 bytes. For example: 'ÉÍ»ººÈÍ¼'.
; Out:	---
; Modf:	---
; Call:	pushr, save_window, get_addr, popr
; Use:	owb_bp, video_seg
;
open_window_bord:
		call	pushr
open_window_2:
		mov	bp,cx
		push	ax
		or	al,al
		jz	@@skip_save_window
		push	dx
		add	dx,0102h
		call	save_window
		pop	dx
@@skip_save_window:
		mov	ax,bx
		push	bp
		call	get_addr
		pop	bp
;		mov	di,ax
		mov	[owb_bp],ax
		mov	es,[video_seg]
		pop	ax
		mov	cl,dl
		sub	cl,bl
		mov	ch,0
		dec	cx
		mov	al,[ds:bp]	;'É'
		stosw

		push	cx
		sub	cl,[si]
		shr	cx,1
		mov	bl,cl
		mov	al,[ds:bp+1]	;'Í'
	rep	stosw

		push	ax
		lodsw
		add	bl,al
		mov	cl,al
		jcxz	@@skip_2
@@next_2:
		lodsb
		stosw
		loop	@@next_2
@@skip_2:
		pop	ax
		pop	cx
		push	cx
		sub	cl,bl
		mov	al,[ds:bp+1]	;'Í'
	rep	stosw
		pop	cx
		mov	al,[ds:bp+2]	;'»'
		stosw
		inc	bh
@@next:
		cmp	bh,dh
		je	@@end_loop
		inc	bh
		add	word [owb_bp],SCR_COLS*2	;160
		mov	di,[owb_bp]
		mov	al,[ds:bp+3]	;'º'
		stosw
		push	cx
		mov	al,' '
	rep	stosw
		pop	cx
		mov	al,[ds:bp+4]	;'º'
		stosw
		mov	al,atShadow
		inc	di
		stosb
		inc	di
		stosb

		jmp	@@next
@@end_loop:
		add	word [owb_bp],SCR_COLS*2	;160
		mov	di,[owb_bp]
		mov	al,[ds:bp+5]	;'È'
		stosw
		mov	al,[ds:bp+6]	;'Í'
		push	cx
	rep	stosw
		pop	cx
		mov	al,[ds:bp+7]	;'¼'
		stosw
		mov	al,atShadow
		inc	di
		stosb
		inc	di
		stosb

		add	word [owb_bp],SCR_COLS*2+5	;165
		mov	di,[owb_bp]
;		mov	al,atShadow	;AL == atShadow...
		inc	cx
		inc	cx
@@next_1:
		stosb
		inc	di
		loop	@@next_1

		call	popr
		ret
;=============================================================================
; save_window
;-----------------------------------------------------------------------------
; Saves rectangular area of screen.
; In:	BH/BL -- row/column of top left corner
;	DH/DL -- row/column of lower right corner
;	[buf_pos] -> buffer
; Out:	[buf_pos], [buf_pos_mark] -> end of saved data
; Modf:	---
; Call:	pushr, get_addr, popr
; Use:	buf_pos, video_seg, buf_pos_mark
;
save_window:
		call	pushr
		mov	ax,bx
		call	get_addr
		mov	cl,dl
		sub	cl,bl
		mov	ch,0		;CX == width
		inc	cx
		push	cs
		pop	es
		mov	di,[buf_pos]
		mov	ds,[video_seg]
		push	bx		;save left/top
		push	dx		;save right/bottom
@@next:
		mov	si,bp
		push	cx
	rep	movsw
		pop	cx
		add	bp,SCR_COLS*2
		inc	bh
		cmp	bh,dh
		jbe	@@next

		pop	ax		;buffer: °°°°°°°°rblt....
		stosw			;buf_pos: -----------^
		pop	ax
		stosw
		mov	[cs:buf_pos],di
		mov	[cs:buf_pos_mark],di
		call	popr
		ret
;=============================================================================
; close_window, restore_window
;-----------------------------------------------------------------------------
; Restores rectangular area of screen.
; In:	[buf_pos] -> end of buffer
; Out:	[buf_pos] -> buffer
; Modf:	---
; Call:	pushr, get_addr, popr
; Use:	buf_pos, video_seg
;
close_window:
restore_window:
		call	pushr
		mov	si,[buf_pos]
		std
		lodsw
		lodsw
		xchg	ax,bx
		lodsw
		mov	dx,ax

		call	get_addr
		mov	cl,dl
		sub	cl,bl
		mov	ch,0		; cx=width
		inc	cx
		mov	es,[video_seg]
		std			;get_addr set CF

@@next:
		mov	di,bp
		push	cx
	rep	movsw
		pop	cx
		sub	bp,SCR_COLS*2
		inc	bh
		cmp	bh,dh
		jbe	@@next

		cld
		lodsw
		mov	[buf_pos],si
		call	popr
		ret
;=============================================================================
; write_string
;-----------------------------------------------------------------------------
; Writes zero-terminated string directly to video memory.
; In:	AX -- coordinates of string
;	DS:SI -> string
;	BH -- color
; Out:	---
; Modf:	AX, SI
; Call:	get_addr
; Use:	video_seg
;
write_string:
		push	di
		push	bp
		push	es
		call	get_addr
		mov	es,[video_seg]
		mov	ah,bh
@@next:
		lodsb
		cmp	al,0
		je	@@quit
		stosw
		jmp	@@next
@@quit:
		pop	es
		pop	bp
		pop	di
		ret
;=============================================================================
; fill_word, fill_byte
;-----------------------------------------------------------------------------
; Stores hexadecimal representation of byte or word in video memory.
; In:	fill_byte:
;		AL -- byte
;	fill_word:
;		AX -- word
;	ES:DI -> video memory
;	BH -- color (attribute)
; Out:	---
; Modf:	DI
; Call:	fill_byte
; Use:	---
;
fill_word:
		xchg	al,ah
		call	fill_byte
		xchg	al,ah

fill_byte:
		push	ax
		push	cx
		mov	ah,bh
		mov	cl,4
@@next:
		push	ax
		shr	al,cl
		and	al,0fh
		cmp	al,10
		sbb	al,69h
		das
		stosw
		pop	ax
		sub	cl,4
		jz	@@next

		pop	cx
		pop	ax
		ret
;=============================================================================
; get_addr
;-----------------------------------------------------------------------------
; Returns address (offset) of character in video memory.
; In:	AL -- column
;	AH -- row
; Out:	AX, DI, BP -- offset
;	DF cleared
; Modf:	AX, DI, BP
; Call:	---
; Use:	---
;
get_addr:
		push	bx
		mov	bx,ax
		mov	al,SCR_COLS*2
		mul	bh
		mov	bh,0
		add	ax,bx
		add	ax,bx
		pop	bx
		mov	di,ax
		mov	bp,ax
		cld
		ret
;=============================================================================
; gotoxy
;-----------------------------------------------------------------------------
; Changes position of cursor.
; In:	DL -- column
;	DH -- row
; Out:	---
; Modf:	---
; Call:	---
; Use:	---
;
gotoxy:
		push	ax
		push	bx
		mov	ah,2
		mov	bh,0
		int	10h
		pop	bx
		pop	ax
		ret
;=============================================================================
; get_cursor
;-----------------------------------------------------------------------------
; Returns position of cursor.
; In:	---
; Out:	DL -- column
;	DH -- row
;	CX -- shape
; Modf:	CX, DX
; Call:	---
; Use:	---
;
get_cursor:
		push	ax
		push	bx
		mov	ah,3
		mov	bh,0
		int	10h
		pop	bx
		pop	ax
		ret
;=============================================================================
; no_cursor, small_cursor, large_cursor, set_cursor
;-----------------------------------------------------------------------------
; Turns cursor off, sets cursor like '_', like 'Û', or reads parameters from CX
; In:	set_cursor:
;		CX -- shape
;	others:
;		---
; Out:	---
; Modf:	---
; Call:	set_cursor
; Use:	---
;
no_cursor:
		mov	cx,2020h	;### 2000h ?
		jmp	set_cursor
;
small_cursor:
		mov	cx,0607h	;### 0d0eh ?
		jmp	set_cursor
;
large_cursor:
		mov	cx,000dh	;### 000fh ?
;
set_cursor:
		push	ax
		mov	ah,1
		int	10h
		pop	ax
		ret
;=============================================================================
; write_message
;-----------------------------------------------------------------------------
; Writes $-terminated string to stdout.
; In:	DS:DX -> string
; Out:	---
; Modf:	AX
; Call:	---
; Use:	---
;
write_message:
		mov	ah,9
		int	21h
		ret
;=============================================================================
; E0F
;=============================================================================

