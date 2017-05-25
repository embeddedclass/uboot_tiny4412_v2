/*
 * (C) Copyright 2011 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
 
#include <common.h>
#include <asm/arch/cpu.h>

unsigned long (*get_uart_clk)(int dev_index);
unsigned long (*get_pwm_clk)(void);
unsigned long (*get_arm_clk)(void);
unsigned long (*get_pll_clk)(int);

void s5p_clock_init(void)
{
}

#define APLL 0
#define MPLL 1
#define EPLL 2
#define VPLL 3

#define ACLK_133 133000000

/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * CONFIG_SYS_CLK_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */

static ulong get_PLLCLK(int pllreg)
{
	ulong r, m, p, s;

	if (pllreg == APLL) {
		r = APLL_CON0_REG;
		m = (r>>16) & 0x3ff;
	} else if (pllreg == MPLL) {
		r = MPLL_CON0_REG;
		m = (r>>16) & 0x3ff;
	} else
		hang();

	p = (r>>8) & 0x3f;
	s = r & 0x7;
#if !(defined(CONFIG_SMDKC220) || defined(CONFIG_ARCH_EXYNOS5))
	if ((pllreg == APLL) || (pllreg == MPLL)) 
		s= s-1;
#endif

	return (m * (CONFIG_SYS_CLK_FREQ / (p * (1 << s))));
}

ulong get_APLL_CLK(void)
{
	return (get_PLLCLK(APLL));
}

ulong get_MPLL_CLK(void)
{
	return (get_PLLCLK(MPLL));
}

ulong get_ACLK_CLK(void)
{
	unsigned int *clk_div_top;
	unsigned int clk_div_top_val;

	clk_div_top = (unsigned int *)(ELFIN_CLOCK_BASE + CLK_DIV_TOP_OFFSET);
	clk_div_top_val = *clk_div_top;
	return (ACLK_133 / ((*clk_div_top & (0x07 << 12) >> 12) + 1));
}

ulong get_LEFTBUS_CLK (int clk_type)
{
	ulong mux_mpll_user, mux_gdl_sel;
	ulong clk_gdl, clk_gpl, mpll_clk, apll_clk;
	unsigned int *clk_left_bus_div;
	unsigned int gpl_div, gdl_div, read_left_bus_div;

	clk_left_bus_div = (unsigned int *)(ELFIN_CLOCK_BASE + CLK_DIV_LEFTBUS_OFFSET);

	mux_mpll_user = (LEFTBUS_CON0_REG & (0x01 << 4)) >> 4;
	if (!mux_mpll_user) {
		printf ("FIN_PLL : %lu\n", CONFIG_SYS_CLK_FREQ);
		return;
	} else {
		printf ("FOUT_MPLL : %lu\n", get_APLL_CLK());
	}

	mux_gdl_sel = LEFTBUS_CON0_REG & 0x01;
	if (!mux_gdl_sel) {
		printf ("SCLK_MPLL\n");
	} else {
		printf ("SCLK_APLL\n");
	}

	read_left_bus_div = *clk_left_bus_div;
	gpl_div = (read_left_bus_div & (0x07 << 4)) >> 4;
	gdl_div = read_left_bus_div & 0x07;

	printf ("gpl_div %d\n", gpl_div);
	printf ("gdl_div %d\n", gdl_div);

	if (mux_gdl_sel == 1) {
		clk_gdl = get_APLL_CLK() / gdl_div;
		clk_gpl = (get_APLL_CLK() / gdl_div) / gpl_div;
	} else {
		if (mux_mpll_user == 1) {
			clk_gdl = get_MPLL_CLK() / gdl_div;
			clk_gpl = (get_MPLL_CLK() / gdl_div) / gpl_div;
		} else {
			clk_gdl = CONFIG_SYS_CLK_FREQ / gdl_div;
			clk_gpl = (CONFIG_SYS_CLK_FREQ / gdl_div) / gpl_div;
		}
	}

	if (clk_type == ACLK_GDL_TYPE)
		return clk_gdl;
	else
		return clk_gpl;

}

