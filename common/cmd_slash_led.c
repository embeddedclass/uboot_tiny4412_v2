/*
 * Copy from cmd_version.c by Slash
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
#include <asm/arch/gpio.h>

/*
 * Exynos 4412 SCP_Users Manual_Ver.0.10.00_Preliminary0.pdf
 * 6.2.3.113 - 6.2.3.113 
 */
#define GPIO_BASE 0x11000000
#define GPIO_M4_CON (GPIO_BASE + 0x02E0)
#define GPIO_M4_DAT (GPIO_BASE + 0x02E4)
#define GPIO_M4_PULL (GPIO_BASE + 0x02E8)
#define GPIO_M4_DRV (GPIO_BASE + 0x02EC)
#define GPIO_M4_PDN_CON (GPIO_BASE + 0x02F0)
#define GPIO_M4_PDN_PULL (GPIO_BASE + 0x02F4)

int do_led_sample(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i, j;
	struct s5p_gpio_bank gpio_bank;
	unsigned char value, 	tmp = 0x0f;

	unsigned char led_patten[] = {
			0x0f, //0000_1111
			0x0e, //0000_1110
			0x0d, //0000_1101
			0x0b, //0000_1011
			0x07, //0000_0111
			0x00, //0000_0000
			0x05, //0000_0101
			0x0a, //0000_1010
		};

	printf("%s\n", __func__);

	gpio_bank.con = GPIO_M4_CON;
	gpio_bank.dat = GPIO_M4_DAT;
	gpio_bank.pull = GPIO_M4_PULL;
	gpio_bank.drv = GPIO_M4_DRV;
	gpio_bank.pdn_con = GPIO_M4_PDN_CON;
	gpio_bank.pdn_pull = GPIO_M4_PDN_PULL;


	/* configure gpio as output */
	gpio_cfg_pin(&gpio_bank, 0, 1);
	gpio_cfg_pin(&gpio_bank, 1, 1);
	gpio_cfg_pin(&gpio_bank, 2, 1);
	gpio_cfg_pin(&gpio_bank, 3, 1);

	/* disabled GPM4PUD */
	gpio_set_pull(&gpio_bank, 0, GPIO_PULL_NONE);
	gpio_set_pull(&gpio_bank, 1, GPIO_PULL_NONE);
	gpio_set_pull(&gpio_bank, 2, GPIO_PULL_NONE);
	gpio_set_pull(&gpio_bank, 3, GPIO_PULL_NONE);

	for (j = 0; j < 2; j++) {
		tmp = 0x0f;
		/* led_0 ~ led_3 off */
		gpio_set_value(&gpio_bank, 0, 1);
		gpio_set_value(&gpio_bank, 1, 1);
		gpio_set_value(&gpio_bank, 2, 1);
		gpio_set_value(&gpio_bank, 3, 1);

		for (i = 0; i < 8; i++) {
			tmp = tmp < i;
			set_led(&gpio_bank, led_patten[i]);
			udelay(1000000);
		}
	}
	
	return 0;
}

void set_led(struct s5p_gpio_bank *gpio_bank, unsigned char v)
{
	gpio_set_value(gpio_bank, 0, !!(v & 0x01));
	gpio_set_value(gpio_bank, 1, !!(v & 0x02));
	gpio_set_value(gpio_bank, 2, !!(v & 0x04));
	gpio_set_value(gpio_bank, 3, !!(v & 0x08));
}

U_BOOT_CMD(
	led_sample,	1,		1,	do_led_sample,
	"print monitor version",
	""
);
