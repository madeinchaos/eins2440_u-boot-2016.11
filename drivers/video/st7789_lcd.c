/*
 * ST7789 LCD Controller
 *
 * Copyright (C) 2024 madeinchaos <https://github.com/madeinchaos>
 *
 */
#include <common.h>
#include <asm/io.h>
#include <lcd.h>
#include <linux/types.h>
#include <stdarg.h>
#include <stdio_dev.h>
#include <asm/arch/s3c2440.h>
#include <s3c2440_spi.h>
#include <s3c2440_dma.h>

/* #define DEBUG */

#ifdef CONFIG_LCD

#define GPIO_INPUT  0x0
#define GPIO_OUTPUT 0x1
#define ST7789_RES_GPG(x)  ((x)<<10)
#define ST7789_DC_GPG(x)  ((x)<<12)

#define LCD_COL		240
#define LCD_ROW		240
#define SPI_CHL			S3C2440_SPI_CHL0
#define SPI_BAUDRATE	(25000000)
#define DMA_CHL		S3C2440_DMA_CHL1

vidinfo_t panel_info = {
	.vl_col		= LCD_COL,
	.vl_row		= LCD_ROW,
	.vl_bpix	= LCD_BPP,
};

/*
 * tlv format: cmd, data len, data
*/
static const u8 st7789_init_data[] = {
	0x11, 0x00,
	0x36, 0x01, 0x00,
	0x3A, 0x01, 0x05,
	0xB2, 0x05, 0x0C, 0x0C, 0x00, 0x33, 0x33,
	0xB7, 0x01, 0x35,
	0xBB, 0x01, 0x19,
	0xC0, 0x01, 0x2C,
	0xC2, 0x01, 0x01,
	0xC3, 0x01, 0x12,
	0xC4, 0x01, 0x20,
	0xC6, 0x01, 0x0F,
	0xD0, 0x02, 0xA4, 0xA1,
	0xE0, 0x0E, 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23,
	0xE1, 0x0E, 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23,
	0x21, 0x00,
	0x11, 0x00,
	0x29, 0x00,
	/* init region */
	0x2a, 0x04,  0x00, 0x00, 0x00, LCD_COL - 1,
	0x2b, 0x04,  0x00, 0x00, 0x00, LCD_ROW - 1,
	0x2c, 0x00,
};

static void st7789_spi_init(void)
{
	s3c2440_spi_gpio_init(SPI_CHL);

	u32 val = 0;
	val |= S3C2440_SPI_SPCON_SMOD(2);
	val |= S3C2440_SPI_SPCON_ENSCK(1);
	val |= S3C2440_SPI_SPCON_MSTR(1);
	val |= S3C2440_SPI_SPCON_CPOL(1);
	val |= S3C2440_SPI_SPCON_CPHA(1);
	s3c2440_spi_set_control(SPI_CHL, val);

	s3c2440_spi_set_baudrate(SPI_CHL, SPI_BAUDRATE);
}

static void st7789_lcd_dc_cmd(void)
{
	u32 val = 0;
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
	val = readl(&gpio->gpgdat);
	val &= ~(0x1 << 6);
	writel(val, &gpio->gpgdat);
}

static void st7789_lcd_dc_data(void)
{
	u32 val = 0;
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
	val = readl(&gpio->gpgdat);
	val |= 0x1 << 6;
	writel(val, &gpio->gpgdat);
}

static void st7789_lcd_reset(void)
{
	u32 val = 0;

	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();

	val = readl(&gpio->gpgdat);
	val &= ~(0x1 << 5);
	writel(val, &gpio->gpgdat);
	mdelay(100);
	val = readl(&gpio->gpgdat);
	val |= 0x1 << 5;
	writel(val, &gpio->gpgdat);
	mdelay(100);
}

static void st7789_lcd_write_cmd(u8 cmd)
{
	st7789_lcd_dc_cmd();
	s3c2440_spi_write(SPI_CHL, cmd);
}

static void st7789_lcd_write_data(u8 data)
{
	st7789_lcd_dc_data();
	s3c2440_spi_write(SPI_CHL, data);
}

void st7789_lcd_write_color(u16 color)
{
	st7789_lcd_write_data(color >> 8);
	st7789_lcd_write_data(color);
}

static void st7789_lcd_init(void)
{
	int i, j, data_len;

	st7789_lcd_reset();

	for (i = 0; i < sizeof(st7789_init_data); i = i + data_len + 1) {
		st7789_lcd_write_cmd(st7789_init_data[i]);

		i++; /* shift to data len */
		data_len = st7789_init_data[i];

		for (j = 0; j < data_len; j++) {
			st7789_lcd_write_data(st7789_init_data[i + j + 1]);
		}
	}
}

static void st7789_dma_init(void *lcdbase)
{
	u32 val;

	val = 0;
	s3c2440_dma_set_src(DMA_CHL, (u32)lcdbase, val);

	val = 0;
	val |= S3C2440_DMA_DIDSTC_LOC(1);
	val |= S3C2440_DMA_DIDSTC_INC(1);
	s3c2440_dma_set_dst(DMA_CHL, s3c2440_spi_get_dataddr(SPI_CHL), val);

	val = 0;
	val |= S3C2440_DMA_DCON_DMD_HS(1);
	val |= S3C2440_DMA_DCON_HWSRCSEL(3);
	val |= S3C2440_DMA_DCON_SWHW_SEL(1);
	val |= S3C2440_DMA_DCON_TC(115200);
	s3c2440_dma_set_control(DMA_CHL, val);
	s3c2440_dma_trigger(DMA_CHL);
}

static void st7789_lcd_set_gpio(void)
{
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
	u32 val = readl(&gpio->gpgcon);
	val &= ~(ST7789_RES_GPG(3) | ST7789_DC_GPG(3));
	val |= ST7789_RES_GPG(GPIO_OUTPUT);
	val |= ST7789_DC_GPG(GPIO_OUTPUT);
	writel(val, &gpio->gpgcon);
	st7789_spi_init();
}

void lcd_ctrl_init (void *lcdbase)
{
	st7789_lcd_set_gpio();
	st7789_lcd_init();
	st7789_lcd_dc_data();
	st7789_dma_init(lcdbase);
}

__weak void lcd_enable(void)
{

}

#endif /* CONFIG_LCD */
