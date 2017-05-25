/*
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, d.mueller@elsoft.ch
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>

#include <asm/arch/cpu.h>
#include <asm/io.h>
#include <asm/arch/cpu_exynos.h>

#define ADC_RATE_5M 5000000
/*
 * ADC_CFG[16] in System Register
 * (Base address : 0x1001_0118)
 * 0 : General ADC
 * 1 : MTCADC_ISP
 */


unsigned char get_adc_prscvl (ulong adc_clk, ulong aclk)
{
	ulong adc_clk_tmp;
	unsigned char prscvl = 0;

	while (prscvl < 0xFF) {
		adc_clk_tmp = aclk / (prscvl + 1);
		if (adc_clk_tmp <= adc_clk)
			return prscvl;
		prscvl++;
	}
}

/* 10bit:0, 12 bit:1 */
#define ADC_12_BIT 1
#define ADC_END_CFLG 1



void init_adc()
{
	struct exynos_adc *adc = exynos_get_base_adc();
	unsigned char adc_prscvl = 0;
	ulong aclk;

	//stop adc 
	writel ((readl(&adc->adccon) & ~(1 << 0)), &adc->adccon);

	//into normal mode
	writel ((readl(&adc->adccon) & ~(1 << 2)), &adc->adccon);

	//set 10 bit
	writel ((readl(&adc->adccon) & ~(1 << 16)), &adc->adccon);

	/* Set ADC frequency is 5MHz */
	aclk = get_ACLK_CLK();

	adc_prscvl = get_adc_prscvl (ADC_RATE_5M, aclk);
	writel ((readl(&adc->adccon) | (adc_prscvl << 6)), &adc->adccon);
	writel ((readl(&adc->adccon) | (1 << 14)), &adc->adccon);
}

void start_adc(unsigned char adc_ch)
{
	struct exynos_adc *adc = exynos_get_base_adc();

	//set analog channel
	writel (adc_ch, &adc->adcmux);

	//start adc 
	writel ((readl(&adc->adccon) | (1 << 0)), &adc->adccon);
}

unsigned int read_adc()
{
	struct exynos_adc *adc = exynos_get_base_adc();
	unsigned int adc_end;
	int delay_tmie = 20;

	while (delay_tmie > 0) {
		adc_end = readl(&adc->adccon) & (1 << 15);
		if (adc_end) {
			/* 10 bit */
			return readl (&adc->adcdat) & 0x03FF;
		}
		delay_tmie--;
		udelay(10);
	};

	return 0;
}

void standy_mode_adc()
{
	struct exynos_adc *adc = exynos_get_base_adc();

	//into standy mode
	writel ((readl(&adc->adccon) | (1 << 2)), &adc->adccon);
}
