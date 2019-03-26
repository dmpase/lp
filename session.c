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
#include <sys/file.h>
#include "lp.h"
#include "or.h"

#define F_OK    00              /* E_ACC does file exist */
#define X_OK    01              /* X_ACC is it executable by caller */
#define W_OK    02              /* W_ACC writable by caller */
#define R_OK    04              /* R_ACC readable by caller */


short av[CLIMIT];	/* all variables, artificial variables < 0 */
short avct;		/* number of artificial vars in basic list */
			/* it starts out positive, but if it ever */
			/* reaches zero, it stays there, even if the */
			/* artificial variable count goes back up */
short basic[CLIMIT];	/* basic variables */

save_session()
{
	FILE *fp;
	char ans[80],file[80];

	ask("File (save): ",file);
	if (file[0] == '\0')
		return;
	if (access(file,F_OK) == 0) {
		if (access(file,W_OK) == -1) {
			sprintf(ans,"Unable to overwrite %s",file);
			errorw(ans);
			return;
		}
		else {
			ask("Overwrite this file? ",ans);
			if (ans[0] != 'y' && ans[0] != 'Y') {
				sprintf(ans,"%s not overwritten",file);
				errorw(ans);
				return;
			}
		}
	}

	fp = fopen(file,"w");
	if (fp == (FILE *) NULL) {
		sprintf(ans,"Unable to open %s",file);
		errorw(ans);
		return;
	}

	write_system(fp);

	if (cur_soln == modified) {
		cur_soln = 0;
	}
	else {
		cur_soln = 10000000;
	}
	modified = 0;
	fclose(fp);
}

restore_session()
{
	int i;
	FILE *fp;
	char ans[80],file[80];


	if (modified) {
		ask("This session was modified - save? (n)[yn] ",ans);
		if (ans[0] == 'y' || ans[0] == 'Y') {
			save_session();
		}
	}

	ask("File (restore): ",file);
	if (file[0] == '\0')
		return;
	if (access(file,F_OK) == -1) {
		sprintf(ans,"%s does not exist",file);
		errorw(ans);
		return;
	}

	fp = fopen(file,"r");
	if (fp == (FILE *) NULL) {
		sprintf(ans,"Unable to open %s",file);
		errorw(ans);
		return;
	}

	read_system(fp);

	rscr = 1;
	cscr = 1;
	move_area('a');
	move_position(1,1);
	for (i=rmax+1; i < rscr+rsmx; i++) {
		blank_row(i);
	}
	for (i=cmax+1; i < cscr+csmx; i++) {
		blank_col(i);
	}

	modified = 0;
	cur_soln = -1;
	print_screen();
	fclose(fp);

	no_soln();
}

write_system(fp)
FILE *fp;
{
	int i,j,cksum;
	char conf[8];

	cksum = write_field(fp, "confirm", sizeof(conf));

	cksum += write_field(fp, (char *) &rmax, sizeof(rmax));
	cksum += write_field(fp, (char *) &cmax, sizeof(cmax));

	for (i=0; i < rmax; i++) {
		cksum += write_field(fp, d[i], sizeof(d[i]));
		for (j=0; j < cmax; j++) {
			cksum += write_field(fp, (char *) &a[i][j],
				sizeof(a[i][j]));
		}
		cksum += write_field(fp, (char *) &e[i], sizeof(e[i]));
		cksum += write_field(fp, (char *) &b[i], sizeof(b[i]));
	}

	for (j=0; j < cmax; j++) {
		cksum += write_field(fp, x[j], sizeof(x[j]));
		cksum += write_field(fp, (char *) &c[j], sizeof(c[j]));
	}

	cksum += write_field(fp, z, sizeof(z));
	cksum += write_field(fp, (char *) &cksum, sizeof(cksum));
}

read_system(fp)
FILE *fp;
{
	int i,j,cksum,sum;
	char conf[8];

	cksum = read_field(fp, (char *) conf, sizeof(conf));
	if (strcmp(conf,"confirm") != 0) {
		errorw("Read failed - not a session");
		return;
	}

	i = rmax; j = cmax;
	cksum += read_field(fp, (char *) &rmax, sizeof(rmax));
	cksum += read_field(fp, (char *) &cmax, sizeof(cmax));
	if (rmax < 1 || rmax > RLIMIT || cmax < 1 || cmax > CLIMIT) {
		rmax = i;
		cmax = j;
		errorw("Read failed - session corrupted (?)");
		return;
	}

	for (i=0; i < rmax; i++) {
		cksum += read_field(fp, d[i], sizeof(d[i]));
		for (j=0; j < cmax; j++) {
			cksum += read_field(fp, (char *) &a[i][j],
				sizeof(a[i][j]));
		}
		cksum += read_field(fp, (char *) &e[i], sizeof(e[i]));
		cksum += read_field(fp, (char *) &b[i], sizeof(b[i]));
	}

	for (j=0; j < cmax; j++) {
		cksum += read_field(fp, x[j], sizeof(x[j]));
		cksum += read_field(fp, (char *) &c[j], sizeof(c[j]));
	}

	cksum += read_field(fp, z, sizeof(z));
	read_field(fp, (char *) &sum, sizeof(sum));
	if (sum != cksum) {
		errorw("Read failed - session corrupted");
		init_session();
	}
}

write_field(fp,f,s)
FILE *fp;
char *f;
int s;
{
	int sum;

	sum = 0;
	while (0 < s) {
		putc(*f,fp);
		sum += *f;
		f++;
		s--;
	}
	return(sum);
}

read_field(fp,f,s)
FILE *fp;
char *f;
int s;
{
	int sum;

	sum = 0;
	while (0 < s) {
		*f = getc(fp);
		sum += *f;
		f++;
		s--;
	}
	return(sum);
}

print_session()
{
	errorw("print_session is not yet implemented");
}

maximize_system()
{
	int i;
	struct m2d *A, *B, *C;

	errorw("Maximizing...");
	refresh();
	setup_system(&A,&B,&C,-1);
	soln_type = simplex_m2d(A,B,C,&Z);
	clear_errw();

	move(CLINE,BCOL);
	printw("Maximized");

	cur_soln = modified;
	soln();

	if (soln_type == SOLUTION || soln_type == UNBOUNDED) {
		for (i=1; i <= rmax; i++) {
			if (basic[i-1] < 0 && 0.0 < get_m2d(B,i,1)) {
				if (avct)
					soln_type = NO_SOLN;
				else
					soln_type = UNBOUNDED;
				break;
			}
		}
	}

	kill_system(A,B,C,soln_type);

	print_solution(soln_type);
}

minimize_system()
{
	int i;
	struct m2d *A, *B, *C;

	errorw("Minimizing...");
	refresh();
	setup_system(&A,&B,&C,1);
	soln_type = simplex_m2d(A,B,C,&Z);
	clear_errw();

	move(CLINE,BCOL);
	printw("minimized");

	Z = -Z;

	cur_soln = modified;
	soln();

	if (soln_type == SOLUTION || soln_type == UNBOUNDED) {
		for (i=1; i <= rmax; i++) {
			if (basic[i-1] < 0 && 0.0 < get_m2d(B,i,1)) {
				if (avct)
					soln_type = NO_SOLN;
				else
					soln_type = UNBOUNDED;
				break;
			}
		}
	}

	kill_system(A,B,C,soln_type);

	print_solution(soln_type);
}

print_solution(k)
int k;
{
	int j;

	move(SLINE, BCOL);

	switch (k) {
	case NO_SOLN :
		printw("%9s","None");
		return;
	case SOLUTION :
		print9gw(Z);
		break;
	case UNBOUNDED :
		printw("%9s","Unbounded");
		return;
	}

	for (j=0; j < min(cmax,csmx); j++) {
		print_field(rscr,cscr+j,0,'s');
	}
}

setup_system(A,B,C,m)
struct m2d **A, **B, **C;
int m;
{
	int i,j,k,extras,bigM;
	matelt M,oldC,newC;

	extras = rmax;
	bigM = 0;
	for (i=1; i <= rmax; i++) {
		switch (e[i-1]) {
		case '<' :
			break;
		case '=' :
			bigM = 1;
			break;
		case '>' :
			extras += 1;
			bigM = 1;
			break;
		}
	}

	if (bigM) {
		matelt MaxA,MaxB,MaxC;

		MaxA = MaxB = MaxC = 1.0;
		for (i=1; i <= rmax; i++) {
			for (j=1; j <= cmax; j++)
				if (a[i-1][j-1] > MaxA)
					MaxA = a[i-1][j-1];

			if (b[i-1] > MaxB)
				MaxB = b[i-1];
		}
		for (j=1; j <= cmax; j++) {
			if (c[j-1] > MaxC)
				MaxC = c[j-1];
		}
		M = max(MaxA,max(MaxB,MaxC)) * 128.0;
	}

	*A = new_m2d(rmax,cmax+extras);
	*B = new_m2d(rmax,1);
	*C = new_m2d(1,cmax+extras);
	 Z = 0.0;

	for (j=1; j <= cmax; j++) {
		set_m2d(*C,1,j,m*c[j-1]);
		s[j-1] = 0.0;
		av[j-1] = j;		/* normal variable */
	}
	for (j=cmax+1; j <= cmax+extras; j++) {
		set_m2d(*C,1,j,0.0);
	}

	avct = 0;
	k = cmax+1;
	for (i=1; i <= rmax; i++) {
		for (j=1; j <= cmax; j++) {
			set_m2d(*A,i,j,a[i-1][j-1]);
		}
		for (j=cmax+1; j <= cmax+extras; j++) {
			set_m2d(*A,i,j,0.0);
		}

		switch (e[i-1]) {
		case '<' :
			set_m2d(*A,i,k,1.0);
			basic[i-1] = k;
			av[k-1] = k;		/* slack variable */
			k++;
			break;
		case '>' :
			set_m2d(*C,1,k,M);
			set_m2d(*A,i,k,-1.0);
			av[k-1] = k;		/* surplus variable */
			k++;
		case '=' :
			Z -= b[i-1] * M;
			for (j=1; j <= cmax; j++) {
				oldC = get_m2d(*C,1,j);
				newC = oldC - get_m2d(*A,i,j)*M;
				set_m2d(*C,1,j,newC);
			}
			set_m2d(*A,i,k,1.0);
			basic[i-1] = -k;
			av[k-1] = -k;		/* artificial variable */
			avct++;
			k++;
			break;
		}

		set_m2d(*B,i,1,b[i-1]);
	}
}

kill_system(A,B,C,k)
struct m2d *A, *B, *C;
int k;
{
	int i,j;

	for (i=1; i <= rmax; i++) {
		j = basic[i-1];
		if (1 <= j && j <= cmax) {
			if (k == NO_SOLN)
				s[j-1] = 0.0;
			else
				s[j-1] = get_m2d(B,i,1);
		}
	}

	kill_m2d(A);
	kill_m2d(B);
	kill_m2d(C);
}

no_soln()
{
	move(SLINE,0);
	clrtoeol();
	move(SLINE,68);
	printw("|");
}

soln()
{
	no_soln();
	move(SLINE,2);
	printw("    Solution:");
}

invalid_soln()
{
	if (cur_soln > -1) {
		move(SLINE,2);
		printw("Invalid soln:");
	}
}
