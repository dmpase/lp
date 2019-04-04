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
#include <sys/time.h>
#include <curses.h>
#include "lp.h"
#include "or.h"

static void eat_look();

void init_sys(int argc, char*argv[])
{
    FILE *fp;
    char err[80];

    initscr();
    raw();
    noecho();
    nonl();
    draw_frame();

    if (argc > 1 && (fp=fopen(argv[1],"r"))) {
	read_system(fp);
    } else {
	init_session();
    }

    print_screen();
    print_field(rcur, ccur, 1, area);
    move(0,71);
    printw("%3d,%3d",rcur,ccur);
    if (argc > 1 && ! fp) {
	sprintf(err,"Cannot open %s",argv[1]);
	errorw(err);
    }

    move(LINES-1,0);
    refresh();
    eat_look();
}

void end_sys()
{
    clear_errw();
    move(LINES-1,0);
    refresh();
    endwin();
}

void get_com()
{
    int c;
    char ans[80];

    errorw("type `?' for help");
    refresh();
    sleep(3);
    do {
	c = getch();
	clear_errw();
	switch(c) {
	case 3 :		/* ^C */
	    delete_col(ccur);
	    break;
	case 4 :		/* ^D */
	    clearok(curscr,TRUE);
	    break;
	case 5 :		/* ^E */
	    if (modified) {
		ask("System modified - really erase? (n)[yn] ", ans);
		if (ans[0] == 'y' || ans[0] == 'Y')
		    erase_session();
	    } else {
		erase_session();
	    }
	    break;
	case 8 :		/* ^H */
	    add_col(ccur);
	    break;
	case 10 :		/* ^J */
	    add_row(rcur+1);
	    move_position(rcur+1,ccur);
	    break;
	case 11 :		/* ^K */
	    add_row(rcur);
	    break;
	case 12 :		/* ^L */
	    add_col(ccur+1);
	    move_position(rcur,ccur+1);
	    break;
	case 13 :		/* ^M */
	    markhov_chain();
	    break;
	case 18 :		/* ^R */
	    delete_row(rcur);
	    break;
	case 26 :		/* ^Z */
	    tstp();
	    break;
	case ' ' :
	    errorw("");
	    break;
	case '?' :
	    helper();
	    break;
	case 'C' :
	    copy_rc();
	    break;
	case 'H' :
	    move_position(rcur,max(1,ccur-csmx));
	    break;
	case 'J' :
	    move_position(min(rmax,rcur+rsmx),ccur);
	    break;
	case 'K' :
	    move_position(max(1,rcur-rsmx),ccur);
	    break;
	case 'L' :
	    move_position(rcur,min(cmax,ccur+csmx));
	    break;
	case 'M' :
	    maximize_system();
	    break;
	case 'P' :
	    print_session();
	    break;
	case 'Q' :
	    if (modified) {
		ask("System modified - really quit? (n)[yn] ", ans);
		if (ans[0] == 'y' || ans[0] == 'Y') {
		    c = 'q';
		} else {
		    c = ' ';
		}
	    } else {
		c = 'q';
	    }
	    break;
	case 'R' :
	    restore_session();
	    break;
	case 'S' :
	    save_session();
	    break;
	case 'a' : case 'b' : case 'c' : case 'd' :
	case 'e' : case 'n' : case 'x' : case 'z' :
	    move_area(c);
	    break;
	case 'g' :
	    go_to();
	    break;
	case 'i' :
	    enter_field();
	    break;
	case 'h' :
	    move_position(rcur,ccur-1);
	    break;
	case 'j' :
	    move_position(rcur+1,ccur);
	    break;
	case 'k' :
	    move_position(rcur-1,ccur);
	    break;
	case 'l' :
	    move_position(rcur,ccur+1);
	    break;
	case 'm' :
	    minimize_system();
	    break;
	case 'q' :
	    if (modified) {
		ask("System modified - really quit? (n)[yn] ", ans);
	    } else {
		ask("Really quit? (n)[yn] ",ans);
	    }
	    if (ans[0] == 'y' || ans[0] == 'Y') {
		c = 'q';
	    } else {
		c = ' ';
	    }
	    break;
	default :
	    if (c < ' ') {
		char err[80];

		sprintf(err, "^%c (?)", c+'A'-1);
		errorw(err);
	    } else {
		char err[80];

		sprintf(err, "%c (?)", c);
		errorw(err);
	    }
	}

	move(0,71);		/* print new position in upper rhc */
	printw("%3d,%3d",rcur,ccur);
	print_field(rcur, ccur, 1, area);
	move(LINES-1,0);
	refresh();
    } while (c != 'q');
}

void draw_frame()
{
    int i;

    move(0,0);
    printw("d      \\ x    |");
    move(0,68);
    printw("|");
    move(1,0);
    printw("--------------+-----------------------------------------------------+-----------");
    for(i=1; i<rsmx; i++) {
	move(2*i,14);
	printw("|");
	move(2*i+1,14);
	printw("|");
    }
    i = 2*i;
    move(i++,14);
    printw("|");
    move(i++,0);
    printw("--------------+-----------------------------------------------------+-----------");
    move(CLINE,14);
    printw("=");
    move(i++,68);
    printw("|");
    move(i++,68);
    printw("|");
    move(i,68);
    printw("|");
}

static void eat_look()
{
    int r,w,e;
    struct timeval zip;

    zip.tv_sec = zip.tv_usec = 0;
    r = 1;
    w = e = 0;
    while (select(1,&r,&w,&e,&zip)) {
	getch();
    }
}

void init_session()
{
    area='a';
    rscr = 1;	/* row position of screen */
    cscr = 1;	/* column position of screen */
    rcur = 1;	/* row position of cursor */
    ccur = 1;	/* column position of cursor */
    rmax = 1;	/* maximum rows in the system */
    cmax = 1;	/* maximum columns in the system */

    cur_soln = -1;
    modified = 0;

    a[0][0] = 0.0;
    b[0] = 0.0;
    c[0] = 0.0;
    d[0][0] = '\0';
    e[0] = '<';
    x[0][0] = '\0';
    z[0] = 'Z';
    z[1] = '\0';
}

void erase_session()
{
    int i;

    for (i=rscr; i < min(rmax+1, rscr+rsmx); i++) {
	blank_row(i);
    }

    for (i=cscr; i < min(cmax+1, cscr+csmx); i++) {
	blank_col(i);
    }

    init_session();
    print_screen();
}

void helper()
{
    char a[40];

    move(LINES-1,0);
    refresh();
    nl(); echo(); noraw();
    if (fork() == 0) {
	execl("/usr/ucb/more", "Marvelous Marvin",
	    "/afs/aix.kingston.ibm.com/u/pase/src/utils/lp/lp.help",
	    0);
	    exit(1);
    }
    wait(0);
    nonl(); noecho(); raw();
    ask("Ready? ",a);
    clearok(stdscr, TRUE);
}
