

#include<stdio.h>


#define DISPLAY_LOOP	2

#include "htf_dbg.h"

void
display ( char* s, int i )
{
    int j;

    printf ("[htf] in display() - 1.\n");
    printf("%s: %d\n",s, i);
    //__HTF_ADD_LEBEL(__LINE__);
//    j=i*2;
    //__HTF_ADD_LEBEL(__LINE__);
}

int myFunc(int i) {
    i += 1;
    printf ("[htf] myFunc() line: %d.\n", __LINE__);

    return i*3;
}

int
main ()
{
	int i=10,j=0;
	unsigned long pc;

        i = myFunc(i);
	printf ("[htf] main() - 1.\n");
	printf ("[htf] main() line: %d.\n", __LINE__);
	printf ("[htf] main() - 3.\n");

	for (i=0; i< DISPLAY_LOOP; i++) {
	    //__HTF_ADD_LEBEL(__LINE__);
            display ("printf in loop.", i);
	    //__HTF_ADD_LEBEL(__LINE__);
	}

        __HTF_ADD_LEBEL(__LINE__);

        i = myFunc(i);

        __HTF_ADD_LEBEL(__LINE__);

}
