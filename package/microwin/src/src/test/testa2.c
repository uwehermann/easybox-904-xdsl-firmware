
//#define muldiv255(a,b)	(((a)*(b))/255)		/* slow divide, exact*/
#define muldiv255(a,b)	(((a)*((b)+1))>>8)		/* very fast, 92% accurate*/
//#define muldiv255(a,b)	((((a)+((a)>>7))*(b))>>8)	/* fast, 35% accurate*/
	char *src8, *dst8;
	unsigned char alpha;
	unsigned char pd;
main() {

	for(;;) {
		pd = *dst8;
		*dst8++ = muldiv255(alpha, *src8++ - pd) + pd;
	}
}
