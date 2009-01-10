;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; actudata.inc
; Data: uninitialized variables.
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
; 2007-02-08	0leg	first_mcb, string added.
; 2007-02-10	0leg	Some variables from windows.inc, keyboard.inc
;			moved here.
; 2007-03-13	0leg	Removed exec_message_bx, exec_message_dx.
; 2007-03-18	0leg	Removed exec_dialog_cx, exec_dialog_dx
;			zz_cpu -> system_cpu (was `CPU' in v1.10)
; 2007-11-11	0leg	list_act added, cur_str moved from actdata.inc
; 2008-01-02	0leg	...
; 2008-03-23	0leg	dump_address added
;=============================================================================


;=============================================================================
save_cpu_regs		resw	16*2	;Saved CPU regs, 16 regs * 2 words
cpu_regs		resw	16*2	;Current CPU regs

bp0			resb	SIZEOF_BREAKPOINT	;Temporary breakpoint
bp1			resb	MAX_BREAKPOINTS * SIZEOF_BREAKPOINT ;Breakpoints

save_int_01		resd	1	;
int_01_set		resb	1	;
save_int_03		resd	1	;
int_03_set		resb	1	;
save_int_09		resd	1	;
int_09_set		resb	1	;
save_int_16		resd	1	;
save_int_24		resd	1	;

save_sp			resw	1	;For saving SP in trace.inc

system_cpu		resb	1	;Type of CPU: 0 -- 8086, ..., 4 -- 486+

save_dta_seg		resw	1	;

cur_x			resw	1	;*Initial column of text in the Code wnd
cur_line		resw	1	;*Number of current line in the Code wnd
cur_line_ip		resw	1	;IP of current (selected) instruction
user_pid		resw	1	;Segment address of user program's PSP
memory_flag		resb	1	;
mem_disp_type		resb	1	;
mem_disp_offs		resw	1	;
mem_prefix		resw	1	;
cmd_text_size		resw	1	;
my_port_21		resb	1	;
user_port_21		resb	1	;

edit_mode		resb	1	;2008-03-23
quit_flag		resb	1	;*1 if exit from debugger requested
resident_mode		resb	1	;*1 if debugger is in TSR mode
code_mark_exist		resw	1	;
file_specified		resb	1	;*1 if program is loaded in the debugger
cmd_line_options	resb	1	;Command line options (bit array)
regs_mode		resb	1	;*1 if regs is in 32-bit mode
cur_line_brk		resb	1	;
video_type		resb	1	;Type of video card. 1: VGA, 0 otherwise

unasm_cur_ip		resw	1	;
cur_cmd			resb	1	;
cur_param		resw	1	;
word_flag		resb	1	;
dword_flag		resb	1	;
print_flag		resb	1	;
cur_prefix		resw	1	;
ignore_prefix		resb	1	;
unasm_seg		resw	1	;Segment of current instruction
inst_286		resb	1	;
op_size_flag		resb	1	;
addr_size_flag		resb	1	;
str_flag		resb	1	;
cur_rep			resw	1	;
jcnear			resb	1	;
db_flag			resb	1	;

follow_offs		resw	1	;
follow_seg		resw	1	;

data_watch_proc		resw	1	;
data_watch_temp		resw	1	;
data_watch_label	resw	1	;

search_offs		resw	1	;Initial offset and
search_seg		resw	1	;segment for search
search_len		resw	1	;Length of searched byte sequence
search_options		resw	1	;Search options (bit array)

dump_address		resw	1	;Current offset in the Dump window
dump_seg		resw	1	;Initial segment of the Dump window
dump_offs		resw	1	;Initial offset of the Dump window
new_dump_seg		resw	1	;Tmp var for update_dump_window
new_dump_offs		resw	1	;Tmp var for update_dump_window
dump_proc		resw	1	;Tmp var for update_dump_window

minus_flag		resb	1	;
mov_flag		resb	1	;
lxx_flag		resb	1	;
jump_flag		resb	1	;
param_count		resb	1	;
rep_pref		resb	1	;
far_value		resw	1	;

from_addr		resd	1	;Block: source address
to_addr			resd	1	;Destination address
count_value		resw	1	;Number of bytes to copy/move/etc.

magic_offs		resw	1	;*Index of next char in DOOM cheat code

;=============================================================================
; system.inc
;-----------------------------------------------------------------------------
keep_ip			resw	1	;For pushr and popr

;=============================================================================
; windows.inc
;-----------------------------------------------------------------------------
sub_struc		resw	1	;exec_sub_menu: ptr to number of items
sub_coord		resw	1	;exec_sub_menu: coordinates of l/t corner
exec_message_cx		resw	1	;exec_message: cursor shape
vert_menu_result	resw	1	;vert_menu: return value
msg_title		resw	1	;exec_message: ptr to wnd title
msg_box_attr		resb	1	;exec_message: color of window
message_attr		resb	1	;exec_message: color of message
owb_bp			resw	1	;open_window_bord: ???
read_line_attr		resb	1	;Normal color of input line
read_line_sel_attr	resb	1	;Color of unchanged input line
read_line_result	resw	1	;read_line: return value
read_line_cur		resw	1	;read_line: cursor shape
read_line_cur_c		resw	1	;read_line: cursor coord.
str_buffer		resw	1	;read_line, update_string: ptr to string buffer
str_start		resw	1	;read_line, update_string: coord. of input line
point_options		resw	1	;Ptr to list of menu procedures/submenus
string_changed		resb	1	;1 if input string is not changed yet
str_length		resw	1	;Length of input line `window'
max_str_length		resw	1	;Max length of input line

;=============================================================================
; keyboard.inc
;-----------------------------------------------------------------------------
key_loop_sub		resw	1	;Ptr to proc. called by key_loop
keys			resw	1	;Ptr to keys table for key_loop
cur_test_code		resw	1	;Key code if it isn't found in a table

;=============================================================================
; dialogs.inc
;-----------------------------------------------------------------------------
exec_dialog_res		resw	1	;exec_dialog: return value
cb_max:					;draw_check_boxes: width of labels
cb_si			resw	1	;{init|draw}_check_boxes: tmp var
cb_cur			resb	1	;-//-: number of current checkbox
string_count		resw	1	;Number of strings in string list
string_ptr		resw	1	;Ptr to buffer for string list
string_size		resw	1	;Length of string in string list
list_height		resw	1	;Height of string list
link_string		resw	1	;Ptr to input line struc. for string list
list_choice		resw	1	;run_list: selected item
list_first		resw	1	;draw_list: number of top string
cur_str			resw	1	;draw_list: number of current string
list_act		resb	1	;draw_list: flag: normal/active list
dlg_win_a		resw	1	;Coordinates ot top left corner of wnd
dlg_win_b		resw	1	;Coordinates ot lower right corner of wnd
dlg_items		resw	1	;Number of elements in a dialog window
dlg_active_item		resw	1	;Ptr to number of active element
dlg_items_data		resw	MAX_ITEMS ;Array of pointers to data of items

;=============================================================================

instr_table		resw	CPU_HEIGHT-1	;
asm_buffer		resb	20		;
buffer			resb	BUF_SIZE	;
name_buf		resb	MAX_LETTER+2	;
my_cmd_line		resb	80h		;
cmd_line_string		resb	80h		;
filename_string		resb	FILENAME_LEN+1	;
search_string		resb	SEARCH_FILL_LEN+1 ;
search_bin		resb	SEARCH_FILL_LEN+1 ;
filename		resb	128		;
asm_line		resb	MAX_ASM_SIZE+1	;

dump_addr_string	resb	ADDRESS_STRING_LENGTH+1	;
main_addr_string	resb	ADDRESS_STRING_LENGTH+1	;
search_addr_str:
from_string		resb	ADDRESS_STRING_LENGTH+1	;
to_string		resb	ADDRESS_STRING_LENGTH+1	;
count_string		resb	ADDRESS_STRING_LENGTH+1	;
delay_string		resb	ADDRESS_STRING_LENGTH+1	;
fill_string		resb	SEARCH_FILL_LEN+1	;
fill_bin		resb	SEARCH_FILL_LEN+1	;

interrupt_table		resb	1024		;Saved interrupt vectors table

;=============================================================================
; video.inc
;-----------------------------------------------------------------------------
old_mode_num		resb	1	;
old_cursor		resw	1	;
old_cur_shape		resw	1	;
old_video_regs		resb	79h	;
old_mode_num_2		resb	1	;
old_screen		resw	SCR_COLS*SCR_ROWS ;
font			resb	4096	;Saved font, but may be used for saving
					;the screen when /e switch specified

;=============================================================================
; tools.inc
;-----------------------------------------------------------------------------
first_mcb		resw	1	;Segment of 1st MCB
string			resb	45	;String for list_memory_blocks

;=============================================================================
; E0F
;=============================================================================

