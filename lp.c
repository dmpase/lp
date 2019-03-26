/*******************************************************************************
 * Copyright (c) 1988, Douglas M. Pase                                         *
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

char area='a';	/* ax <= b, Z = cx, non-negativity constraints {axbcn} */
int rscr = 1;	/* row position of screen */
int cscr = 1;	/* column position of screen */
int rcur = 1;	/* row position of cursor */
int ccur = 1;	/* column position of cursor */
int rmax = 1;	/* maximum rows in the system */
int cmax = 1;	/* maximum columns in the system */
int rsmx = 8;	/* maximum rows on the screen */
int csmx = 5;	/* maximum columns on the screen */

int cur_soln = -1;
int soln_type = 0;
int modified = 0;

double ator();
double a[RLIMIT][CLIMIT];
double b[RLIMIT];
double c[CLIMIT];
double s[CLIMIT];

matelt Z;

char d[RLIMIT][14];
char e[RLIMIT];
char x[CLIMIT][10];
char z[14] = "Z";

main(argc,argv)
int argc;
char *argv[];
{
	init_sys(argc,argv);
	get_com();
	end_sys();
}

print_field(row,col,h,area)
int row,col,h,area;
{
	if (row < 1 || row > rscr+rsmx-1 || row > rmax)
		return;

	if (col < 1 || col > cscr+csmx-1 || col > cmax)
		return;

	switch(area) {
	case 'a':
		move(ALINE+2*(row - rscr), ACOL+10*(col - cscr));
		if (h) standout();
		print9gw(a[row-1][col-1]);
		if (h) standend();
		break;
	case 'b':
		move(ALINE+2*(row - rscr), BCOL);
		if (h) standout();
		print9gw(b[row-1]);
		if (h) standend();
		break;
	case 'c':
		move(CLINE, ACOL+10*(col - cscr));
		if (h) standout();
		print9gw(c[col-1]);
		if (h) standend();
		break;
	case 'd':
		move(ALINE+2*(row - rscr), DCOL);
		if (h) standout();
		printw("%13s",d[row-1]);
		if (h) standend();
		break;
	case 'e':
		move(ALINE+2*(row - rscr), ECOL);
		if (h) standout();
		printw("%c=",e[row-1]);
		if (h) standend();
		break;
	case 'n':
		move(NLINE, ACOL+10*(col - cscr));
		if (h) standout();
		printw("         ");
		if (h) standend();
		break;
	case 's':
		if (soln_type == SOLUTION) {
			move(SLINE, ACOL+10*(col - cscr));
			print9gw(s[col-1]);
		}
		break;
	case 'x':
		move(XLINE, ACOL+10*(col - cscr));
		if (h) standout();
		printw("%9s",x[col-1]);
		if (h) standend();
		break;
	case 'z':
		move(CLINE, DCOL);
		if (h) standout();
		printw("%13s",z);
		if (h) standend();
		break;
	}
}

enter_field()
{
	char ans[80];

	ask("New field value: ",ans);
	if (ans[0] == '\0')
		return;

	switch(area) {
	case 'z':
		ans[13] = '\0';
		strcpy(z,ans);
		break;
	case 'a':
		a[rcur-1][ccur-1] = ator(ans);
		modified += 1;
		invalid_soln();
		break;
	case 'b':
		b[rcur-1] = ator(ans);
		modified += 1;
		invalid_soln();
		break;
	case 'c':
		c[ccur-1] = ator(ans);
		modified += 1;
		invalid_soln();
		break;
	case 'd':
		ans[13] = '\0';
		strcpy(d[rcur-1],ans);
		break;
	case 'e':
		switch (ans[0]) {
		case '<' : case '=' : case '>' :
			e[rcur-1] = ans[0];
		}
		modified += 1;
		invalid_soln();
		break;
	case 'n':
		break;
	case 's':
		break;
	case 'x':
		ans[9] = '\0';
		strcpy(x[ccur-1],ans);
		break;
	}
}

delete_row(row)
int row;
{
	char ans[80];

	print_row(row,1);
	ask("Delete this row? (n)[yn] ",ans);
	if (ans[0] == 'y' || ans[0] == 'Y') {
		del_row(row);
	}
	else {
		errorw("Row not deleted");
	}
	if (rmax < row)
		print_row(rmax,0);
	else
		print_row(row,0);
}

del_row(row)
int row;
{
	int i,j;

	move_area(area);
	if (row < 1 || row > rmax || rmax <= 1) {
		errorw("Unable to delete row");
		return;
	}

	for (i=row; i < rmax; i++) {
		strcpy(d[i-1],d[i]);
		for (j=0; j < cmax; j++) {
			a[i-1][j] = a[i][j];
		}
		e[i-1] = e[i];
		b[i-1] = b[i];
	}
	rmax--;
	rcur = min(rmax,rcur);
	rscr = min(rmax,rscr);

	for (i=row; i < min(rmax+1,rscr+rsmx); i++) {
		print_row(i,0);
	}
	if (rmax < rscr+rsmx-1) {
		blank_row(rmax+1);
	}

	modified += 1;
	invalid_soln();
}

delete_col(col)
int col;
{
	char ans[80];

	print_col(col,1);
	ask("Delete this column? (n)[yn] ",ans);
	if (ans[0] == 'y' || ans[0] == 'Y') {
		del_col(col);
	}
	else {
		errorw("Column not deleted");
	}
	if (cmax < col)
		print_col(cmax,0);
	else
		print_col(col,0);
}

del_col(col)
int col;
{
	int i,j;

	if (col < 1 || col > cmax || cmax <= 1) {
		errorw("Unable to delete column");
		return;
	}

	for(i=col; i < cmax; i++) {
		strcpy(x[i-1], x[i]);
		for (j=0; j < rmax; j++) {
			a[j][i-1] = a[j][i];
		}
		c[i-1] = c[i];
		s[i-1] = s[i];
	}
	cmax--;
	ccur = min(cmax,ccur);
	cscr = min(cmax,cscr);

	for (i=col; i < min(cmax+1,cscr+csmx); i++) {
		print_col(i,0);
	}
	if (cmax < cscr+csmx-1) {
		blank_col(cmax+1);
	}

	modified += 1;
	invalid_soln();
}

add_row(row)
int row;
{
	int i,j;

	if (row < 1 || row > rmax+1 || rmax >= RLIMIT) {
		return;
	}

	rmax++;
	for (i=rmax-1; i >= row; i--) {
		strcpy(d[i],d[i-1]);
		for (j=0; j < cmax; j++) {
			a[i][j] = a[i-1][j];
		}
		e[i] = e[i-1];
		b[i] = b[i-1];
	}
	d[row-1][0] = '\0';
	for (j=0; j < cmax; j++) {
		a[row-1][j] = 0.0;
	}
	e[row-1] = '<';
	b[row-1] = 0.0;
	for (i=row; i < min(rmax+1,rscr+rsmx); i++) {
		print_row(i,0);
	}

	modified += 1;
	invalid_soln();
}

add_col(col)
int col;
{
	int i,j;

	if (col < 1 || col > cmax+1)
		return;

	cmax++;
	for (i=cmax-1; i >= col; i--) {
		strcpy(x[i],x[i-1]);
		for (j=0; j < rmax; j++) {
			a[j][i] = a[j][i-1];
		}
		c[i] = c[i-1];
		s[i] = s[i-1];
	}
	x[col-1][0] = '\0';
	for (j=0; j < rmax; j++) {
		a[j][col-1] = 0.0;
	}
	c[col-1] = 0.0;
	s[col-1] = 0.0;
	for (i=col; i < min(cmax+1,cscr+csmx); i++) {
		print_col(i,0);
	}

	modified += 1;
	invalid_soln();
}

copy_rc()
{
	int from, to;
	char ans[80];

	ask("Copy a row or column (rc)? ",ans);
	if (ans[0] == 'r' || ans[0] == 'R') {
		from = get_row("From (row): ");
		if (!from) {
			errorw("Row not copied");
			return;
		}
		to = get_row("To (row): ");
		if (!to) {
			errorw("Row not copied");
			return;
		}
		copy_row(from,to);
	}
	else if (ans[0] == 'c' || ans[0] == 'C') {
		from = get_col("From (column): ");
		if (!from) {
			errorw("Column not copied");
			return;
		}
		to = get_col("To (column): ");
		if (!to) {
			errorw("Column not copied");
			return;
		}
		copy_col(from,to);
	}
	else {
		errorw("Nothing copied");
	}
}

copy_row(from,to)
int from, to;
{
	int j;

	if (from < 1 || from > rmax || to < 1 || to > rmax) {
		errorw("copy_row: parameter error");
		return;
	}

	strcpy(d[to-1], d[from-1]);
	for (j=0; j < cmax; j++) {
		a[to-1][j] = a[from-1][j];
	}
	e[to-1] = e[from-1];
	b[to-1] = b[from-1];

	if (rscr <= to && to < rscr+rsmx) {
		print_row(to,0);
	}

	modified += 1;
	invalid_soln();
}

copy_col(from,to)
int from, to;
{
	int j;

	if (from < 1 || from > cmax || to < 1 || to > cmax) {
		errorw("copy_col: parameter error");
		return;
	}

	strcpy(x[to-1], x[from-1]);
	for (j=0; j < rmax; j++) {
		a[j][to-1] = a[j][from-1];
	}
	c[to-1] = c[from-1];
	s[to-1] = s[from-1];

	if (cscr <= to && to < cscr+csmx) {
		print_col(to,0);
	}

	modified += 1;
	invalid_soln();
}

print9gw(f)
double f;
{
	char str[32];

	sprintf(str,"%g",f);
	if (strlen(str) > 9) {
		if (f < 0.0) {
			sprintf(str,"%.3g",f);
		}
		else {
			sprintf(str,"%.4g",f);
		}
	}

	printw("%9s",str);
}
