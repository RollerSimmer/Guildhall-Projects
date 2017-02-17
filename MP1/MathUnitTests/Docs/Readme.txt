Known Issues:
* The bell random function has a lot of spikes, but this may be due to the core random function that it calls.  The overall shape of the graph still looks close enough to a Bell Curve to pass.
* Line segment intersection is the longest method, and therefore the most likely to contain a bug.  However I did not find any problems when testing it.
* No other known issues 

How to Use:
Use the following lines to include the library headers into your code.
#include "Engine/MathUtils.h"
#include "Engine/Vector2.h"
#include "Engine/LineSegment2.h"
#include "Engine/

Deep Learning:
(MP1 Assingnment 1)
	I had to redo the projects in order to get them to work.  After editing different project and solution settings to no avail, I started fresh and followed more thoroughly all the instructions given in the assignment.  In writing the test code, I had to try different initial values for the vectors to get a result that showed that the function that I was testing was working.

(MP1 Assignment 2)
	I added test functions for each method that I suspected would produce something off.  I added a line intersection function thinking it would be useful in the future.  This function took the most time to get working correctly.  It came down to misassigned variables in the  multiple term quotient used to calculate an intersection point.  Most everything else went smoothly.  LineSegment2's methods borrow heavily from those of the Vector2 class.  In testing some of the functions, I found it easier to add functions that created an object quickly from basic value arguments, like the makeVector2() function.  