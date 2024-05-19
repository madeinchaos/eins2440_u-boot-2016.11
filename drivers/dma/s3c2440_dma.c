/*
 * S3C2440 DMA controller interface support functions
 *
 * Copyright (C) 2024 madeinchaos <https://github.com/madeinchaos>
 *
 */

#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/s3c2440.h>
#include <s3c2440_dma.h>

/* 
 * DMA channel on/off bit.
 * 0: DMA channel is turned off.
 * (DMA request to this channel is ignored.)
 * 1: DMA channel is turned on and the DMA request is handled. This
 * bit is automatically set to off if we set the DCONn[22] bit to “no
 * auto reload” and/or STOP bit of DMASKTRIGn to “stop”.
 * Note that when DCON[22] bit is "no auto reload", this bit becomes
 * 0 when CURR_TC reaches 0. If the STOP bit is 1, this bit becomes
 * 0 as soon as the current atomic transfer is completed.
*/
#define S3C2440_DMA_DMASKTRIG_ON_OFF(x)  ((x)<<1)

/* 
 * Trigger the DMA channel in S/W request mode.
 * 1: it requests a DMA operation to this controller.
 * Note that this trigger gets effective after S/W request mode has to
 * be selected (DCONn[23]) and channel ON_OFF bit has to be set to
 * 1 (channel on). When DMA operation starts, this bit is cleared
 * automatically.
*/
#define S3C2440_DMA_DMASKTRIG_SW_TRIG(x)  ((x)<<0)

struct s3c24x0_dma *s3c2440_get_dma(u8 chl)
{
	if (chl > S3C2440_DMA_CHL3) {
		return NULL;
	}

	struct s3c24x0_dmas * const dmas = s3c24x0_get_base_dmas();
	return &(dmas->dma[chl]);
}

void s3c2440_dma_set_src(u8 chl, u32 disrc, u32 disrcc)
{
	struct s3c24x0_dma *dma = s3c2440_get_dma(chl);

	if (dma) {
		writel(disrc, &dma->disrc);
		writel(disrcc, &dma->disrcc);
	}
}

void s3c2440_dma_set_dst(u8 chl, u32 didst, u32 didstc)
{
	struct s3c24x0_dma *dma = s3c2440_get_dma(chl);

	if (dma) {
		writel(didst, &dma->didst);
		writel(didstc, &dma->didstc);
	}
}

void s3c2440_dma_set_control(u8 chl, u32 val)
{
	struct s3c24x0_dma *dma = s3c2440_get_dma(chl);

	if (dma) {
		writel(val, &dma->dcon);
	}
}

void s3c2440_dma_trigger(u8 chl)
{
	struct s3c24x0_dma *dma = s3c2440_get_dma(chl);
	u32 val = 0;

	if (dma) {
		val |= S3C2440_DMA_DMASKTRIG_ON_OFF(1);
		val |= S3C2440_DMA_DMASKTRIG_SW_TRIG(1);
		writel(val, &dma->dmasktrig);
	}
}

