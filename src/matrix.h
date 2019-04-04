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

#ifndef MATRIX_H
#define MATRIX_H

typedef double matelt;

struct m2d {		/* 2-d matrices, 1st elt is a(1,1) */
	int i;		/* rows */
	int j;		/* columns */
	matelt *elts;	/* array elements */
};

#define m2d_NULL	(struct m2d *) 0

/*
 * not_m2d: checks for a valid matrix.
 */

extern int not_m2d();


/*
 * new_m2d:  create a new matrix
 */

extern struct m2d *new_m2d();


/*
 * kill_m2d:  deallocate a matrix
 */

extern void kill_m2d();


/*
 * set_m2d:  initialize an element of an array.
 */

extern matelt set_m2d();


/*
 * get_m2d:  get an element from a 2d matrix.
 */


extern matelt get_m2d();


/*
 * add_m2d:  add two matrices and return the result.
 */

extern struct m2d *add_m2d();


/*
 * mult_m2d:  multiply two matrices and return the result.
 */

extern struct m2d *mult_m2d();


/*
 * prt_m2d:  print the matrix on the specified device.
 */

extern void prt_m2d();


/*
 * rd_m2d:  read values into a matrix.
 */

extern void rd_m2d();


/*
 * rdnew_m2d:  read values into a new matrix.
 */

extern struct m2d *rdnew_m2d();


/*
 * trans_m2d:  transpose a matrix.
 */

extern struct m2d *trans_m2d();


/*
 * id_m2d:  get an identity matrix.
 */

extern struct m2d *id_m2d();


/*
 * zero_m2d:  get a zero matrix.
 */

extern struct m2d *zero_m2d();


/*
 * row_m2d:  get a row from a matrix.
 */

extern struct m2d *row_m2d();


/*
 * col_m2d:  get a column from a matrix.
 */

extern struct m2d *col_m2d();


/*
 * scalar_m2d:  multiply a matrix by a scalar.
 */

extern struct m2d *scalar_m2d();


/*
 * submat_m2d:  get a sub-matrix from a matrix.
 */

extern struct m2d *submat_m2d();


/*
 * minor_m2d:  get a minor from a matrix.
 */

extern struct m2d *minor_m2d();


/*
 * det_m2d:  get a matrix determinant.
 */

extern matelt det_m2d();


/*
 * app_r_m2d:  append two matrices by row.
 */

extern struct m2d *app_r_m2d();


/*
 * app_c_m2d:  append two matrices by column.
 */

extern struct m2d *app_c_m2d();


/*
 * rho_m2d:  change a matrix from mxn to ixj.
 */

extern struct m2d *rho_m2d();

extern struct m2d *inv_m2d();


#endif
