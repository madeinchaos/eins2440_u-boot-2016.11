/*
 * s3c2440 SPI Controller
 */
#ifndef __S3C2440_SPI_H__
#define __S3C2440_SPI_H__

/* 
 * Determine how SPTDAT is read/written
 * 00 = polling mode 01 = interrupt mode
 * 10 = DMA mode 11 = reserved
*/
#define S3C2440_SPI_SPCON_SMOD(x)	((x)<<5)

/* 
 * Determine whether you want SCK enabled or not (master only).
 * 0 = disable 1 = enable
*/
#define S3C2440_SPI_SPCON_ENSCK(x)	((x)<<4)

/* 
 * Determine the desired mode (master or slave).
 * 0 = slave 1 = master
*/
#define S3C2440_SPI_SPCON_MSTR(x)	((x)<<3)

/* 
 * Determine an active high or active low clock.
 * 0 = active high 1 = active low
*/
#define S3C2440_SPI_SPCON_CPOL(x)	((x)<<2)

/* 
 * Select one of the two fundamentally different transfer format
 * 0 = format A 1 = format B
*/
#define S3C2440_SPI_SPCON_CPHA(x)	((x)<<1)

/* 
 * Decide whether the receiving data is required or not.
 * 0 = normal mode 1 = Tx auto garbage data mode
*/
#define S3C2440_SPI_SPCON_TAGD(x)	((x)<<0)

#define S3C2440_SPI_CHL0		(0)
#define S3C2440_SPI_CHL1		(1)

u32 s3c2440_spi_get_dataddr(u8 chl);
void s3c2440_spi_set_baudrate(u8 chl, int baudrate);
void s3c2440_spi_set_control(u8 chl, u32 val);
void s3c2440_spi_write(u8 chl, u8 data);
u8 s3c2440_spi_write_read(u8 chl, u8 data);
void s3c2440_spi_gpio_init(u8 chl);

#endif /*__S3C2440_SPI_H__*/

