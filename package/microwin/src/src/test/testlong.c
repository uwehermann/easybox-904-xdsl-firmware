int w;

main()
{
	unsigned long *dst;
	unsigned char *p;
volatile unsigned char r, g, b;
volatile unsigned long fg;
	int i;

	for (i=0; i<w; i++)
		*dst++ = fg;

	for (i=0; i<w; i++) {
		p[0] = r;
		p[1] = g;
		p[2] = b;
		p += 3;
	}
}
