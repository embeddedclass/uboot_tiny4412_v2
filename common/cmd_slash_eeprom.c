/*
 * Copy from cmd_slash_led.c by Slash
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

int do_eeprom_sample(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i = 0;
	uint addr[1] = {0x00};
	unsigned char wbuffer[1] = {0x23};
	unsigned char rbuffer[1] = {0xFF};

	//i2c_probe(0);

	printf ("%s\n", argv[0]);
	printf ("%s\n", argv[1]);

	if (argc > 2) {
		wbuffer[0] = simple_strtoul (argv[2], NULL, 16);
		printf ("wbuffer[0] 0x%x\n", wbuffer[0]);
	}

	if (strcmp (argv[1], "w") == 0) {
		printf ("%s - write\n", __func__);
		i2c_write(0xA0 >> 1, addr, 1, wbuffer, 1);
	} else {
		printf ("%s - read\n", __func__);
		i2c_read(0xA0 >> 1, addr, 1, rbuffer, 1);
		printf ("rbuffer[0] 0x%x\n", rbuffer[0]);
	}

	return 0;
}

U_BOOT_CMD(
	eeprom_sample,	3,		1,	do_eeprom_sample,
	"eeprom_sample",
	""
);
