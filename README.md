# lp
lp is a linear programming system written in 1988 while the author
(Douglas Pase) was attending the Oregon Graduate Center. The code
is written in the C programming language using the "curses" CRT 
(e.g., VT-100) terminal windowing package. 

The solver uses the "Big M" version of the Simplex method, as 
described in chapter 2 of "Operations Research" by Hillier and 
Lieberman, 2nd Ed., Holden Day, Copyright 1968 and 1974.

The package has undergone minor re-formatting and updates to 
reflect changes in the C programming language from 1988 to 2019.
The package implements a 2-dimension matrix package to ease, 
somewhat, the burden of managing matrix storage and addressing. 
In hind sight it doesn't help that much. (Hey, after more than
30 years of professional programming, I learned a few things!)

At the time of its original writing (1987-1988) the solver worked
correctly, but sometime afterwards, a few lines got deleted in the
routine setup_session. That created a bug that caused constraints
of the type >= to to produce bad results. After 30 years, I finally
figured out what the problem was and fixed it.

A somewhat different problem today is that versions of the "curses"
terminal control library are not as easy to come by as they once
were. The end result is that the code, for the most part, is no
longer runnable. 

Fortunately, only the editor portion is dependent on the "curses"
library. The solver still works fine if it could be extracted
from the rest of the system and fed data. I have done exactly that,
sort of. 

I have translated the key routines from C to Java and placed them 
into a separate repository, along with routines to read data from a 
text file. The result is a new Github project called "dmpase/lp-java".
