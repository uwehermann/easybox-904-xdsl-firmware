
#define muldiv255(a,b)	((((unsigned char)(a))*((((unsigned char)(b))+1)))>>8)		/* very fast, 92% accurate*/

#if 0
unsigned char x, y;
unsigned int  a, b;

foo()
{
	func(x-y);
	func(a-b);
}
#endif

main()
{
	unsigned long s, d, a;		// must be unsigned char!!
	unsigned long res1, res2;

	for(a=0; a<256; ++a)
		for(s=0; s<256; ++s) {
			for(d=0; d<256; ++d) {
 				//bg =muldiv255(a,fg-bg) + bg
				res1 = muldiv255(a, s-d) + d;

 				//bg = (a*(fg-bg))/255 + bg
				res2 = (unsigned char)((unsigned char)a*((unsigned char)s - (unsigned char)d))/255U + d;

 				//bg =muldiv255(fg-bg, a) + bg
				//res2 = muldiv255(s-d, a) + d;

				if(res1 != res2)
					printf("Fail a %08lx s %08lx d %08lx (%08lx %08lx)\n", a, s, d, res1, res2);
		}
	}
}
