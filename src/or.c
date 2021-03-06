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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <curses.h>
#include "or.h"

int lp_update();

extern short av[];	/* list of variables, artificial variables are < 0 */
extern short avct;	/* number of artificial variables in basic list */
			/* if this value drops to zero, it stays there even */
			/* if the count goes back up */
extern short basic[];	/* vector of basic variables */

/*
 * simplex_m2d: solves linear programing problems via the simplex method.
 * Maximize Z = cx under the constraints ax = b.
 */

int simplex_m2d(struct m2d* a, struct m2d* b, struct m2d* c, matelt* Z)
{
    int i,pc,pr;

    if (a == m2d_NULL || b == m2d_NULL || c == m2d_NULL)
	return(NO_SOLN);

    int m = a->i;
    int n = a->j - m;
    if (b->i != m || b->j != 1 || c->i != 1 || c->j != a->j || n < 1)
	return(NO_SOLN);

    /* while there exists another pivot column */
	/* select pivot column */
	/* select pivot row */
	/* update basic, a and c */

    i = SOLUTION;
    while ((pc = pivot_col(c)) != 0) {
	pr = pivot_row(a,b,pc);

#if defined(DEBUG)
	{ 
	    int j; char ans[80];
	    pr_m2d(stdout,a);
	    pr_m2d(stdout,b);
	    pr_m2d(stdout,c);
	    printf("Z = %g, i = %d\r\n",*Z,i);
	    printf("av = ");
	    for (j=0; j < a->j; j++) {
		printf("%d ",av[j]);
	    }
	    printf("\r\n");
	    printf("basic = ");
	    for (j=0; j < a->i; j++) {
		printf("%d ",basic[j]);
	    }
	    printf("\r\n");
	    ask("Ready? ",ans);
	    if (ans[0] == 'Q') {
		endwin();exit();
	    }
	}
#endif

	if (pr) {
	    if ((i=lp_update(a,b,c,pr,pc,Z)) != SOLUTION) {
		break;
	    }
	} else {
	    /* Z is unbounded */
	    i = UNBOUNDED;
	    break;
	}
    }

#if defined(DEBUG)
    { 
	int j; char ans[80];
	pr_m2d(stdout,a);
	pr_m2d(stdout,b);
	pr_m2d(stdout,c);
	printf("Z = %g, i = %d\r\n",*Z,i);
	printf("av = ");
	for (j=0; j < a->j; j++) {
	    printf("%d ",av[j]);
	}
	printf("\r\n");
	printf("basic = ");
	for (j=0; j < a->i; j++) {
	    printf("%d ",basic[j]);
	}
	printf("\r\n");
	ask("Ready? ",ans);
	clearok(stdscr,TRUE);
    }
#endif


    return(i);
}

int pivot_col(struct m2d *c)
{
    int i,j;

    j = 0;				/* search across the columns */
    matelt e = 0.0;
    for (i=1; i <= c->j; i++) {
	matelt f = get_m2d(c,1,i);
	if (f < e) {
	    j = i;
	    e = get_m2d(c,1,i);
	}
    }

    return(j);
}

int pivot_row(struct m2d* a, struct m2d* b, int col)
/* ax = b -- solution constraints */
/* pivot row */
{
    int i,j;

    j = 0;				/* search down the rows */
    matelt e = 0.0;
    for (i=1; i <= a->i; i++) {
	matelt f = get_m2d(b,i,1);
	matelt g = get_m2d(a,i,col);
	if (g > 0.0) {
	    matelt h = f/g;
	    if (h < e || j == 0) {
		j = i;
		e = h;
	    }
	}
    }

    return(j);
}

int lp_update(struct m2d* a, struct m2d* b, struct m2d* c, int pr, int pc, matelt* Z)
		/* ax = b -- solution constraints */
		/* Z = cx -- equation to be minimized */
		/* pivot row, column */
{
    int i,j;

    if (pr == 0 || pc == 0) {
	return(UNBOUNDED);	/* Z is unbounded */
    }

    matelt pivot = get_m2d(a, pr, pc);

				/* update pivot row */
    for (i=1; i <= a->j; i++) {
	set_m2d(a, pr, i, get_m2d(a, pr, i)/pivot);
    }
    set_m2d(b, pr, 1, get_m2d(b, pr, 1)/pivot);

				/* update c vector */
    matelt aux = get_m2d(c, 1, pc);
    for (j=1; j <= c->j; j++) {
	set_m2d(c,1,j,get_m2d(c,1,j)-(aux*get_m2d(a,pr,j)));
    }
    *Z = *Z-(aux*get_m2d(b,pr,1));

				/* update matrix a */
    for (i=1; i <= a->i; i++) {
	aux = get_m2d(a, i, pc);
	if (i == pr || aux == 0.0)
	    continue;

	for (j=1; j <= a->j; j++) {
	    set_m2d(a,i,j,(get_m2d(a,i,j)-aux*get_m2d(a,pr,j)));
	}
	set_m2d(b,i,1,(get_m2d(b,i,1)-aux*get_m2d(b,pr,1)));
    }

    if (avct && basic[pr-1] < 0 && av[pc-1] > 0) {
	avct--;
    } else if (avct && basic[pr-1] > 0 && av[pc-1] < 0) {
	avct++;
    }

    basic[pr-1] = av[pc-1];

    return(SOLUTION);
}

bool is_bas(int v, int n)
		/* basic(?) variable */
		/* number of variables */
{
    int i,j;

    for (i=0; i < n; i++) {
	if (basic[i] < 0) {
	    j = -basic[i];
	} else {
	    j = basic[i];
	}

	if (v == j) {
	    return(true);
	}
    }

    return(false);
}
