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

#ifndef LP_H
#define LP_H

#include "matrix.h"

#define min(x,y) (x<y?x:y)
#define max(x,y) (x>y?x:y)

#define CLIMIT 100
#define RLIMIT 100

#define XLINE 0
#define ALINE 2
#define CLINE 18
#define SLINE 20
#define NLINE 22

#define DCOL 0
#define ACOL 16
#define BCOL 71
#define ECOL 67

extern char area;	/* ax <= b, Z = cx, non-neg. constraints {axbcn} */
extern int rscr;	/* row position of screen */
extern int cscr;	/* column position of screen */
extern int rcur;	/* row position of cursor */
extern int ccur;	/* column position of cursor */
extern int rmax;	/* maximum rows in the system */
extern int cmax;	/* maximum columns in the system */
extern int rsmx;	/* maximum rows on the screen */
extern int csmx;	/* maximum columns on the screen */

extern int cur_soln;
extern int soln_type;
extern int modified;

extern double a[RLIMIT][CLIMIT];
extern double b[RLIMIT];
extern double c[CLIMIT];
extern double s[CLIMIT];

extern matelt Z;

extern char d[RLIMIT][14];
extern char e[RLIMIT];
extern char x[CLIMIT][10];
extern char z[14];

double ator();

#endif
