#ifndef __SWITCH_VENDOR_COMMON__
#define __SWITCH_VENDOR_COMMON__

#ifdef SW_ENABLED
#undef SW_ENABLED
#endif

#ifdef SW_DISABLED
#undef SW_DISABLED
#endif

#define SW_ENABLED		(1==1)
#define SW_DISABLED 	(1==0)

/*-------------------------------------------------------------
  Bit Operation.
--------------------------------------------------------------- */
#define	BIT00 		0x01
#define BIT01 		0x02
#define BIT02 		0x04
#define BIT03 		0x08
#define BIT04 		0x10
#define BIT05 		0x20
#define BIT06 		0x40
#define BIT07 		0x80

#define	WBIT00 		0x0001
#define WBIT01 		0x0002
#define WBIT02 		0x0004
#define WBIT03 		0x0008
#define WBIT04 		0x0010
#define WBIT05 		0x0020
#define WBIT06 		0x0040
#define WBIT07 		0x0080
#define WBIT08 		0x0100
#define WBIT09 		0x0200
#define WBIT10 		0x0400
#define WBIT11 		0x0800
#define WBIT12 		0x1000
#define WBIT13 		0x2000
#define WBIT14 		0x4000
#define WBIT15 		0x8000

#define DWBIT00 	0x00000001
#define DWBIT01 	0x00000002
#define DWBIT02 	0x00000004
#define DWBIT03 	0x00000008
#define DWBIT04 	0x00000010
#define DWBIT05 	0x00000020
#define DWBIT06 	0x00000040
#define DWBIT07 	0x00000080
#define DWBIT08 	0x00000100
#define DWBIT09 	0x00000200
#define DWBIT10 	0x00000400
#define DWBIT11 	0x00000800
#define DWBIT12 	0x00001000
#define DWBIT13 	0x00002000
#define DWBIT14 	0x00004000
#define DWBIT15 	0x00008000
#define DWBIT16 	0x00010000
#define DWBIT17 	0x00020000
#define DWBIT18 	0x00040000
#define DWBIT19 	0x00080000
#define DWBIT20 	0x00100000
#define DWBIT21 	0x00200000
#define DWBIT22 	0x00400000
#define DWBIT23 	0x00800000
#define DWBIT24 	0x01000000
#define DWBIT25 	0x02000000
#define DWBIT26 	0x04000000
#define DWBIT27 	0x08000000
#define DWBIT28 	0x10000000
#define DWBIT29 	0x20000000
#define DWBIT30 	0x40000000
#define DWBIT31 	0x80000000

#endif // __SWITCH_VENDOR_COMMON__