#include "switch_vendor_project.h"

#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)
#include <asm-mips/delay.h>

#include <asm/ifx/vr9/vr9.h>
#include <switch_api/ifxmips_sw_reg.h>
#elif((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
#endif

#define RTK_MDC_MDIO_DUMMY_ID       0
#define RTK_MDC_MDIO_CTRL0_REG      31
#define RTK_MDC_MDIO_CTRL1_REG      21

#define RTK_MDC_MDIO_START_REG      29
#define RTK_MDC_MDIO_ADDRESS_REG    23
#define RTK_MDC_MDIO_DATA_WRITE_REG 24
#define RTK_MDC_MDIO_DATA_READ_REG  25
#define RTK_MDC_MDIO_PREAMBLE_LEN   32

#define RTK_MDC_MDIO_START_OP       0xFFFF
#define RTK_MDC_MDIO_ADDR_OP        0x000E
#define RTK_MDC_MDIO_READ_OP        0x0001
#define RTK_MDC_MDIO_WRITE_OP       0x0003

#define RTK_MDIO_WRITE_CMD  				((0 << 11)| (1 << 10))
#define RTK_MDIO_READ_CMD  					((1 << 11)| (0 << 10))

#define RTK_REG32(addr)							(*(volatile unsigned int *) (addr))


void ifx_vr9_mdio_init(void)
{
#if 0 // ref. vr9_new.h
    *BSP_GPIO_P2_ALTSEL0 	= *BSP_GPIO_P2_ALTSEL0 | (0xc00);
    *BSP_GPIO_P2_ALTSEL1 	= *BSP_GPIO_P2_ALTSEL1 & ~(0xc00);
    *BSP_GPIO_P2_DIR 			= *BSP_GPIO_P2_DIR  | 0x800;
    *BSP_GPIO_P2_OD 			= *BSP_GPIO_P2_OD | 0xc00;
#else
		*IFX_GPIO_P2_ALTSEL0 	= *IFX_GPIO_P2_ALTSEL0 | (0xc00);
		*IFX_GPIO_P2_ALTSEL1 	= *IFX_GPIO_P2_ALTSEL1 & ~(0xc00);

		*IFX_GPIO_P2_DIR 			= *IFX_GPIO_P2_DIR | 0x800;
		*IFX_GPIO_P2_OD 			= *IFX_GPIO_P2_OD | 0xc00;
#endif
}

void ifx_vr9_mdio_write(unsigned char phyaddr, unsigned char phyreg, unsigned short data)
{
    unsigned short i=0;
#if 0  // ref. vr9_new.h
    REG32(VR9_ETHSW_MDIO_WRITE)= data;
    while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000);
    REG32(VR9_ETHSW_MDIO_CTRL) =  RTK_MDIO_WRITE_CMD | (((unsigned int)phyaddr)<<5) | ((unsigned int) phyreg)| 0x1000 ;

		while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000)
    {
    		i++;
    		if(i>0x7fff)
    		{
    		  	//printf("MDIO access time out!\n");
    		  	break;
    		}
    }
#else
    RTK_REG32(MDIO_WRITE_REG)= data;
    while(RTK_REG32(MDIO_CTRL_REG) & 0x1000);
    RTK_REG32(MDIO_CTRL_REG) =  RTK_MDIO_WRITE_CMD | (((unsigned int)phyaddr)<<5) | ((unsigned int) phyreg)| 0x1000 ;

		while(RTK_REG32(MDIO_CTRL_REG) & 0x1000)
    {
    		i++;
    		if(i>0x7fff)
    		{
    		  	//printf("MDIO access time out!\n");
    		  	break;
    		}
    }
#endif
}

unsigned short ifx_vr9_mdio_read(unsigned char phyaddr, unsigned char phyreg)
{
    unsigned short i=0;
#if 0
    while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000);
    REG32(VR9_ETHSW_MDIO_CTRL) = RTK_MDIO_READ_CMD | (((unsigned int)phyaddr)<<5) | ((unsigned int) phyreg)| 0x1000 ;

		while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000)
    {
	    	i++;
	    	if(i>0x7fff)
	    	{
	    		  //printf("MDIO access time out!\n");
	    		  break;
	    	}
    }

    return((unsigned short)(REG32(VR9_ETHSW_MDIO_READ)));
#else
    while(RTK_REG32(MDIO_CTRL_REG) & 0x1000);
    RTK_REG32(MDIO_CTRL_REG) = RTK_MDIO_READ_CMD | (((unsigned int)phyaddr)<<5) | ((unsigned int) phyreg)| 0x1000 ;

		while(RTK_REG32(MDIO_CTRL_REG) & 0x1000)
    {
	    	i++;
	    	if(i>0x7fff)
	    	{
	    		  //printf("MDIO access time out!\n");
	    		  break;
	    	}
    }

    return((unsigned short)(RTK_REG32(MDIO_READ_REG)));
#endif
}

void ifx_ltq_write_mdio(unsigned int phyAddr, unsigned int regAddr,unsigned int data )
{
    unsigned int reg, i;

		reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
    }

    reg = MDIO_READ_WDATA(data);
    SW_WRITE_REG32( reg, MDIO_WRITE_REG);

    reg = ( MDIO_CTRL_OP_WR | MDIO_CTRL_PHYAD_SET(phyAddr) | MDIO_CTRL_REGAD(regAddr) );
    reg |= MDIO_CTRL_MBUSY;
    SW_WRITE_REG32( reg, MDIO_CTRL_REG);

    udelay(100);

		reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
        udelay(10);
    }

    //ifxmips_mdelay(10);
		for(i=10; i>0; i--)
			udelay(1000);

		return ;
}

unsigned short ifx_ltq_read_mdio(unsigned int phyAddr, unsigned int regAddr )
{
		unsigned int reg, i;

    reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
    }

		reg = ( MDIO_CTRL_OP_RD | MDIO_CTRL_PHYAD_SET(phyAddr) | MDIO_CTRL_REGAD(regAddr) );
    reg |= MDIO_CTRL_MBUSY;
    SW_WRITE_REG32( reg, MDIO_CTRL_REG);

		reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
    }

		//ifxmips_mdelay(10);
		for(i=10; i>0; i--)
			udelay(1000);

		reg = SW_READ_REG32(MDIO_READ_REG);

		return (MDIO_READ_RDATA(reg));
}


unsigned short RTL83XX_SMI_READ(unsigned short u16Addr)
{
#if defined(MDIO_USED) // Lantiq definition, better performance
		/* Write address control code to register 31 */
	  ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_CTRL0_REG, RTK_MDC_MDIO_ADDR_OP);
		/* Write address to register 23 */
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_ADDRESS_REG, u16Addr);
		/* Write read control code to register 21 */
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_CTRL1_REG, RTK_MDC_MDIO_READ_OP);
		/* Read data from register 25 */
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		return (u16)ifx_vr9_mdio_read(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_DATA_READ_REG );
#else
		/* Write address control code to register 31 */
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_CTRL0_REG, RTK_MDC_MDIO_ADDR_OP);
		/* Write address to register 23 */
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_ADDRESS_REG, u16Addr);
		/* Write read control code to register 21 */
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_CTRL1_REG, RTK_MDC_MDIO_READ_OP);
		/* Read data from register 25 */
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		return (u16)ifx_ltq_read_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_DATA_READ_REG );
#endif
}

void RTL83XX_SMI_WRITE(unsigned short u16Addr, unsigned short u16Data)
{
#if defined(MDIO_USED) // Lantiq definition
		/* Write address control code to register 31 */
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_CTRL0_REG, RTK_MDC_MDIO_ADDR_OP );
		/* Write address to register 23 */
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_ADDRESS_REG, u16Addr );
		/* Write data to register 24 */
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_DATA_WRITE_REG, u16Data );
		/* Write data control code to register 21 */
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_vr9_mdio_write(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_CTRL1_REG, RTK_MDC_MDIO_WRITE_OP );
#else
		/* Write address control code to register 31 */
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_CTRL0_REG, RTK_MDC_MDIO_ADDR_OP );
		/* Write address to register 23 */
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_ADDRESS_REG, u16Addr );
		/* Write data to register 24 */
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_DATA_WRITE_REG, u16Data );
		/* Write data control code to register 21 */
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_START_REG, RTK_MDC_MDIO_START_OP);
		ifx_ltq_write_mdio(RTK_MDC_MDIO_DUMMY_ID, RTK_MDC_MDIO_CTRL1_REG, RTK_MDC_MDIO_WRITE_OP );
#endif
}

