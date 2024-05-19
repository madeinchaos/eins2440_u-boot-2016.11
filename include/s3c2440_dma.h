/*
 * s3c2440 DMA Controller
 */
#ifndef __S3C2440_DMA_H__
#define __S3C2440_DMA_H__

#define S3C2440_DMA_CHL0		(0)
#define S3C2440_DMA_CHL1		(1)
#define S3C2440_DMA_CHL2		(2)
#define S3C2440_DMA_CHL3		(3)

/* 
 Select one between Demand mode and Handshake mode.
 0: Demand mode will be selected.
 1: Handshake mode will be selected.
*/
#define S3C2440_DMA_DCON_DMD_HS(x)  ((x)<<31)

/* 
 * Select DMA request source for each DMA.
 * DCON0: 000:nXDREQ0 001:UART0 010:SDI 011:Timer
 * 100:USB device EP1
 * DCON1: 000:nXDREQ1 001:UART1 010:I2SSDI 011:SPI
 * 100:USB device EP2
 * DCON2: 000:I2SSDO 001:I2SSDI 010:SDI 011:Timer
 * 100:USB device EP3
 * DCON3: 000:UART2 001:SDI 010:SPI 011:Timer
 * 100:USB device EP4
 * DCON0: 101:I2SSDO 110:PCMIN
 * DCON1: 101:PCMOUT 110:SDI
 * DCON2: 101:PCMIN 110:MICIN
 * DCON3: 101:MICIN 110:PCMOUT
*/
#define S3C2440_DMA_DCON_HWSRCSEL(x)  ((x)<<24)

/* 
 * Select the DMA source between software (S/W request mode) and
 * hardware (H/W request mode).
 * 0: S/W request mode is selected and DMA is triggered by setting
 * SW_TRIG bit of DMASKTRIG control register.
 * 1: DMA source selected by bit[26:24] triggers the DMA operation.
*/
#define S3C2440_DMA_DCON_SWHW_SEL(x)  ((x)<<23)

/* 
 * Initial transfer count (or transfer beat).
 * Note that the actual number of bytes that are transferred is
 * computed by the following equation: DSZ x TSZ x TC. Where, DSZ,
 * TSZ (1 or 4), and TC represent data size DCONn[21:20], transfer
 * size DCONn[28], and initial transfer count, respectively. This value
 * will be loaded into CURR_TC only if the CURR_TC is 0 and the
 * DMA ACK is 1.
*/
#define S3C2440_DMA_DCON_TC(x)  ((x)<<0)

/* 
 * Determine how SPTDAT is read/written
 * 00 = polling mode 01 = interrupt mode
 * 10 = DMA mode 11 = reserved
*/
#define S3C2440_DMA_DISRCC_LOC(x)  ((x)<<1)
#define S3C2440_DMA_DIDSTC_LOC(x)  ((x)<<1)

/* 
 * Determine how SPTDAT is read/written
 * 00 = polling mode 01 = interrupt mode
 * 10 = DMA mode 11 = reserved
*/
#define S3C2440_DMA_DISRCC_INC(x)  ((x)<<0)
#define S3C2440_DMA_DIDSTC_INC(x)  ((x)<<0)

void s3c2440_dma_set_src(u8 chl, u32 disrc, u32 disrcc);
void s3c2440_dma_set_dst(u8 chl, u32 didst, u32 didstc);
void s3c2440_dma_set_control(u8 chl, u32 val);
void s3c2440_dma_trigger(u8 chl);

#endif /*__S3C2440_DMA_H__*/

