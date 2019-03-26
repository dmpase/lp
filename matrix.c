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

#include "matrix.h"
#include <stdio.h>

char *malloc();
char *strcpy();


/*
 * not_m2d: checks for a valid matrix.
 */

int not_m2d(a)
struct m2d *a;
{
	if (a == m2d_NULL)
		return(1);

	if ((a->i < 1) || (a->j < 1))
		return(1);

	return(0);
}


/*
 * new_m2d:  create a new matrix
 */

struct m2d *new_m2d(i,j)
int i;			/* desired row count */
int j;			/* desired column count */
{
	struct m2d *m;

	if (i<1 || j<1)
		return(m2d_NULL);

	m = (struct m2d *) malloc((unsigned)sizeof(struct m2d));
	m->i = i;
	m->j = j;
	m->elts = (matelt *) malloc((unsigned)sizeof(matelt)*i*j);
	return(m);
}


/*
 * kill_m2d:  deallocate a matrix
 */

void kill_m2d(a)
struct m2d *a;
{
	free((char *) a->elts);
	free((char *) a);
}


/*
 * set_m2d:  initialize an element of an array.
 */

void set_m2d(a,i,j,e)
struct m2d *a;
int i,j;
matelt e;
{
	int idx;

	if (not_m2d(a))
		return;

	idx = (a->i * (j-1)) + (i-1);
	if (idx<0 || (a->i)*(a->j) < idx)
		return;

	*((a->elts)+idx) = e;
	return;
}


/*
 * get_m2d:  get an element from a 2d matrix.
 */


matelt get_m2d(a,i,j)
struct m2d *a;
int i,j;
{
	int idx;

	if (not_m2d(a))
		return((matelt)0);

	idx = (a->i * (j-1)) + (i-1);
	if (idx<0 || (a->i)*(a->j) < idx)
		return((matelt)0);

	return(*((a->elts)+idx));
}


/*
 * add_m2d:  add two matrices and return the result.
 */

struct m2d *add_m2d(a,b)
struct m2d *a;		/* matrix a */
struct m2d *b;		/* matrix b */
{
	int i,j;
	struct m2d *c;

	if (not_m2d(a) || not_m2d(b) || 
		(a->i != b->i) || (a->j != b->j))
		return(m2d_NULL);

	c = new_m2d(a->i, a->j);
	for(i=1; i <= a->i; i++) {
		for(j=1; j <= a->j; j++) {
			set_m2d(c,i,j,get_m2d(a,i,j)+get_m2d(b,i,j));
		}
	}
	return(c);
}


/*
 * mult_m2d:  multiply two matrices and return the result.
 */

struct m2d *mult_m2d(a,b)
struct m2d *a;		/* matrix a */
struct m2d *b;		/* matrix b */
{
	int i,j,k;
	matelt e;
	struct m2d *c;

	if (not_m2d(a) || not_m2d(b) || (a->j != b->i))
		return(m2d_NULL);

	c = new_m2d(a->i, b->j);
	for(i=1; i <= a->i; i++) {
		for(k=1; k <= a->j; k++) {
			for(j=1,e=0.0; j<=a->j; j++) {
				e = e + get_m2d(a,i,j)*get_m2d(b,j,k);
			}
			set_m2d(c,i,k,e);
		}
	}
	return(c);
}


/*
 * prt_m2d:  print the matrix on the specified device.
 */

void prt_m2d(F,a,p1,p2)
FILE *F;
struct m2d *a;
int p1,p2;
{
	int i,j;
	char s[16];

	if (not_m2d(a))
		return;

	strcpy(s, "  % . f\0");
	if (p1<1 || 9<p1 || p2<0 || 9<p2)
		return;
	s[3] = p1+'0';
	s[5] = p2+'0';
	for (i=1; i <= a->i; i++) {
		fprintf(F, "\n\t");
		for (j=1; j <= a->j; j++) {
			fprintf(F,s,get_m2d(a,i,j));
		}
	}
	fprintf(F, "\n");
}


/*
 * pr_m2d:  print the matrix on the specified device.
 */

void pr_m2d(F,a)
FILE *F;
struct m2d *a;
{
	int i,j;

	if (not_m2d(a))
		return;

	for (i=1; i <= a->i; i++) {
		fprintf(F, "\n\r\t");
		for (j=1; j <= a->j; j++) {
			fprintf(F,"%g ",get_m2d(a,i,j));
		}
	}
	fprintf(F, "\n\r");
}


/*
 * rd_m2d:  read values into a matrix.
 */

rd_m2d(F,a)
FILE *F;
struct m2d *a;
{
	matelt e;
	int i,j;

	if (not_m2d(a))
		return;

	for (i=1; i <= a->i; i++) {
		for (j=1; j <= a->j; j++) {
			fscanf(F,"%f",&e);
			set_m2d(a,i,j,e);	/* I could make use */
						/* of by-row storage*/
						/* here, maybe later*/
		}
	}
}


/*
 * rdnew_m2d:  read values into a new matrix.
 */

struct m2d *rdnew_m2d(F,i,j)
FILE *F;
int i,j;
{
	struct m2d *a;

	a = new_m2d(i,j);
	rd_m2d(F,a);
	return(a);
}


/*
 * trans_m2d:  transpose a matrix.
 */

struct m2d *trans_m2d(a)
struct m2d *a;
{
	struct m2d *b;
	int i,j;

	b = new_m2d(a->j, a->i);
	for (i=1; i <= a->i; i++) {
		for (j=1; j <= a->j; j++) {
			set_m2d(b,j,i,get_m2d(a,i,j));
		}
	}
	return(b);
}


/*
 * id_m2d:  get an identity matrix.
 */

struct m2d *id_m2d(size)
int size;
{
	struct m2d *a;
	int i,j;

	a = new_m2d(size,size);
	for (i=1; i <= a->i; i++) {
		for (j=1; j <= a->j; j++) {
			if (i == j)
				set_m2d(a,j,i,(matelt) 1.0);
			else
				set_m2d(a,j,i,(matelt) 0.0);
		}
	}
	return(a);
}


/*
 * zero_m2d:  get a zero matrix.
 */

struct m2d *zero_m2d(mi,mj)
int mi,mj;
{
	struct m2d *a;
	int i,j;

	a = new_m2d(mi,mj);
	for (i=1; i <= a->i; i++) {
		for (j=1; j <= a->j; j++) {
			set_m2d(a,i,j,(matelt) 0.0);
		}
	}
	return(a);
}


/*
 * row_m2d:  get a row from a matrix.
 */

struct m2d *row_m2d(a,i)
struct m2d *a;
int i;
{
	struct m2d *b;
	int j;

	b = new_m2d(1,a->j);
	for (j=1; j <= a->j; j++) {
		set_m2d(b,1,j,get_m2d(a,i,j));
	}
	return(b);
}


/*
 * col_m2d:  get a column from a matrix.
 */

struct m2d *col_m2d(a,j)
struct m2d *a;
int j;
{
	struct m2d *b;
	int i;

	b = new_m2d(a->i, 1);
	for (i=1; i <= a->i; i++) {
		set_m2d(b,i,1,get_m2d(a,i,j));
	}
	return(b);
}


/*
 * scalar_m2d:  scalar multiplication of a matrix.
 */

struct m2d *scalar_m2d(a,r)
struct m2d *a;
matelt r;
{
	struct m2d *b;
	int i,j;

	b = new_m2d(a->i, a->j);
	for (i=1; i <= a->i; i++) {
		for (j=1; j <= a->j; j++) {
			set_m2d(b,i,j,r*get_m2d(a,i,j));
		}
	}
	return(b);
}


/*
 * submat_m2d:  get a sub-matrix.
 *
 * rs is row start, rf is row finish, cs is column start, cs is column finish.
 */

struct m2d *submat_m2d(a, rs, rf, cs, cf)
struct m2d *a;
int rs, rf, cs, cf;
{
	struct m2d *b;
	int i,j;

	if (rs<1)
		rs = 1;
	if (rs>a->i)
		return(m2d_NULL);
	if (rf==0 || rf>a->i)
		rf = a->i;
	if (rf<1)
		return(m2d_NULL);
	if (cs<1)
		cs = 1;
	if (cs>a->j)
		return(m2d_NULL);
	if (cf==0 || cf>a->j)
		cf = a->j;
	if (cf<1)
		return(m2d_NULL);

	b = new_m2d(rf-rs+1, cf-cs+1);
	for (i=rs; i <= rf; i++) {
		for (j=cs; j <= cf; j++) {
			set_m2d(b,i-rs+1,j-cs+1,get_m2d(a,i,j));
		}
	}
	return(b);
}


/*
 * minor_m2d:  get the minor of a matrix.
 */

struct m2d *minor_m2d(a,mi,mj)
struct m2d *a;
int mi,mj;
{
	struct m2d *b;
	int i,j;

	b = new_m2d(a->i-1, a->j-1);
	for (i=1; i <= a->i; i++) {
		for (j=1; j <= a->j; j++) {
			if (i < mi) {
				if (j < mj) {
					set_m2d(b,i,j,get_m2d(a,i,j));
				}
				else if (j != mj) {
					set_m2d(b,i,j-1,get_m2d(a,i,j));
				}
			}
			else if (i != mi) {
				if (j < mj) {
					set_m2d(b,i-1,j,get_m2d(a,i,j));
				}
				else if (j != mj) {
					set_m2d(b,i-1,j-1,get_m2d(a,i,j));
				}
			}
		}
	}
	return(b);
}


/*
 * det_m2d:  get a matrix determinant.
 */

matelt det_m2d(a)
struct m2d *a;
{
	struct m2d *m;
	matelt d;
	int i,k;

	if (a->i != a->j)
		return((matelt) 0.0);
	if (a->i == 1)
		return(get_m2d(a,1,1));
	k = 1;
	d = 0.0;
	for (i=1; i <= a->i; i++) {
		m = minor_m2d(a,i,1);
		d += k * get_m2d(a,i,1) * det_m2d(m);
		kill_m2d(m);
		k = k * -1;
	}
	return(d);
} 


/*
 * app_r_m2d: attach b to the right side of a.
 */

struct m2d *app_r_m2d(a,b)
struct m2d *a,*b;
{
	struct m2d *c;
	int i,j;

	if (a->i != b->i)
		return(m2d_NULL);
	c = new_m2d(a->i, a->j + b->j);
	for (i=1; i <= a->i; i++) {
		for (j=1; j <= a->j; j++) {
			set_m2d(c, i, j, get_m2d(a, i, j));
		}
	}
	for (i=1; i <= b->i; i++) {
		for (j=1; j <= b->j; j++) {
			set_m2d(c, i, a->j+j, get_m2d(b, i, j));
		}
	}
	return(c);
}


/*
 * app_c_m2d: attach b to the bottom of a.
 */

struct m2d *app_c_m2d(a,b)
struct m2d *a,*b;
{
	struct m2d *c;
	int i,j;

	if (a->j != b->j)
		return(m2d_NULL);
	c = new_m2d(a->i + b->i, a->j);
	for (i=1; i <= a->i; i++) {
		for (j=1; j <= a->j; j++) {
			set_m2d(c, i, j, get_m2d(a, i, j));
		}
	}
	for (i=1; i <= b->i; i++) {
		for (j=1; j <= b->j; j++) {
			set_m2d(c, a->i+i, j, get_m2d(b, i, j));
		}
	}
	return(c);
}


/*
 * rho_m2d: transform a matrix from an mxn to ixj matrix.
 */

struct m2d *rho_m2d(a,mi,mj)
struct m2d *a;
{
	struct m2d *b;
	int i,j,k,l;

	b = zero_m2d(mi, mj);
	k = l = 1;
	for (i=1; i <= a->i; i++) {
		for (j=1; j <= a->j; j++) {
			set_m2d(b, k, l, get_m2d(a, i, j));
			l++;
			if (l > mj) {
				l = 1;
				k++;
				if (k > mi)
					break;
			}
		}
		if (k > mi)
			break;
	}
	return(b);
}

/*
 * inv_m2d:  get the multiplicative inverse of a matrix.
 */

struct m2d *inv_m2d(a)
struct m2d *a;
{
	struct m2d *b, *t;
	matelt det,e;
	int i,j;

	b = new_m2d(a->i, a->j);
	det = det_m2d(a);
	for (i=1; i <= a->i; i++) {
		for (j=1; j <= a->j; j++) {
			t = minor_m2d(a,i,j);
			e = det_m2d(t)/det;
			if ((i+j)%2)
				set_m2d(b,j,i,-e);
			else
				set_m2d(b,j,i,e);
			kill_m2d(t);
		}
	}
	return(b);
}
