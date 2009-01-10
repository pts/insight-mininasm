/*****************************************************************************
* mkhelp.c -- creates help file for Insight 1.24.
* Copyright (c) 2007 - 2009, Oleg O. Chukaev
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
* 02111-1307, USA.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char istr[1000], ostr[1000];

int main (int argc, char *argv[]) {

	FILE *in, *out, *cf;
	int i, j, width;
	char c, color, col_frame = 0x03, col_key = 0x02, col_text = 0x07,
		col_bold = 0x0e, col_italic = 0x0f;

	if (argc < 5) {
		printf ("mkhelp: error: filename(s) not specified\n");
		printf ("usage: mkhelp <in-file> <out-file> <width-of-line> <colors-file>\n\n");
		return (1);
	}

	cf = fopen (argv[4], "rt");
	if (cf == NULL) {
		printf ("mkhelp: error: can't open colors file\n\n");
		return (2);
	}
	while (fgets (istr, 800, cf)) {
        	if (strstr (istr, "; HELP: ")) {
		        i = sscanf (istr, "; HELP: %d %d %d %d %d", &col_frame,
				&col_key, &col_text, &col_bold, &col_italic);
		        if (i != 5) {
				printf ("mkhelp: error: can't read colors\n\n");
				return (4);
		        }
                }
        }
        fclose (cf);

	in = fopen (argv[1], "rt");
	if (in == NULL) {
		printf ("mkhelp: error: can't open input file\n\n");
		return (2);
	}

	out = fopen (argv[2], "wb");
	if (out == NULL) {
		printf ("mkhelp: error: can't open output file\n\n");
		return (3);
	}

	width = atoi (argv[3]);
        if (width <= 0 || width > 150) {
		printf ("mkhelp: error: width of line too small or too large\n\n");
                return (4);
        }

	color = col_text;
	while (fgets (istr, 800, in)) {
                for (i = 0; i < 400; i+=2) {
                	ostr[i] = ' ';
			ostr[i+1] = col_text;
                }
		i = j = 0;
		while (istr[i] != '\n' && istr[i] != '\0') {
			c = istr[i++];
			switch (c) {
			case '(':
				color = col_bold; break;
			case '[':
				color = col_italic; break;
			case '<':
				color = col_frame; break;
			case '{':
				color = col_key; break;
			case ')':
			case ']':
			case '>':
			case '}':
				color = col_text; break;
			case '\\':
				if (istr[i+1] != '\0' && istr[i+1] != '\n') {
					c = istr[i++];
				}
				if (c == '0') {
					c = 0x1b;  /* left arrow */
				} else if (c == '1') {
					c = 0x1a;  /* right arrow */
				}
				/* FALLTHROUGH */
			default:
                        	ostr[j++] = c;
                                ostr[j++] = color;
			}
		}
                fwrite (ostr, width*2, 1, out);
	}

	fclose (in);
	fclose (out);

	return (0);
}


