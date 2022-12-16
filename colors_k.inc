;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; data\colors_k.inc
; Colors definition.
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

MAIN_BG		equ BLACK
MAIN_FG		equ LIGHT_GRAY
BORDER_FG	equ WHITE
TITLE_FG	equ YELLOW
CPU_NAME_FG	equ CYAN

CODE_MARK_FG	equ LIGHT_RED
UNASM_ADDR_FG	equ LIGHT_CYAN
DATA_WATCH_FG	equ LIGHT_RED
ADDR_FG		equ LIGHT_GRAY
NAME_FG		equ LIGHT_GRAY
VALUE_FG	equ LIGHT_GREEN
NEW_VALUE_FG	equ LIGHT_CYAN

OP_FG		equ GREEN
INS_FG		equ LIGHT_GREEN
NUM_FG		equ CYAN
ARR_FG		equ WHITE
DIS_FG		equ LIGHT_GRAY

CPU_S_BG	equ LIGHT_GRAY
ADR_S_FG	equ BLACK
OP_S_FG		equ BLACK
INS_S_FG	equ BLACK
NUM_S_FG	equ BLACK
ARR_S_FG	equ LIGHT_CYAN
DIS_S_FG	equ DARK_GRAY

CPU_B_BG	equ RED
ADR_B_FG	equ WHITE
OP_B_FG		equ WHITE
INS_B_FG	equ WHITE
NUM_B_FG	equ WHITE
ARR_B_FG	equ WHITE
DIS_B_FG	equ DARK_GRAY

ADR_BD_FG	equ DARK_GRAY
OP_BD_FG	equ DARK_GRAY
INS_BD_FG	equ DARK_GRAY
NUM_BD_FG	equ DARK_GRAY
ARR_BD_FG	equ WHITE
DIS_BD_FG	equ DARK_GRAY

ADR_BDS_FG	equ DARK_GRAY
OP_BDS_FG	equ DARK_GRAY
INS_BDS_FG	equ DARK_GRAY
NUM_BDS_FG	equ DARK_GRAY
ARR_BDS_FG	equ WHITE
DIS_BDS_FG	equ DARK_GRAY

CPU_BS_BG	equ MAGENTA
ADR_BS_FG	equ WHITE
OP_BS_FG	equ WHITE
INS_BS_FG	equ WHITE
NUM_BS_FG	equ WHITE
ARR_BS_FG	equ WHITE
DIS_BS_FG	equ DARK_GRAY

ADR_NF_FG	equ YELLOW
OP_NF_FG	equ YELLOW
INS_NF_FG	equ YELLOW
NUM_NF_FG	equ YELLOW
ARR_NF_FG	equ WHITE
DIS_NF_FG	equ LIGHT_GRAY

DUMP_CUR_BG	equ BLUE
DUMP_CUR_FG	equ WHITE
DUMP_ZERO_FG	equ GREEN

MEUN_BG		equ LIGHT_GRAY
MENU_S_BG	equ GREEN
MENU_BORDER_FG	equ BLACK
MENU_TXT_FG	equ BLACK
MENU_HOT_FG	equ RED
MENU_DIS_FG	equ DARK_GRAY
MENU_S_TXT_FG	equ BLACK
MENU_S_HOT_FG	equ RED
MENU_S_DIS_FG	equ DARK_GRAY

MSG_BOX_BG	equ BLUE
MSG_BOX_B_FG	equ WHITE
MSG_BOX_T_FG	equ LIGHT_CYAN
MSG_BOX_M_FG	equ YELLOW

ERR_BOX_BG	equ RED
ERR_BOX_B_FG	equ WHITE
ERR_BOX_T_FG	equ YELLOW
ERR_BOX_M_FG	equ YELLOW

DIALOG_BG	equ LIGHT_GRAY
DIALOG_B_FG	equ WHITE
DIALOG_T_FG	equ WHITE

WINDOW_SH_BG	equ BLACK
WINDOW_SH_FG	equ LIGHT_GRAY

INPUT_BG	equ BLUE
INPUT_FG	equ WHITE
INPUT_S_BG	equ GREEN
INPUT_S_FG	equ WHITE

BUTTON_BG	equ GREEN
BUTTON_FG	equ BLACK
BUTTON_L_FG	equ YELLOW
BUTTON_S_FG	equ WHITE
BUTTON_SL_FG	equ YELLOW
BUTTON_DEF_FG	equ BLUE
BUTTON_LDEF_FG	equ YELLOW
BUTTON_SH_FG	equ BLACK

LABEL_FG	equ BLACK
LABEL_L_FG	equ YELLOW
LABEL_S_FG	equ WHITE
LABEL_SL_FG	equ YELLOW

LIST_BG		equ CYAN
LIST_S_BG	equ GREEN
LIST_FG		equ BLACK
LIST_S_FG	equ WHITE
LIST_D_FG	equ YELLOW

CB_RB_BG	equ CYAN
CB_RB_T_FG	equ BLACK
CB_RB_S_FG	equ WHITE
CB_RB_H_FG	equ YELLOW

ASM_FG		equ WHITE
ASM_B_FG	equ LIGHT_CYAN

MEM_ADDR_FG	equ WHITE
MEM_VAL_FG	equ YELLOW

; HELP: 3 2 7 14 15

;=============================================================================
; E0F
;=============================================================================
