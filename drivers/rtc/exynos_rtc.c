/*
 * (C) Copyright 2003
 * David Müller ELSOFT AG Switzerland. d.mueller@elsoft.ch
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

/*
 * Date & Time support for the built-in Samsung S3C24X0 RTC
 */

#include <common.h>
#include <command.h>

#if (defined(CONFIG_CMD_DATE))

#include <asm/arch/cpu.h>
#include <asm/arch/cpu_exynos.h>


#include <rtc.h>
#include <asm/io.h>

/*#define	DEBUG*/

typedef enum {
	RTC_ENABLE,
	RTC_DISABLE
} RTC_ACCESS;


static inline void SetRTC_Access(RTC_ACCESS a)
{
	struct exynos_rtc *rtc = exynos_get_base_rtc();

	switch (a) {
	case RTC_ENABLE:
		writeb(readb(&rtc->rtccon) | 0x01, &rtc->rtccon);
		break;

	case RTC_DISABLE:
		writeb(readb(&rtc->rtccon) & ~0x01, &rtc->rtccon);
		break;
	}
}

/* ------------------------------------------------------------------------- */

int rtc_get(struct rtc_time *tmp)
{
	struct exynos_rtc *rtc = exynos_get_base_rtc();
	uchar sec, min, hour, mday, wday, mon, year;
	uchar a_sec, a_min, a_hour, a_date, a_mon, a_year, a_armed;

	/* enable access to RTC registers */
	SetRTC_Access(RTC_ENABLE);

	/* read RTC registers */
	do {
		sec  = readb(&rtc->bcdsec);
		min  = readb(&rtc->bcdmin);
		hour = readb(&rtc->bcdhour);
		mday = readb(&rtc->bcdday);
		wday = readb(&rtc->bcddayweek);
		mon  = readb(&rtc->bcdmon);
		year = readb(&rtc->bcdyear);
	} while (sec != readb(&rtc->bcdsec));

	/* read ALARM registers */
	a_sec   = readb(&rtc->almsec);
	a_min   = readb(&rtc->almmin);
	a_hour  = readb(&rtc->almhour);
	a_date  = readb(&rtc->almdate);
	a_mon   = readb(&rtc->almmon);
	a_year  = readb(&rtc->almyear);
	a_armed = readb(&rtc->rtcalm);

	/* disable access to RTC registers */
	SetRTC_Access(RTC_DISABLE);

#ifdef RTC_DEBUG
	printf("Get RTC year: %02x mon/cent: %02x mday: %02x wday: %02x "
	       "hr: %02x min: %02x sec: %02x\n",
	       year, mon, mday, wday, hour, min, sec);
	printf("Alarms: %02x: year: %02x month: %02x date: %02x hour: "
	       "%02x min: %02x sec: %02x\n",
	       a_armed, a_year, a_mon, a_date, a_hour, a_min, a_sec);
#endif

	tmp->tm_sec  = bcd2bin(sec & 0x7F);
	tmp->tm_min  = bcd2bin(min & 0x7F);
	tmp->tm_hour = bcd2bin(hour & 0x3F);
	tmp->tm_mday = bcd2bin(mday & 0x3F);
	tmp->tm_mon  = bcd2bin(mon & 0x1F);
	tmp->tm_year = bcd2bin(year);
	tmp->tm_wday = bcd2bin(wday & 0x07);
	if (tmp->tm_year < 70)
		tmp->tm_year += 2000;
	else
		tmp->tm_year += 1900;
	tmp->tm_yday  = 0;
	tmp->tm_isdst = 0;
#ifdef RTC_DEBUG
	printf("Get DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
	       tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
	       tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
#endif

	return 0;
}

int rtc_set(struct rtc_time *tmp)
{
	struct exynos_rtc *rtc = exynos_get_base_rtc();
	uchar sec, min, hour, mday, wday, mon, year;

#ifdef RTC_DEBUG
	printf("Set DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
	       tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
	       tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
#endif
	year = bin2bcd(tmp->tm_year % 100);
	mon  = bin2bcd(tmp->tm_mon);
	wday = bin2bcd(tmp->tm_wday);
	mday = bin2bcd(tmp->tm_mday);
	hour = bin2bcd(tmp->tm_hour);
	min  = bin2bcd(tmp->tm_min);
	sec  = bin2bcd(tmp->tm_sec);

	/* enable access to RTC registers */
	SetRTC_Access(RTC_ENABLE);

	/* write RTC registers */
	writeb(sec, &rtc->bcdsec);
	writeb(min, &rtc->bcdmin);
	writeb(hour, &rtc->bcdhour);
	writeb(mday, &rtc->bcdday);
	writeb(wday, &rtc->bcddayweek);
	writeb(mon, &rtc->bcdmon);
	writeb(year, &rtc->bcdyear);

	/* disable access to RTC registers */
	SetRTC_Access(RTC_DISABLE);

	return 0;
}

void rtc_reset(void)
{
	struct exynos_rtc *rtc = exynos_get_base_rtc();

//	printf ("rtc 0x%x\n", rtc);
//	printf ("rtc->intp 0x%x\n", &rtc->intp);
//	printf ("rtc->rtccon 0x%x\n", &rtc->rtccon);
//	printf ("rtc->ticnt 0x%x\n", &rtc->ticnt);
//	printf ("rtc->rtcalm 0x%x\n", &rtc->rtcalm);
//	printf ("rtc->almsec 0x%x\n", &rtc->almsec);
//	printf ("rtc->almmin 0x%x\n", &rtc->almmin);
//	printf ("rtc->almhour 0x%x\n", &rtc->almhour);
//	printf ("rtc->almdate 0x%x\n", &rtc->almdate);
//	printf ("rtc->almmon 0x%x\n", &rtc->almmon);
//	printf ("rtc->almyear 0x%x\n", &rtc->almyear);
//	printf ("rtc->bcdsec 0x%x\n", &rtc->bcdsec);
//	printf ("rtc->bcdmin 0x%x\n", &rtc->bcdmin);
//	printf ("rtc->bcdhour 0x%x\n", &rtc->bcdhour);
//	printf ("rtc->bcddayweek 0x%x\n", &rtc->bcddayweek);
//	printf ("rtc->bcdday 0x%x\n", &rtc->bcdday);
//	printf ("rtc->bcdmon 0x%x\n", &rtc->bcdmon);
//	printf ("rtc->bcdyear 0x%x\n", &rtc->bcdyear);
//	printf ("rtc->curticcnt 0x%x\n", &rtc->curticcnt);

	writeb(readb(&rtc->rtccon) | 0x08, &rtc->rtccon);
	writeb(readb(&rtc->rtccon) & ~(0x08 | 0x01), &rtc->rtccon);
}


#endif
