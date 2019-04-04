/*******************************************************************************
 * Copyright (c) 1988,2019 Douglas M. Pase                                     *
 * All rights reserved.                                                        *
 * Redistribution and use in source and binary forms, with or without          *
 * modification, are permitted provided that the following conditions          *
 * are met:                                                                    *
 * o       Redistributions of source code must retain the above copyright      *
 *         notice, this list of conditions and the following disclaimer.       *
 * o       Redistributions in binary form must reproduce the above copyright   *
 *         notice, this list of conditions and the following disclaimer in     *
 *         the documentation and/or other materials provided with the          *
 *         distribution.                                                       *
 * o       Neither the name of the copyright holder nor the names of its       *
 *         contributors may be used to endorse or promote products derived     *
 *         from this software without specific prior written permission.       *
 *                                                                             *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" *
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  *
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   *
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         *
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        *
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    *
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     *
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     *
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF      *
 * THE POSSIBILITY OF SUCH DAMAGE.                                             *
 *******************************************************************************/

#include <stdio.h>
#include <curses.h>
#include "lp.h"
#include "or.h"

void move_area(int new)
{
    print_field(rcur, ccur, 0, area);	/* wipe out old highlight */
    area = new;
    print_field(rcur, ccur, 1, area);	/* highlight new area */
}

void move_position(int r, int c)
{
    if (r<1 || r>rmax || c<1 || c>cmax) {
	errorw("End of screen");
	return;
    }

    print_field(rcur, ccur, 0, area);	/* wipe out old highlight */

    move_screen(r,c);

    print_field(r, c, 1, area);	/* display new field */
    rcur = r;
    ccur = c;
}

static int err_set = 1;

void errorw(char *s)
{
    err_set = strlen(s);
    move(LINES-1,1);
    clrtoeol();
    standout();
    printw(s);
    standend();
}

void clear_errw()
{
    if (err_set) {
	err_set = 0;
	move(LINES-1,1);
	clrtoeol();
    }
}

void move_screen(int r, int c)
{
    int i,j;

    i = rscr;
    j = cscr;		/* check if new position is on screen */
    if (r < rscr) {			/* off the top */
	if (r > rscr - rsmx) {		/* short jump */
	    i = r;
	} else {				/* long jump */
	    i = r - rsmx/2;
	    if (i<1) i = 1;
	    if (i>rmax-rsmx) i = rmax-rsmx+1;
	}
    }
    else if (r > rscr + rsmx - 1) {	/* off the bottom */
	if (r < rscr + 2*rsmx - 1) {	/* short jump */
	    i = r - rsmx + 1;
	} else {				/* long jump */
	    i = r - rsmx/2;
	    if (i<1) i = 1;
	    if (i>rmax-rsmx) i = rmax-rsmx+1;
	}
    }
    if (c < cscr) {			/* off the left */
	if (c > cscr - csmx) {		/* short jump */
	    j = c;
	} else {				/* long jump */
	    j = c - csmx/2;
	    if (j<1) j = 1;
	    if (j>cmax-csmx) j = cmax-csmx+1;
	}
    }
    else if (c > cscr + csmx - 1) {	/* off the right */
	if (c < cscr + 2*csmx - 1) {	/* short jump */
	    j = c - csmx + 1;
	} else {				/* long jump */
	    j = c - csmx/2;
	    if (j<1) j = 1;
	    if (j>cmax-csmx) j = cmax-csmx+1;
	}
    }
    if (i == rscr && j == cscr) {	/* new position is on screen */
	return;
    }

    rscr = i;
    cscr = j;
    print_screen();
}

void print_screen()
{
    int i,j;

    print_field(1,1,0,'z');
    if (cur_soln > -1) {
	if (cur_soln == modified) {
	    move(SLINE,2);
	    printw("    Solution:");
	} else {
	    move(SLINE,2);
	    printw("Invalid soln:");
	}
	print_field(1,1,0,'Z');
    } else {
	move(SLINE,2);
	printw("             ");
	move(SLINE, BCOL);
	printw("         ");
	move(CLINE, BCOL);
	printw("         ");
    }

    for (j=0; j < min(cmax,csmx); j++) {
	print_field(rscr,cscr+j,0,'x');
	print_field(rscr,cscr+j,0,'c');
	if (cur_soln > -1 && soln_type == SOLUTION) {
	    print_field(rscr,cscr+j,0,'s');
	}
    }

    for (i=0; i < min(rmax,rsmx); i++) {
	print_field(rscr+i,cscr,0,'d');
	for (j=0; j < min(cmax,csmx); j++) {
	    print_field(rscr+i,cscr+j,0,'a');
	}
	print_field(rscr+i,cscr,0,'e');
	print_field(rscr+i,cscr,0,'b');
    }
}

void go_to()
{
    int row, col;

    if ((row=get_row("Row: ")) && (col=get_col("Column: "))) {
	move_position(row,col);
    }
}

int get_row(char *s)
{
    char ans[80];
    int i,row;

    ask(s,ans);
    row = atoi(ans);
    if (ans[0] != '\0') {
	if (ans[0] == '$') {
	    row = rmax;
	}
	for (i=0; i<rmax; i++) {
	    if (strcmp(ans,d[i]) == 0) {
		row = i + 1;
		break;
	    }
	}
	if (row < 1 || row > rmax) {
	    errorw("Row is out of bounds...");
	    return(0);
	}
    } else {
	row = rcur;
    }

    return(row);
}

void int get_col(char *s)
{
    char ans[80];
    int i,col;

    ask(s,ans);
    col = atoi(ans);
    if (ans[0] != '\0') {
	if (ans[0] == '$') {
	    col = cmax;
	}
	for (i=0; i<cmax; i++) {
	    if (strcmp(ans,x[i]) == 0) {
		col = i + 1;
		break;
	    }
	}
	if (col < 1 || col > cmax) {
	    errorw("Column is out of bounds...");
	    return(0);
	}
    } else {
	col = ccur;
    }

    return(col);
}

void ask(char *p, char *s)
{
    int c, k, pl;

    move(LINES-1, 0);
    pl = strlen(p);
    printw(p);
    clrtoeol();
    refresh();
    k = 0;
    do {
	c = getch();
	if (c == '\b' || c == '\177' ) {
	    k = k-1;
	    if (k < 0) {
		k = 0;
	    } else {
		move(LINES-1,k+pl);
		printw(" ");
		move(LINES-1,k+pl);
	    }
	    c = 'a';
	} else if (c >= ' ') {
	    s[k] = c;
	    move(LINES-1,k+pl);
	    printw("%c",c);
	    k += 1;
	} else if (c != '\r' && c != '\n') {
	    c = ' ';
	}
	refresh();
    } while (k<80 && c>=' ');
    s[k] = '\0';
    move(LINES-1,0);
    clrtoeol();
    refresh();
}

double ator(char *a)
{
    double s;	/* the sign */
    double r;	/* the result */
    double e;	/* the exponent */

    if (*a == '-') {	/* get the sign */
	s = -1.0;
	a++;
    } else {
	s = 1.0;
	if (*a == '+') {
	    a++;
	}
    }
    /*
     * Get the number up to the decimal point.
     */
    r = 0.0;
    for ( ; '0' <= *a && *a <= '9'; a++) {	/* get dd...d. */
	r = r * 10.0 + *a - '0';
    }
    if (*a != '.') {				/* only dd...d to get */
	return((float) s * r);
    }
    /*
     * We now have the number up to the decimal.
     * Get the portion to the right of the decimal.
     */
    for(a++, e = 0.1; '0' <= *a && *a <= '9'; a++, e *= 0.1) {
	r += e * (*a - '0');
    }
    r *= s;
    /*
     * Check for an exponent.
     */
    if (*a != 'D' && *a != 'd' && *a != 'E' && *a != 'e') {
	return((float) r);
    }
    a++;
    /*
     * Get the sign of the exponent.
     */
    if (*a == '-') {
	s = -1.0;
	a++;
    } else {
	s = 1.0;
	if (*a == '+') {
	    a++;
	}
    }
    /*
     * Get the magnitude of the exponent.
     */
    e = 0.0;
    for ( ; '0' <= *a && *a <= '9'; a++) {	/* have ddd.ddE- now get ee */
	e = e * 10.0 + *a - '0';
    }
    /*
     * multiply result by 10^exponent
     */
    for ( ; 0.5 < e; e -= 1.0) {
	if (s < 0.0) {
	    r *= 0.1;
	} else {
	    r *= 10.0;
	}
    }
    return((float) r);
}

void print_row(int row, int h)
{
    int j;

    if (row < 1 || row > rmax || row < rscr || row > rscr+rsmx-1) {
	return;
    }

    print_field(row,cscr,h,'d');
    for (j=0; j < min(cmax,csmx); j++) {
	print_field(row,cscr+j,h,'a');
    }
    print_field(row,cscr,h,'e');
    print_field(row,cscr,h,'b');
}

void print_col(int col, int h)
{
    int j;

    if (col < 1 || col > cmax || col < cscr || col > cscr+csmx-1) {
	return;
    }

    print_field(rscr,col,h,'x');
    for (j=0; j < min(rmax,rsmx); j++) {
	print_field(rscr+j,col,h,'a');
    }
    print_field(rscr,col,h,'c');
    if (cur_soln > -1) {
	print_field(rscr,col,h,'s');
    }
}

void blank_row(int row)
{
    int j;

    if (row < 1 || row < rscr || row > rscr+rsmx-1) {
	return;
    }

    move(ALINE+2*(row - rscr), DCOL);
    printw("             ");
    for (j=0; j < csmx; j++) {
	move(ALINE+2*(row - rscr), ACOL+10*(j));
	printw("         ");
    }
    move(ALINE+2*(row - rscr), ECOL);
    printw("  ");
    move(ALINE+2*(row - rscr), BCOL);
    printw("         ");
}

void blank_col(int col)
{
    int j;

    if (col < 1 || col < cscr || col > cscr+csmx-1) {
	return;
    }

    move(XLINE, ACOL+10*(col - cscr));
    printw("         ");
    for (j=0; j < rsmx; j++) {
	move(ALINE+2*(j), ACOL+10*(col - cscr));
	printw("         ");
    }
    move(CLINE, ACOL+10*(col - cscr));
    printw("         ");
    move(SLINE, ACOL+10*(col - cscr));
    printw("         ");
}
