;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; dialogs.inc
; Procedures for creating dialog windows.
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
; 2007-02-15	0leg	Added comments to all procedures. All variables moved
;			to act[u]data.inc
; 2007-03-18	0leg	Changes in init_dialog, exec_dialog to support
;			optimized data structures.
; 2007-11-11	0leg	init_list/draw_list: highlight default string
;=============================================================================


;=============================================================================
; init_dialog
;-----------------------------------------------------------------------------
; Draws dialog window.
; In:	DS:SI -> dialog window structure.
;		Format of dialog window structure:
;			db	left, top, right, bottom ;coordinates
;			dw	ptr-to-title
;			db	number-of-elements
;			db	number-of-initial-element (0-based)
;
;			Data of 1st element
;			...
;			Data of 2nd element
;			...
;
;		Format of header of element's data:
;			db	type-of-element (eg. PUSH_BUTTON)
;			db	???
;
; Out:	---
; Modf:	---
; Call:	pushr, open_window, popr, init_*
; Use:	dlg_win_a, dlg_win_b, video_seg, dlg_items, dlg_active_item,
;	dlg_items_data, items_table
;
init_dialog:
		call	pushr
		cld
		lodsw			;Top/left
		mov	[dlg_win_a],ax
		mov	bx,ax
		lodsw			;Bottom/right
		mov	[dlg_win_b],ax
		mov	dx,ax
		lodsw			;Ptr to title
		push	si
		xchg	si,ax		;Instead of MOV SI,AX
		mov	ax,atDialogWindow*256	;AL==0: Don't save window
		call	open_window
		pop	si
		mov	es,[video_seg]
		lodsb			;Number of elements
		cbw
		mov	cx,ax
		mov	[dlg_items],ax
		jcxz	@@quit		;Exit if no elements

		mov	[dlg_active_item],si	;Ptr to number of active elem.
		lodsb				;Skip 1 byte
						;Now SI points to data of
						;1st element
		mov	di,dlg_items_data
@@next:
		lodsb			;index in items_table
		cbw
		add	ax,items_table
		xchg	ax,bx
		inc	si
		mov	[di],si		;SI -> data for init_*
		scasw
		call	[bx]		;call `init' proc, on entry:
		loop	@@next		;SI -> data;  on exit:
@@quit:					;SI -> next table (ie. data skipped)
		call	popr
		ret
;=============================================================================
; exec_dialog, exec_dialog_
;-----------------------------------------------------------------------------
; Executes dialog.
; In:	[dlg_win_a] -- top left corner
;	[dlg_win_b] -- lower right corner
;	[dlg_items] -- number of items in dialog window
;	[dlg_active_item] -> number of active item
;	dlg_items_data -- array of pointers to data of items
; Out:	---
; Modf:	exec_dialog:
;		AX, SI
;	exec_dialog_:
;		AX
; Call:	pushr, get_cursor, no_cursor, set_cursor, gotoxy, popr, run_*
; Use:	dlg_active_item, exec_dialog_cx, exec_dialog_dx, video_seg, dlg_items,
;	dlg_items_data, exec_dialog_res, run_push_button, run_static_text,
;	items_table
;
exec_dialog:
		mov	si,[dlg_active_item]
		mov	byte [si],0

exec_dialog_:
		call	pushr
		call	get_cursor
		push	dx
		push	cx
		call	no_cursor
		mov	es,[video_seg]
		cld
		mov	cx,[dlg_items]
		jcxz	@@quit

		mov	bx,[dlg_active_item]
		mov	bl,[bx]		;Number of active element
		mov	bh,0

@@next:
		push	cx
		call	no_cursor
		pop	cx
		mov	di,bx
		add	di,di
		mov	si,[dlg_items_data+di]	;Ptr to real data of element

		mov	al,[si-2]
		cbw
		add	ax,items_table+2
		push	bx
		xchg	ax,bx
		call	[bx]			;`run_*'
		pop	bx

		or	ax,ax			;ESC?
		jz	@@quit			;Yes, exit
		cmp	ax,1
		je	@@enter
@@find_next:
		cmp	ax,2			;Tab
		je	@@inc
		or	bx,bx			;First element?
		jz	@@end			;Yes, move to last element
		dec	bx			;(Shift+Tab)
		jmp	@@check_static
@@end:
		mov	bx,cx
		dec	bx
		jmp	@@check_static
@@inc:
		inc	bx
		cmp	bx,cx
		jb	@@check_static
@@home:
		xor	bx,bx

@@check_static:
		mov	di,bx
		add	di,di
		mov	si,[dlg_items_data+di]
		cmp	byte [si-2],STATIC_TEXT
		jne	@@next
		jmp	@@find_next

@@quit:
		xor	ax,ax		; CM_CANCEL
		mov	si,[dlg_active_item]
		mov	bl,[si]

@@exit:
		mov	si,[dlg_active_item]
		mov	[si],bl

@@restore_cursor:
		mov	[exec_dialog_res],ax
		pop	cx
		call	set_cursor
		pop	dx
		call	gotoxy
		call	popr
		mov	ax,[exec_dialog_res]
		or	ax,ax
		ret
@@enter:
		mov	ax,CM_DEFAULT

		cmp	byte [si-2],PUSH_BUTTON
		jne	@@exit
		mov	ax,[si+5]	;return CM_* if pushbutton was pressed
		jmp	@@exit
;=============================================================================
; init_push_button
;-----------------------------------------------------------------------------
; Initializes button.
; In:	DS:SI -> push button structure
;		Format of push button structure (7 bytes):
;			db	column, row	;relative coordinates
;			db	width-of-button
;			dw	ptr-to-label
;			dw	number-of-command
;	ES -- segment of video buffer
; Out:	SI -> structure of next element
; Modf:	BX, DL, SI
; Call:	draw_push_button
; Use:	---
;
init_push_button:
		mov	bh,atPushButton
		mov	bl,atlPushButton
		test	byte [si-1],BF_DEFAULT
		jz	@@1
		mov	bh,atPushButtonDef
		mov	bl,atlPushButtonDef
@@1:
		mov	dl,0
		call	draw_push_button
		add	si,7
		ret
;=============================================================================
; draw_push_button
;-----------------------------------------------------------------------------
; Draws button.
; In:	DS:SI -> push button structure
;	BL -- color of hot letter
;	BH -- color of text
;	DL -- flag:
;		0 -- draw normal button
;		1 -- draw pressed button
;	ES -- segment of video buffer
; Out:	---
; Modf:	---
; Call:	pushr, get_addr, point_len, draw_text, popr
; Use:	dlg_win_a
;
draw_push_button:
		call	pushr
		lodsw			;Coordinates
		add	ax,[dlg_win_a]
		call	get_addr
		lodsb			;Width
		cbw
		mov	cx,ax
		push	cx
		mov	al,' '
		or	dl,dl
		jz	@@2
		mov	ah,atDialogWindow
		stosw
@@2:
		mov	ah,bh
	rep	stosw
		mov	ah,atButtonShadow
		or	dl,dl
		jnz	@@3
		mov	al,'Ü'
		stosw
		mov	al,'ß'
@@3:
		mov	di,bp
		add	di,(SCR_COLS+1)*2	;162
		pop	cx
		push	cx
	rep	stosw
		lodsw			;Ptr to label
		mov	si,ax
		call	point_len
		pop	ax
		shr	ax,1
		shr	cx,1
		sub	ax,cx
		jns	@@1
		xor	ax,ax
@@1:
		add	al,dl
		mov	di,bp
		add	di,ax
		add	di,ax
		mov	ah,bh
		call	draw_text
		call	popr
		ret
;=============================================================================
; draw_text
;-----------------------------------------------------------------------------
; Prints ASCIIZ-string using another color for chars enclosed in tildes.
; In:	DS:SI -> string
;	AH -- main color
;	BL -- color for highlighting
;	ES:DI -> video buffer
; Out:	---
; Modf:	AX, SI, DI
; Call:	---
; Use:	---
;
draw_text:
@@next:
		lodsb
		cmp	al,0
		je	@@done
		cmp	al,'~'
		jne	@@store
		xchg	ah,bl
		jmp	@@next
@@store:
		stosw
		jmp	@@next
@@done:
		ret
;=============================================================================
; run_push_button
;-----------------------------------------------------------------------------
; Runs push button.
; In:	DS:SI -> push button structure
;	ES -- segment of video buffer
; Out:	AX -- result:
;		0 -- ESC
;		1 -- Enter
;		2 -- Tab
;		3 -- Shift+Tab
; Modf:	AX, DX
; Call:	draw_push_button, read_key, check_move_key
; Use:	---
;
run_push_button:
		push	bx
		push	cx
		mov	bh,atsPushButton
		mov	bl,atlsPushButton
		mov	dl,0
		call	draw_push_button
@@again:
		call	read_key
		call	check_move_key
		jc	@@again

		mov	bh,atPushButton
		mov	bl,atlPushButton
		cmp	ax,1		;Enter?
		jne	@@quit
		mov	dl,1		;Draw pressed button
		call	draw_push_button
		xor	cx,cx		;\ Replace !!!
		loop	$		;/
@@quit:
		test	byte [si-1],BF_DEFAULT
		jz	@@1
		mov	bh,atPushButtonDef
		mov	bl,atlPushButtonDef
@@1:
		mov	dl,0		;Redraw button
		call	draw_push_button
		pop	cx
		pop	bx
		ret
;=============================================================================
; init_input_line
;-----------------------------------------------------------------------------
; Initializes input line.
; In:	DS:SI -> input line structure
;		Format of input line structure (8 bytes):
;			db	column, row	;relative coordinates
;			dw	ptr-to-label
;			db	width-of-input_line, length-of-string
;			dw	ptr-to-buffer
;	ES -- segment of video buffer
; Out:	SI -> structure of next element
; Modf:	SI
; Call:	draw_input_line
; Use:	---
;
init_input_line:
		call	draw_input_line
		add	si,8
		ret
;=============================================================================
; draw_input_line
;-----------------------------------------------------------------------------
; Draws input line.
; In:	DS:SI -> input line structure
;	ES -- segment of video buffer
; Out:	---
; Modf:	---
; Call:	pushr, get_addr, draw_label, popr
; Use:	dlg_win_a
;
draw_input_line:
		call	pushr
		lodsw			;Coordinates
		add	ax,[dlg_win_a]
		call	get_addr
		lodsw			;Ptr to label
		mov	dl,0		;Normal label
		call	draw_label
		mov	ax,atInputLine*256+' '
		stosw
		lodsw			;Width of input line
		cbw
		mov	cx,ax
		lodsw			;Ptr to buffer
		mov	si,ax
		mov	ah,atInputLine
@@next:
		lodsb
		cmp	al,0
		je	@@done
		stosw
		loop	@@next
@@done:
		mov	al,' '
		inc	cx
	rep	stosw
		call	popr
		ret
;=============================================================================
; run_input_line
;-----------------------------------------------------------------------------
; Runs input line.
; In:	DS:SI -> input line structure
;	ES -- segment of video buffer
; Out:	AX -- result:
;		0 -- ESC
;		1 -- Enter
;		2 -- Tab
;		3 -- Shift+Tab
; Modf:	AX, DI
; Call:	get_addr, draw_label, read_line, check_move_key, draw_input_line
; Use:	read_exit_keys, dlg_win_a
;
run_input_line:
		push	word [read_exit_keys]
		push	bx
		push	cx
		push	si
		push	bp
		mov	word [read_exit_keys],inp_line_exit_keys
		lodsw			;Coordinates
		add	ax,[dlg_win_a]
		push	ax
		call	get_addr
		lodsw			;Label
		mov	dl,1		;Selected
		call	draw_label
		lodsw			;Width/length
		mov	cx,ax
		lodsw			;Ptr to buffer
		mov	di,ax
		pop	ax
		sub	ax,0102h
		mov	bx,atsInputLine*256+atInputLine
		call	read_line
		call	check_move_key
		pop	bp
		pop	si
		pop	cx
		pop	bx
		call	draw_input_line
		pop	word [read_exit_keys]
		ret
;=============================================================================
; draw_label
;-----------------------------------------------------------------------------
; Draws label.
; In:	AX -> string
;	ES:DI -> video memory. Label will be written one row above.
;	DL -- flag:
;		0 -- draw normal label
;		not 0 -- draw selected label
; Out:	---
; Modf:	---
; Call:	pushr, draw_text, popr
; Use:	---
;
draw_label:
		call	pushr
		sub	di,SCR_COLS*2	;160
		mov	si,ax
		mov	ah,atLabel
		mov	bl,atlLabel
		or	dl,dl
		jz	@@1
		mov	ah,atsLabel
		mov	bl,atlsLabel
@@1:
		call	draw_text
		call	popr
		ret
;=============================================================================
; init_list
;-----------------------------------------------------------------------------
; Initializes string list.
; In:	DS:SI -> string list structure
;		Format of string list structure (8 bytes):
;			db	column, row	;relative coordinates
;			dw	ptr-to-label
;			dw	height-of-list
;			dw	ptr-to-input-line-structure	;If 0, then
;							input line not used
;	ES -- segment of video buffer
; Out:	SI -> structure of next element
; Out:	---
; Modf:	DL, SI
; Call:	draw_list
; Use:	list_choice, list_first
;
init_list:
		mov	word [list_choice],-1
		mov	word [list_first],0
		mov	word [cur_str],0
		mov	dl,0
		call	draw_list
		add	si,8
		ret
;=============================================================================
; draw_list
;-----------------------------------------------------------------------------
; Draws string list.
; In:	DS:SI -> string list structure
;	ES -- segment of video buffer
;	DL -- flag:
;		0 -- draw normal list
;		not 0 -- draw active list
;	[list_first] -- number of top string
;	[cur_str] -- number of current string
; Out:	---
; Modf:	---
; Call:	pushr, get_addr, draw_label, get_string_addr, popr
; Use:	dlg_win_a, list_start, list_height, list_first, string_size,
;	string_count, cur_str
;
draw_list:
		mov	[list_act],dl
		call	pushr
		lodsw			;Coordinates
		add	ax,[dlg_win_a]
		call	get_addr

		lodsw			;Ptr to label
		call	draw_label	;DL used

		lodsw			;Height of list
		mov	[list_height],ax
		lodsw			;Ptr to input line structure
		mov	[link_string],ax
		mov	ax,[list_first]
		call	get_string_addr
		mov	si,ax		;Ptr to first (top) string
		mov	cx,[string_size]
		mov	dx,[string_count]
		mov	bx,[list_height]
		cmp	dx,bx
		jbe	@@ns
		mov	dx,bx
@@ns:
		sub	bx,dx
		push	bx
		mov	bx,[list_first]
		or	dx,dx
		jz	@@clear
@@next_line:
		mov	ah,atListNormal
		cmp	bx,[cur_str]
		jne	@@2

		mov	ah,atListDefault
		cmp	byte [list_act],0
		je	@@2

		mov	ah,atListSelected
@@2:
		push	cx
		mov	di,bp
		mov	al,' '
		stosw
@@next1:
		lodsb
		stosw
		loop	@@next1

		pop	cx
		inc	bx
		add	bp,SCR_COLS*2
		dec	dx
		jnz	@@next_line
@@clear:
		pop	bx
		or	bx,bx
		jz	@@all
@@next_clear:
		mov	ah,atListNormal
		mov	di,bp
		push	cx
		inc	cx
		mov	al,' '
	rep	stosw
		pop	cx
		add	bp,SCR_COLS*2
		dec	bx
		jnz	@@next_clear
@@all:
		call	popr
		ret
;=============================================================================
; run_list
;-----------------------------------------------------------------------------
; Runs string list.
; In:	DS:SI -> string list structure
;	ES -- segment of video buffer
; Out:	AX -- result:
;		0 -- ESC
;		1 -- Enter
;		2 -- Tab
;		3 -- Shift+Tab
;	[list_choice] -- selected item
; Modf:	AX, DX, DI
; Call:	draw_list, read_key, check_move_key, get_string_addr, trim_spaces,
;	draw_input_line
; Use:	string_count, cur_str, list_first, list_height, list_choice, cur_str,
;	link_string, string_size
;
run_list:
		push	bx
		push	cx
;		xor	bx,bx
;		xor	cx,cx
		mov	bx,[cur_str]		;\ 2007-11-12
		mov	cx,[list_first]		;/
		mov	dx,[string_count]
@@again:
		call	@@set_link
		mov	[cur_str],bx
		mov	[list_first],cx
		push	dx
		mov	dl,1
		call	draw_list
		pop	dx
		call	read_key
		or	dx,dx
;		jnz	@@not_cmk
;		jmp	@@cmk
;@@not_cmk:
		jz	@@cmk		;!!!
		cmp	ax,kbDown
		jne	@@1
		inc	bx
		cmp	bx,dx
		jb	@@loc_correct
@@end:
		mov	bx,dx
		dec	bx
@@loc_correct:
		jmp	@@correct
@@1:
		cmp	ax,kbUp
		jne	@@2
		or	bx,bx
		jz	@@again
		dec	bx

		jmp	@@correct
@@2:
		cmp	ax,kbHome
		jne	@@3
@@home:
		xor	bx,bx
		jmp	@@correct
@@3:
		cmp	ax,kbEnd
		je	@@end
		cmp	ax,kbPgUp
		jne	@@4
		or	cx,cx
		jz	@@home
		cmp	cx,[list_height]
		ja	@@big_up
		xor	cx,cx
@@pg_corr:
		mov	bx,cx
		add	bx,[list_height]
		dec	bx
		jmp	@@correct
@@big_up:
		sub	cx,[list_height]
		jmp	@@pg_corr
@@4:
		cmp	ax,kbPgDn
		jne	@@cmk
		cmp	dx,[list_height]
		jbe	@@end
		push	cx
		add	cx,[list_height]
		cmp	cx,dx
		jne	@@5
		pop	cx
		jmp	@@end
@@5:
		add	cx,[list_height]
		cmp	cx,dx
		pop	cx
		jbe	@@big_down
		mov	cx,dx
		sub	cx,[list_height]
		mov	bx,cx
		jmp	@@correct
@@big_down:
		add	cx,[list_height]
		mov	bx,cx
		jmp	@@correct
@@cmk:
		call	check_move_key
		jc	@@loc_again
		cmp	ax,1
		jne	@@quit
		mov	[list_choice],bx
		call	@@set_link
@@quit:
		mov	[list_choice],bx	;0leg, 2007-02-08
;		mov	word [cur_str],1	;!!!
		mov	dl,0
		call	draw_list
		pop	cx
		pop	bx
		ret
@@correct:
		cmp	bx,cx
		jb	@@lower
		push	cx
		add	cx,[list_height]
		cmp	bx,cx
		pop	cx
		jb	@@loc_again
@@upper:
		mov	cx,bx
		sub	cx,[list_height]
		inc	cx
		jmp	@@again
@@lower:
		mov	cx,bx
@@loc_again:
		jmp	@@again
@@set_link:
		cmp	word [link_string],0
		je	@@skip_link
		or	dx,dx
		jz	@@skip_link
		push	ax
		push	cx
		push	si
		push	es
		push	cs
		pop	es
		mov	di,[link_string]
		push	di
		mov	di,[di+6]
		mov	ax,bx
		call	get_string_addr
		mov	si,ax
		mov	cx,[string_size]
		push	cx
	rep	movsb
		pop	cx
		call	trim_spaces
		pop	si
		pop	es
		call	draw_input_line
		pop	si
		pop	cx
		pop	ax
@@skip_link:
		ret
;=============================================================================
; trim_spaces
;-----------------------------------------------------------------------------
; Cuts trailing spaces and puts 0 at the end of string.
; In:	ES:DI -> end of string
;	CX -- length of string
; Out:	---
; Modf:	AX, CX
; Call:	---
; Use:	---
;
trim_spaces:
		push	di
		std
		dec	di
		mov	al,' '
	repe	scasb
		je	@@emp
		inc	di
@@emp:
		inc	di
		mov	al,0
		cld
		stosb
		pop	di
		ret
;=============================================================================
; init_string_list
;-----------------------------------------------------------------------------
; Initializes string list.
; In:	AX -- size of string
; Out:	---
; Modf:	---
; Call:	---
; Use:	string_size, string_count, string_ptr, string_buffer
;
init_string_list:
		mov	[string_size],ax
		mov	word [string_count],0
		mov	word [string_ptr],string_buffer
		ret
;=============================================================================
; add_string
;-----------------------------------------------------------------------------
; Adds string to list.
; In:	DS:SI -> string
; Out:	CY if string buffer is full
;	NC if string added successfully
; Modf:	AX
; Call:	pushr, popr
; Use:	string_buffer, string_size, string_ptr, string_count
;
add_string:
		mov	ax,string_buffer+STRING_BUF_SIZE
		sub	ax,[string_size]
		cmp	[string_ptr],ax
		jb	@@1
		stc
		ret
@@1:
		call	pushr
		push	cs
		pop	es
		mov	di,[string_ptr]
		cld
		mov	cx,[string_size]
@@next:
		lodsb
		cmp	al,0
		je	@@done
		stosb
		loop	@@next
@@end:
		mov	[string_ptr],di
		inc	word [string_count]
		call	popr
		clc
		ret
@@done:
		mov	al,' '
	rep	stosb
		jmp	@@end
;=============================================================================
; init_static_text
;-----------------------------------------------------------------------------
; Initializes static text.
; In:	DS:SI -> static text structure
;		Format of static text structure (2 bytes):
;			dw	ptr-to-array-of-label-structures
;
;		Format of array of label structures:
;			db	column1, row1	;relative coordinates
;			dw	ptr-to-1st-string
;			...
;			db	columnN, rowN	;relative coordinates
;			dw	ptr-to-Nth-string
;			...
;			dw	0		;End of array
;
;	ES -- segment of video buffer
; Out:	SI -> structure of next element
; Modf:	AX, DL
; Call:	get_addr, draw_label
; Use:	dlg_win_a
;
init_static_text:
		lodsw			;Ptr to array
		push	si
		push	di
		push	bp
		xchg	ax,si		;Instead of MOV SI,AX
@@next:
		lodsw			;Coordinates
		or	ax,ax		;0?
		jz	@@exit		;Yes, exit
		add	ax,[dlg_win_a]
		call	get_addr
		lodsw			;Ptr to string
		mov	dl,0		;Normal label
		call	draw_label
		jmp	@@next
@@exit:
		pop	bp
		pop	di
		pop	si
		ret
;=============================================================================
; run_static_text
;-----------------------------------------------------------------------------
; Runs static text.
; In:	---
; Out:	AX -- result: Tab pressed
; Modf:	AX
; Call:	---
; Use:	---
;
run_static_text:
		mov	ax,2
		ret
;=============================================================================
; init_check_boxes
;-----------------------------------------------------------------------------
; Initializes check boxes.
; In:	DS:SI -> check boxes structure
;		Format of check boxes structure:
;			db	column, row	;relative coordinates
;			dw	ptr-to-main-label
;			dw	number-of-checkboxes
;			dw	width-of-labels
;			dw	result: 16 bits, 0 bit -- 1st checkbox,
;				1 bit -- 2nd checkbox, etc.
;			dw	ptr-to-1st-label
;			...
;			dw	ptr-to-Nth-label
;
;	ES -- segment of video buffer
; Out:	SI -> structure of next element
; Modf:	DL, SI
; Call:	draw_check_boxes
; Use:	cb_cur, cb_si
;
init_check_boxes:
		mov	dl,0
		mov	byte [cb_cur],-1
		call	draw_check_boxes
		mov	si,[cb_si]	;Ptr to structure of next element
		ret
;=============================================================================
; draw_check_boxes
;-----------------------------------------------------------------------------
; Draws check boxes.
; In:	DS:SI -> check boxes structure
;	ES -- segment of video buffer
;	[cb_cur] -- current check box
; Out:	[cb_si] -> structure of next element
; Modf:	---
; Call:	pushr, get_addr, draw_label, draw_text, popr
; Use:	dlg_win_a, cb_cur, cb_max, cb_si
;
draw_check_boxes:
		call	pushr
		lodsw			;Coordinates
		add	ax,[dlg_win_a]
		call	get_addr
		lodsw			;Ptr to label
		call	draw_label
		lodsw			;Number of checkboxes
		mov	cx,ax
		sub	al,[cb_cur]
		mov	[cb_cur],al
		lodsw			;Width of labels
		mov	[cb_max],ax
		lodsw			;Result / initial settings
		xchg	ax,dx

@@next:
		lodsw			;Ptr to label
		push	si
		xchg	ax,si

		mov	ah,atCheckBoxes
		cmp	[cb_cur],cl	;Selected?
		jne	@@2
		mov	ah,atsCheckBoxes;Yes
@@2:
		mov	al,' '
		stosw
		mov	al,'['
		stosw
		mov	al,' '
		shr	dx,1
		jnc	@@1
		mov	al,'X'
@@1:
		stosw
		mov	al,']'
		stosw
		mov	al,' '
		stosw
		mov	bl,atlCheckBoxes
		push	cx
		mov	cx,di
		add	cx,[cb_max]
		add	cx,[cb_max]

		call	draw_text	;Draw label

		sub	cx,di
		shr	cx,1
		mov	al,' '
	rep	stosw
		pop	cx
		add	bp,SCR_COLS*2	;160	;Next line
		mov	di,bp
		pop	si		;Ptr to next label

		loop	@@next

		mov	[cb_si],si	;Ptr to structure of next element
		call	popr
		ret
;=============================================================================
; run_check_boxes
;-----------------------------------------------------------------------------
; Runs check boxes.
; In:	DS:SI -> check boxes structure
;	ES -- segment of video buffer
; Out:	AX -- result:
;		0 -- ESC
;		1 -- Enter
;		2 -- Tab
;		3 -- Shift+Tab
;	`result' field in check boxes structure contains result
; Modf:	AX, DX
; Call:	small_cursor, gotoxy, draw_check_boxes, read_key, check_move_key
; Use:	dlg_win_a, cb_cur
;
run_check_boxes:
		push	bx
		push	cx
		call	small_cursor
		xor	cx,cx
@@again:
		mov	dx,[si]		;Coordinates
		add	dx,[dlg_win_a]
		add	dh,cl		;Current checkbox
		inc	dx
		inc	dx
		call	gotoxy
		mov	dl,1
		mov	[cb_cur],cl	;Current checkbox
		call	draw_check_boxes

		call	read_key
		cmp	ax,kbDown
		jne	@@d
		inc	cx
		cmp	cx,[si+4]	;Number of check boxes
		jb	@@again
		xor	cx,cx
		jmp	@@again
@@d:
		cmp	ax,kbUp
		jne	@@u
		dec	cx
		jns	@@again
		mov	cx,[si+4]	;Number of check boxes
		dec	cx
		jmp	@@again
@@u:
		cmp	ax,kbSpace
		jne	@@s
		mov	ax,1
		shl	ax,cl
		xor	[si+8],ax	;Toggle one bit of result
		jmp	@@again
@@s:
		call	check_move_key
		jc	@@again
		mov	dl,0		;Inactive label
		mov	byte [cb_cur],-1;Inactive check boxes
		call	draw_check_boxes
		pop	cx
		pop	bx
		ret
;=============================================================================

;~ifndef __ACT__
;~;---------------------------- Radio Buttons -----------------------------------
;~;------------------------------------------------------------------------------
;~InitRadioButtons proc
;~		mov	dl,0
;~		call	DrawRadioButtons
;~		mov	si,CBSI
;~		ret
;~endp
;~
;~DrawRadioButtons proc
;~		call	pushr
;~		lodsw
;~		add	ax,DlgWinA
;~		call	get_addr
;~;		mov	di,ax
;~;		mov	bp,ax
;~		lodsw
;~		call	DrawLabel
;~		lodsw
;~		mov	cx,ax
;~;		sub	al,CBcur
;~;		mov	CBcur,al
;~		lodsw
;~		mov	CBmax,ax
;~		lodsw
;~		mov	bl,dl
;~		mov	dx,cx
;~		sub	dx,ax	; pos
;~		mov	dh,bl
;~@@Next:
;~		mov	ah,atRadioButtons
;~		mov	bh,' '
;~		cmp	dl,cl
;~		jne	@@2
;~		mov	bh,7
;~		or	dh,dh
;~		jz	@@2
;~		mov	ah,atsRadioButtons
;~@@2:
;~		mov	al,' '
;~		stosw
;~		mov	al,'('
;~		stosw
;~		mov	al,bh
;~		stosw
;~		mov	al,')'
;~		stosw
;~		mov	al,' '
;~		stosw
;~		mov	bl,atlRadioButtons
;~		push	cx
;~		mov	cx,di
;~		add	cx,CBmax
;~		add	cx,CBmax
;~		call	DrawText
;~		sub	cx,di
;~		shr	cx,1
;~		mov	al,' '
;~		rep	stosw
;~		pop	cx
;~		add	bp,160
;~		mov	di,bp
;~		loop	@@Next
;~
;~		mov	CBSI,si
;~		call	popr
;~		ret
;~endp
;~
;~RunRadioButtons	proc
;~		push	bx cx
;~		mov	cx,0607h
;~		call	set_cursor
;~		mov	cx,[si+8]
;~@@Again:
;~		mov	dx,[si]
;~		add	dx,DlgWinA
;~		add	dh,cl
;~		inc	dx
;~		inc	dx
;~		call	gotoxy
;~		mov	dl,1
;~		mov	[si+8],cx
;~		call	DrawRadioButtons
;~
;~		call	read_key
;~		cmp	ax,kbDown
;~		jne	@@D
;~		inc	cx
;~		cmp	cx,[si+4]
;~		jb	@@Again
;~		xor	cx,cx
;~		jmp	@@Again
;~@@D:
;~		cmp	ax,kbUp
;~		jne	@@U
;~		dec	cx
;~		jns	@@Again
;~		mov	cx,[si+4]
;~		dec	cx
;~		jmp	@@Again
;~@@U:
;~		call	CheckMoveKey
;~		jc	@@Again
;~		mov	dl,0
;~		mov	CBcur,-1
;~		call	DrawRadioButtons
;~		pop	cx bx
;~		ret
;~endp
;~
;~endif

;=============================================================================
; get_string_addr
;-----------------------------------------------------------------------------
; Returns address of string in string buffer.
; In:	AX -- number of string
; Out:	AX -> string
; Modf:	AX
; Call:	---
; Use:	string_size, string_buffer
;
get_string_addr:
		push	dx
		mul	word [string_size]
		add	ax,string_buffer
		pop	dx
		ret
;=============================================================================
; point_len
;-----------------------------------------------------------------------------
; Returns length of string, not counting tildes.
; In:	DS:SI -> string
; Out:	CX -- length
; Modf:	AL, CX
; Call:	---
; Use:	---
;
point_len:
		xor	cx,cx
		push	si
@@next:
		lodsb
		cmp	al,0
		je	@@done
		cmp	al,'~'
		je	@@next
		inc	cx
		jmp	@@next
@@done:
		pop	si
		ret
;=============================================================================
; check_move_key
;-----------------------------------------------------------------------------
; Checks if ESC, Enter, Tab or Shift+Tab was pressed.
; In:	AX -- ASCII/scan codes
; Out:	NC -- one of the following keys was pressed
;		AX -- result:
;			0 -- ESC
;			1 -- Enter
;			2 -- Tab
;			3 -- Shift+Tab
;	CY -- another key was pressed
; Modf:	AX
; Call:	---
; Use:	---
;
check_move_key:
		cmp	ax,kbEsc
		je	@@0
		cmp	ax,kbEnter
		je	@@1
		cmp	ax,kbTab
		je	@@2
		cmp	ax,kbShiftTab
		je	@@3
		stc
		ret
@@0:
		xor	ax,ax
		ret
@@1:
		mov	ax,1
		ret
@@2:
		mov	ax,2
		ret
@@3:
		mov	ax,3
		ret
;=============================================================================
; E0F
;=============================================================================


