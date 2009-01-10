;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; actdlgs.inc
; Data: dialog windows.
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
; 2007-02-06	0leg	animate_dialog, animate_title, delay_label added
; 2007-02-08	0leg	list_mem_blocks_dialog, mem_blks_label,
;			mem_blks_title, dump_button added
;=============================================================================


;=============================================================================
about_dialog:
		db	15, 2, 65, 21
		dw	about_title
		db	2
		db	0

		db	STATIC_TEXT
		db	0
		dw	about_labels

		db	PUSH_BUTTON
		db	0
		db	20, 17
		db	10
		dw	ok_button
		dw	CM_OK
;=============================================================================
info_dialog:
		db	16, 4, 62, 20
		dw	info_title
		db	2
		db	0

		db	STATIC_TEXT
		db	0
		dw	info_labels

		db	PUSH_BUTTON
		db	0
		db	18, 14
		db	10
		dw	ok_button
		dw	CM_OK
;=============================================================================
search_dialog:
		db	14, 6, 65, 17
		dw	search_title
		db	5
search_dlg_item	db	0

		db	INPUT_LINE
		db	0
		db	3, 3
		dw	search_for_label
		db	44, SEARCH_FILL_LEN
		dw	search_string

		db	CHECK_BOXES
		db	0
		db	3, 6
		dw	options_label
		dw	4
		dw	15
search_bits	dw	0
		dw	ignore_case_check
		dw	backward_check
		dw	exclude_hma_check
		dw	result_code_check

		db	INPUT_LINE
		db	0
		db	25, 6
		dw	addr_name_label
		db	22, ADDRESS_STRING_LENGTH
		dw	search_addr_str

		db	PUSH_BUTTON
		db	0
		db	25, 9			; Button1
		db	10
		dw	search_button
		dw	CM_OK

		db	PUSH_BUTTON
		db	0
		db	38, 9			; Button2
		db	10
		dw	cancel_button
		dw	CM_CANCEL
;=============================================================================
copy_data_dialog:
		db	23, 6, 57, 17
		dw	copy_title
		db	5
copy_dlg_item	db	0

		db	INPUT_LINE
		db	0
		db	3, 3
		dw	from_label
		db	ADDR_STR_SIZE/2, ADDRESS_STRING_LENGTH
		dw	from_string

		db	INPUT_LINE
		db	0
		db	3, 6
		dw	to_label
		db	ADDR_STR_SIZE/2, ADDRESS_STRING_LENGTH
		dw	to_string

		db	INPUT_LINE
		db	0
		db	3, 9
		dw	count_label
		db	ADDR_STR_SIZE/2, ADDRESS_STRING_LENGTH
		dw	count_string

		db	PUSH_BUTTON
		db	0
		db	21, 3			; Button1
		db	10
		dw	ok_button
		dw	CM_OK

		db	PUSH_BUTTON
		db	0
		db	21, 5			; Button2
		db	10
		dw	cancel_button
		dw	CM_CANCEL
;=============================================================================
fill_data_dialog:
		db	22, 6, 58, 17
		dw	fill_title
		db	5
fill_dlg_item	db	0

		db	INPUT_LINE
		db	0
		db	3, 3
		dw	from_label
		db	ADDR_STR_SIZE/2, ADDRESS_STRING_LENGTH
		dw	to_string

		db	INPUT_LINE
		db	0
		db	20, 3
		dw	count_label
		db	ADDR_STR_SIZE/2, ADDRESS_STRING_LENGTH
		dw	count_string

		db	INPUT_LINE
		db	0
		db	3, 6
		dw	fill_label
		db	29, SEARCH_FILL_LEN
		dw	fill_string

		db	PUSH_BUTTON
		db	0
		db	11, 9			; Button1
		db	10
		dw	ok_button
		dw	CM_OK

		db	PUSH_BUTTON
		db	0
		db	23, 9			; Button2
		db	10
		dw	cancel_button
		dw	CM_CANCEL
;=============================================================================
write_data_dialog:
read_data_dialog:
		db	22, 6, 58, 17
block_dlg_title	dw	fill_title
		db	5
block_dlg_item	db	0

		db	INPUT_LINE
		db	0
		db	3, 3
		dw	filename_label
		db	29, FILENAME_LEN
		dw	filename_string

		db	INPUT_LINE
		db	0
		db	3, 6
block_dlg_label	dw	from_label
		db	ADDR_STR_SIZE/2, ADDRESS_STRING_LENGTH
		dw	from_string

		db	INPUT_LINE
		db	0
		db	20, 6
		dw	count_label
		db	ADDR_STR_SIZE/2, ADDRESS_STRING_LENGTH
		dw	count_string

		db	PUSH_BUTTON
		db	0
		db	11, 9			; Button1
		db	10
		dw	ok_button
		dw	CM_OK

		db	PUSH_BUTTON
		db	0
		db	23, 9			; Button2
		db	10
		dw	cancel_button
		dw	CM_CANCEL
;=============================================================================
question_dialog:
		db	18, 8, 63, 15
		dw	question_title
		db	4
		db	0

		db	STATIC_TEXT
		db	0
		dw	question_labels

		db	PUSH_BUTTON
		db	0
		db	4, 5			; Button1
		db	10
		dw	append_button
		dw	CM_OK

		db	PUSH_BUTTON
		db	0
		db	17, 5			; Button1
		db	11
		dw	rewrite_button
		dw	CM_OK+1

		db	PUSH_BUTTON
		db	0
		db	31, 5			; Button2
		db	10
		dw	cancel_button
		dw	CM_CANCEL
;=============================================================================
arg_dialog:
		db	21, 8, 59, 15
		dw	arg_title
		db	3
		db	0

		db	INPUT_LINE
		db	0
		db	3, 3
		dw	arg_name_label
		db	31, 126
		dw	cmd_line_string

		db	PUSH_BUTTON
		db	0
		db	13, 5			; Button1
		db	10
		dw	ok_button
		dw	CM_OK

		db	PUSH_BUTTON
		db	0
		db	25, 5			; Button2
		db	10
		dw	cancel_button
		dw	CM_CANCEL
;=============================================================================
addr_dialog:
		db	20, 6, 51, 13
		dw	enter_addr_title
		db	3
		db	0

		db	INPUT_LINE
		db	0
		db	3, 3
		dw	addr_name_label
		db	ADDR_STR_SIZE, ADDRESS_STRING_LENGTH
d_addr_str	dw	main_addr_string

		db	PUSH_BUTTON
		db	0
		db	3, 5			; Button1
		db	10
		dw	go_button
		dw	CM_OK

		db	PUSH_BUTTON
		db	0
		db	18, 5			; Button2
		db	10
		dw	cancel_button
		dw	CM_CANCEL
;=============================================================================
load_dialog:
		db	16, 4, 54, 19
		dw	load_title
		db	4
		db	0

		db	INPUT_LINE
		db	0
list_string	db	5, 3
		dw	name_label
		db	27, FILENAME_LEN
		dw	filename

		db	STRING_LIST
		db	0
		db	5, 6
		dw	files_label
		dw	8
		dw	list_string

		db	PUSH_BUTTON
		db	0			; bfDefault
		db	24, 5			; Button1
		db	10
		dw	ok_button
		dw	CM_OK

		db	PUSH_BUTTON
		db	0
		db	24, 8			; Button2
		db	10
		dw	cancel_button
		dw	CM_CANCEL
;=============================================================================
info_labels:
		db	17, 4
		dw	main_title
		db	6, 7
		dw	info_str_1
		db	6, 8
		dw	info_str_2
		db	10, 11
		dw	info_str_3
		db	11, 12
		dw	info_str_4
		dw	0
;=============================================================================
about_labels:
		db	19, 4
		dw	main_title
		db	26-(about_str_2-about_str_1)/2, 6
		dw	about_str_1
		db	26-(about_str_3-about_str_2)/2, 7
		dw	about_str_2
		db	26-(about_str_4-about_str_3)/2, 8
		dw	about_str_3
		db	26-(about_str_5-about_str_4)/2, 10
		dw	about_str_4
		db	26-(about_str_6-about_str_5)/2, 11
		dw	about_str_5
		db	26-(about_str_7-about_str_6)/2, 12
		dw	about_str_6
		db	26-(about_str_8-about_str_7)/2, 14
		dw	about_str_7
		db	26-(about_str_9-about_str_8)/2, 15
		dw	about_str_8
		db	26-(about_str_last-about_str_9)/2, 16
		dw	about_str_9
		dw	0
;=============================================================================
animate_dialog:
		db	24, 6, 55, 13
		dw	animate_title
		db	3
anim_dlg_item	db	0

		db	INPUT_LINE
		db	0
		db	3, 3
		dw	delay_label
		db	ADDR_STR_SIZE, ADDRESS_STRING_LENGTH
		dw	delay_string

		db	PUSH_BUTTON
		db	0
		db	3, 5			; Button1
		db	10
		dw	ok_button
		dw	CM_OK

		db	PUSH_BUTTON
		db	0
		db	18, 5			; Button2
		db	10
		dw	cancel_button
		dw	CM_CANCEL
;=============================================================================
list_mem_blocks_dialog:
		db	16, 4, 63, 19
		dw	mem_blks_title
		db	4
		db	0

		db	STRING_LIST
		db	0
		db	3, 2
		dw	mem_blks_label
		dw	10
		dw	0			;No linked string

		db	PUSH_BUTTON		;Dump
		db	0
		db	4, 13
		db	10
		dw	dump_button
		dw	1

		db	PUSH_BUTTON		;Go
		db	0
		db	18, 13
		db	10
		dw	go_button
		dw	2

		db	PUSH_BUTTON		;Cancel
		db	0
		db	32, 13
		db	10
		dw	cancel_button
		dw	CM_CANCEL
;=============================================================================
about_str_1	db	"Original idea and programming:", 0
about_str_2	db	"Victor Gamayunov", 0
about_str_3	db	"Copyright (c) 1993, 96, 97", 0
about_str_4	db	"Additional ideas and modifications:", 0
about_str_5	db	"Sergey Pimenov", 0
about_str_6	db	"Copyright (c) 1996, 97", 0
about_str_7	db	"Also thanks to:", 0
about_str_8	db	"Sergey Dyakov, Igor Tryndin, Andrey Ivanov", 0
about_str_9	db	"Igor Merkulov, Dmitriy Dudkin, Igor Chorny", 0
about_str_last:
;=============================================================================
info_str_1	db	"Internal version number:    V06.S06", 0
info_str_2	db	"Compilation date:        ", BUILD, 0
info_str_3	db	"Chip & Dale,  Rescue Rangers", 0
info_str_4	db	"Copyright (C) 1993, 96, 97", 0
;=============================================================================
question_labels:
		db	4, 3
		dw	question_str_1
		db	6, 4
		dw	question_str_2
		dw	0
;=============================================================================
name_label	db	"~N~ame", 0
filename_label	db	"File ~n~ame", 0
files_label	db	"~F~iles", 0
arg_name_label	db	"~C~ommand line", 0
addr_name_label	db	"~A~ddress", 0
search_for_label db	"Search ~f~or", 0
options_label	db	"~O~ptions", 0
from_label	db	"~F~rom", 0
to_label	db	"~T~o", 0
count_label	db	"~C~ount", 0
fill_label	db	"F~i~ll with", 0
delay_label	db	"Enter delay (18ths of sec)", 0
mem_blks_label	db	" Seg     Owner   Size    Type    Name", 0
;			  0123456789012345678901234567890123456789
;			  0         1         2         3
;=============================================================================
ignore_case_check db	"~I~gnore case", 0
backward_check	db	"~B~ackward", 0
exclude_hma_check db	"~E~xclude HMA", 0
result_code_check db	"~R~esult in code", 0
;=============================================================================
ok_button	db	"~O~k", 0
append_button	db	"~A~ppend", 0
rewrite_button	db	"~R~ewrite", 0
cancel_button	db	"~C~ancel", 0
go_button	db	"~G~o", 0
search_button	db	"~S~earch", 0
dump_button	db	"~D~ump", 0
;=============================================================================
question_str_1	db	"File with the same name already exist.", 0
question_str_2	db	"What do you want to do with file?", 0
;=============================================================================
about_title	db	7,	atDialogWindowTitle,	" About "
info_title	db	13,	atDialogWindowTitle,	" Information "
question_title	db	10,	atDialogWindowTitle,	" Question "
search_title	db	8,	atDialogWindowTitle,	" Search "
copy_title	db	12,	atDialogWindowTitle,	" Copy Block "
fill_title	db	12,	atDialogWindowTitle,	" Fill Block "
read_title	db	12,	atDialogWindowTitle,	" Read Block "
write_title	db	13,	atDialogWindowTitle,	" Write Block "
arg_title	db	11,	atDialogWindowTitle,	" Arguments "
enter_addr_title db	6,	atDialogWindowTitle,	" Goto "
load_title	db	13,	atDialogWindowTitle,	" Load a File "
animate_title	db	9,	atDialogWindowTitle,	" Animate "
mem_blks_title	db	15,	atDialogWindowTitle,	" Memory Blocks "
;=============================================================================
; E0F
;=============================================================================


