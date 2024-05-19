/*
 * s3c2440 SPI driver
 *
 * Copyright (C) 2024 madeinchaos <https://github.com/madeinchaos>
 *
 */
#include <common.h>
#include <errno.h>
#include <spi.h>
#include <asm/io.h>
#include <asm/arch/s3c2440.h>
#include <s3c2440_spi.h>

static struct s3c24x0_spi_channel *s3c2440_get_spi_chl(u8 chl)
{
	if (chl > S3C2440_SPI_CHL1) {
		return NULL;
	}

	struct s3c24x0_spi * const spis = s3c24x0_get_base_spi();
	return &(spis->ch[chl]);
}

u32 s3c2440_spi_get_dataddr(u8 chl)
{
	struct s3c24x0_spi_channel *spi = s3c2440_get_spi_chl(chl);

	if (spi) {
		return (u32)(&(spi->sptdat));
	}
	return 0;
}

void s3c2440_spi_set_baudrate(u8 chl, int baudrate)
{
	struct s3c24x0_spi_channel *spi = s3c2440_get_spi_chl(chl);
	u8 *addr = &(spi->sppre);

	if (spi) {
		ulong pclk = get_PCLK();
		/* Baud rate = PCLK / 2 / (Prescaler value + 1) */
		writel((pclk / 2 / baudrate) - 1, addr);
	}
}

void s3c2440_spi_set_control(u8 chl, u32 val)
{
	struct s3c24x0_spi_channel *spi = s3c2440_get_spi_chl(chl);
	u8 *addr = &(spi->spcon);

	if (spi) {
		writel(0, addr);
		writel(val, addr);
	}
}

void s3c2440_spi_write(u8 chl, u8 data)
{
	struct s3c24x0_spi_channel *spi = s3c2440_get_spi_chl(chl);
	u8 *addr = &(spi->spsta);

	if (spi) {
		while (!(readl(addr) & 0x01));
		addr = &(spi->sptdat);
		writel(data, addr);
		addr = &(spi->spsta);
		while (!(readl(addr) & 0x01));
	}
}

u8 s3c2440_spi_write_read(u8 chl, u8 data)
{
	struct s3c24x0_spi_channel *spi = s3c2440_get_spi_chl(chl);
	u8 *addr = &(spi->spsta);

	if (spi) {
		while (!(readl(addr) & 0x01));
		addr = &(spi->sptdat);
		writel(data, addr);
		addr = &(spi->spsta);
		while (!(readl(addr) & 0x01));
		addr = &(spi->sptdat);
		return readl(addr);
	}

	return 0;
}

void s3c2440_spi_gpio_init(u8 chl)
{
	if (chl == S3C2440_SPI_CHL0) {
		u32 val = 0;
		struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
		val = readl(&gpio->gpecon);
		val &= ~((3<<26)|(3<<24)|(3<<22));
		val |= ((2<<26)|(2<<24)|(2<<22));
		writel(val, &gpio->gpecon);
	}
}

