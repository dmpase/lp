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
#include <stdlib.h>
#include <ncurses/curses.h>
/* #include <curses.h> */
#include <sys/file.h>
#include "lp.h"
#include "or.h"

void markhov_chain()
{
    struct m2d *sq, *ch, *tmp;
    char ans[20];
    int gen;
    int i,j;

    if (rmax != cmax) {
	errorw("Invalid Markov chain");
	return;
    } 
    else {
	ask("Generations?  ",ans);
	gen = atoi(ans);
	if (gen < 1) {
	    if (ans[0] != '\0') {
		errorw("Must be > 0");
	    }
	    else {
		errorw("");
	    }
	    return;
	}
    }

    sq = new_m2d(rmax,cmax);
    ch = id_m2d(rmax);
    for (i=0; i < rmax; i++) {
	for (j=0; j < cmax; j++) {
	    set_m2d(sq,i+1,j+1,a[i][j]);
	}
    }

    for ( ; gen; gen>>=1) {
	if (gen & 1) {		/* multiply ch by sq */
	    tmp = mult_m2d(sq,ch);
	    kill_m2d(ch);
	    ch = tmp;
	}

	tmp = mult_m2d(sq,sq);
	kill_m2d(sq);
	sq = tmp;
    }

    for (i=0; i < rmax; i++) {
	for (j=0; j < cmax; j++) {
	    a[i][j] = get_m2d(ch,i+1,j+1);
	}
    }

    kill_m2d(ch);
    kill_m2d(sq);

    for (i=0; i < rmax; i++) {
	float sum;

	sum = 0.0;
	for (j=0; j < cmax; j++) {
	    sum += a[i][j];
	}
	b[i] = sum - 1.0;
    }

    print_screen();
}
