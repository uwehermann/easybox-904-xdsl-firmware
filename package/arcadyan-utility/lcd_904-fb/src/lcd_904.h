#ifndef _LCD_904_H_
#define _LCD_904_H_


#ifdef __KERNEL__
 #include <linux/errno.h>
 #include <linux/ioctl.h>
#else
 #include <sys/errno.h>
 #include <sys/ioctl.h>
#endif 


#define	LCD904_MAJOR				233
#define LCD904_MINORS				1

#define	LCD904_PROC_FILE			"driver/lcd904"
#define	LCD904_DEV_FILE				"lcd904"

#define	LCD_COL						240		// in portrait
#define	LCD_ROW						320		// in portrait

#define	LCD_BL_MIN					0			// dark
#define	LCD_BL_MAX					16			// full
#define	LCD_BL_DFT					LCD_BL_MAX

#define	LCD_LED_MIN					0			// dark
#define	LCD_LED_MAX					16			// full
#define	LCD_LED_DFT					LCD_LED_MAX

#define	LCD_CONTRAST_MIN			0
#define	LCD_CONTRAST_MAX			31
#define	LCD_CONTRAST_DFT			12


#define	LCD_IOC_DEVTYPE				0x53

#define	LCD_IOC_NR_INIT				1	// initialize LCD driver IC
#define	LCD_IOC_NR_SHUTDOWN			2	// shutdown LCD driver IC
#define	LCD_IOC_NR_SYNC				3	// sync dirty mirrored GRAM in driver to LCD GRAM
#define	LCD_IOC_NR_REFRESH			4	// forcedly refresh a region of LCD GRAM
#define	LCD_IOC_NR_REGION			5	// get/set graphic RAM in kernel driver
#define	LCD_IOC_NR_BL				6	// get/set backlight
#define	LCD_IOC_NR_CONTRAST			7	// get/set contrast
#define	LCD_IOC_NR_REGISTER			8	// get/set LCD driver IC register
#define	LCD_IOC_NR_COLOR			9	// draw a region in solid color
#define	LCD_IOC_NR_GRADIENT			10	// draw a region in gradient color
#define	LCD_IOC_NR_ORIENTATION		11	// get/set orientation
#define	LCD_IOC_NR_PROPERTY			12	// get all properties
#define	LCD_IOC_NR_LED				13	// get/set backlight

#define	LCD_IO(   reqId )			_IO(   LCD_IOC_DEVTYPE, reqId )
#define	LCD_IOR(  reqId, dataType )	_IOR(  LCD_IOC_DEVTYPE, reqId, dataType )
#define	LCD_IOW(  reqId, dataType )	_IOW(  LCD_IOC_DEVTYPE, reqId, dataType )
#define	LCD_IOWR( reqId, dataType )	_IOWR( LCD_IOC_DEVTYPE, reqId, dataType )

typedef unsigned short coord_t;
typedef unsigned short color_t;

#define	RGB16( r, g, b )		( ( ((color_t)(r) & 0x1F) << 11 ) | ( ((color_t)(g) & 0x3F) << 5 ) | ((color_t)(b) & 0x1F) )
#define	COLOR16_R( color16 )	( ((color16)>>11) & 0x1F )
#define	COLOR16_G( color16 )	( ((color16)>> 5) & 0x3F )
#define	COLOR16_B( color16 )	( ((color16)>> 0) & 0x1F )
#define	RGB16_TO_BGR16(color16)	RGB16( COLOR16_B( color16 ), COLOR16_G( color16 ), COLOR16_R( color16 ) )

#define	RGB24( r, g, b )		( ( ((unsigned long)(r) & 0xFF) << 16 ) | ( ((unsigned long)(g) & 0xFF) << 8 ) | ((unsigned long)(b) & 0xFF) )
#define	COLOR24_R( color24 )	( ((color24)>>24) & 0xFF )
#define	COLOR24_G( color24 )	( ((color24)>>16) & 0xFF )
#define	COLOR24_B( color24 )	( ((color24)>> 8) & 0xFF )

#define	RGB24_TO_16( r, g, b ) ( ( (((unsigned long)(r)>>3) & 0x1F) << 11 ) | ( (((unsigned long)(g)>>2) & 0x3F) << 5 ) | (((unsigned long)(b)>>3) & 0x1F) )

#define	COLOR16_BLACK			RGB24_TO_16( 0x00, 0x00, 0x00 )
#define	COLOR16_RED				RGB24_TO_16( 0xFF, 0x00, 0x00 )
#define	COLOR16_GREEN			RGB24_TO_16( 0x00, 0xFF, 0x00 )
#define	COLOR16_BLUE			RGB24_TO_16( 0x00, 0x00, 0xFF )
#define	COLOR16_YELLOW			RGB24_TO_16( 0xFF, 0xFF, 0x00 )
#define	COLOR16_MAGENTA			RGB24_TO_16( 0xFF, 0x00, 0xFF )
#define	COLOR16_CYAN			RGB24_TO_16( 0x00, 0xFF, 0xFF )
#define	COLOR16_WHITE			RGB24_TO_16( 0xFF, 0xFF, 0xFF )
#define	COLOR16_DEFAULT			COLOR16_BLACK

#define	LCD_ORIENTATION_PORTRAIT	0
#define	LCD_ORIENTATION_LANDSCAPE	1
#define	LCD_ORIENTATION_DEFAULT		LCD_ORIENTATION_LANDSCAPE

typedef struct {
	coord_t			width;			// LCD width in pixels (in current orientation)
	coord_t			height;			// LCD height in pixels (in current orientation)
	unsigned char	orientation;	// Whether the LCD orientation can be modified
	unsigned char	hwscrolling;	// Whether the LCD support HW scrolling
	unsigned char	backlight;		// backlight level, LCD_BL_MIN~LCD_BL_MAX
	unsigned char	contrast;		// contrast level, LCD_CONTRAST_MIN~LCD_CONTRAST_MAX
	unsigned char	led;			// LED level, LCD_LED_MIN~LCD_LED_MAX
} stLcdIoctlProperty;

typedef struct {
	coord_t	x;	/* X coordinate of point, 0~LCD_COL */
	coord_t	y;	/* Y coordinate of point, 0~LCD_ROW */
} stLcdIoctlPoint;

typedef struct {
	stLcdIoctlPoint	tl;		/* coordinate of top-left vortex of region */
	stLcdIoctlPoint	br;		/* coordinate of bottom-right vortex of region */
} stLcdIoctlRegion;

typedef struct {
	stLcdIoctlRegion	region;		/* current working area of LCD */
	color_t				color16[2];	/* data buffer of region */
									/*   size is (number of pixels)*sizeof(color_t) */
									/*   direction is from top-left to bottom-right, row by row */
} stLcdIoctlRegionData; /* data structure of LCD_IOC_GET_REGION or LCD_IOC_DRAW_REGION */

typedef struct {
	unsigned char	reg;	/* register ID */
	unsigned short	data;	/* register data */
} stLcdIoctlRegister;


#define	LCD_IOC_INIT				LCD_IO(   LCD_IOC_NR_INIT )
#define	LCD_IOC_SHUTDOWN			LCD_IO(   LCD_IOC_NR_SHUTDOWN )
#define	LCD_IOC_SYNC				LCD_IO(   LCD_IOC_NR_SYNC )
#define	LCD_IOC_REFRESH				LCD_IOW(  LCD_IOC_NR_REFRESH,		stLcdIoctlRegion )
#define	LCD_IOC_REGION_GET			LCD_IORW( LCD_IOC_NR_REGION,		stLcdIoctlRegionData )
#define	LCD_IOC_REGION_SET			LCD_IOW(  LCD_IOC_NR_REGION,		stLcdIoctlRegionData )
#define	LCD_IOC_BL_GET				LCD_IOR(  LCD_IOC_NR_BL,			unsigned char )
#define	LCD_IOC_BL_SET				LCD_IOW(  LCD_IOC_NR_BL,			unsigned char )
#define	LCD_IOC_CONTRAST_GET		LCD_IOR(  LCD_IOC_NR_CONTRAST,		unsigned char )
#define	LCD_IOC_CONTRAST_SET		LCD_IOW(  LCD_IOC_NR_CONTRAST,		unsigned char )
#define	LCD_IOC_REGISTER_GET		LCD_IORW( LCD_IOC_NR_REGISTER,		stLcdIoctlRegister )
#define	LCD_IOC_REGISTER_SET		LCD_IOW(  LCD_IOC_NR_REGISTER,		stLcdIoctlRegister )
#define	LCD_IOC_COLOR_SET			LCD_IOW(  LCD_IOC_NR_COLOR,			stLcdIoctlRegionData )
#define	LCD_IOC_GRADIENT_SET		LCD_IOW(  LCD_IOC_NR_GRADIENT,		stLcdIoctlRegionData )
#define	LCD_IOC_ORIENTATION_GET		LCD_IOR(  LCD_IOC_NR_ORIENTATION,	unsigned char )
#define	LCD_IOC_ORIENTATION_SET		LCD_IOW(  LCD_IOC_NR_ORIENTATION,	unsigned char )
#define	LCD_IOC_PROPERTY_GET		LCD_IOR(  LCD_IOC_NR_PROPERTY,		stLcdIoctlProperty )
#define	LCD_IOC_LED_GET				LCD_IOR(  LCD_IOC_NR_LED,			unsigned char )
#define	LCD_IOC_LED_SET				LCD_IOW(  LCD_IOC_NR_LED,			unsigned char )


#endif /* _LCD_904_H_ */
