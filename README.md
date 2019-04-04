# lp
lp is a linear programming system written in 1988 while the author
(Douglas Pase) was attending the Oregon Graduate Center. The code
is written in the C programming language using the "curses" CRT 
(e.g., VT-100) terminal control package. 

The solver uses the "Big M" version of the Simplex method, as 
described in chapter 2 of "Operations Research" by Hillier and 
Lieberman, 2nd Ed., Holden Day, Copyright 1968 and 1974.

The package has undergone minor re-formatting and updates to 
reflect changes in the C programming language from 1988 to 2019.
The package implements a 2-dimensional matrix package to ease, 
somewhat, the burden of managing matrix storage and addressing. 
In hind sight it doesn't help that much. (Hey, after more than
30 years of professional programming I've learned a few things!)

At the time of its original writing (1987-1988) the solver worked
correctly and did so for several years, but at sometime in the past 
a few lines got deleted in the routine setup_session. No doubt I
was trying to make it "better" in some way, and unknowingly hit 
the wrong key. That created a bug that caused constraints of the 
type >= to to produce bad results. The package sat unused for 
many years until I finally figured out what the problem was and 
fixed it.

A somewhat different problem today is that versions of the "curses"
terminal control library are not as easy to come by as they once
were. The end result is that the code, for the most part, is no
longer runnable in its current form. Fortunately, only the editor 
portion is dependent on the "curses" library. The solver still works 
fine if it could be extracted from the rest of the system and fed 
data. I have done exactly that, sort of. 

What I have done is I have translated the key routines from C to 
Java and placed them into a separate repository, along with routines 
to read data from a text file. The result is a new Github project 
called "dmpase/lp-java". There is no fancy session editor, but the
act of creating an intuitively structured text file appears to be
no real burden. All of this is described in the new project.
