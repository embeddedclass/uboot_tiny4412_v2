#ifndef _EXYNOS_CPU_H
#define _EXYNOS_CPU_H

/* RTC (see manual chapter 17) */
struct exynos_rtc {
	u8	res0[0x30];
	u32	intp;
	u8	res1[0x9];
	u32	rtccon;
	u32	ticnt;
	u32	res3[0x02];
	u32	rtcalm;
	u32	almsec;
	u32	almmin;
	u32	almhour;
	u32	almdate;
	u32	almmon;
	u32	almyear;
	u8	res4[4];
	u32	bcdsec;
	u32	bcdmin;
	u32	bcdhour;
	u32	bcddayweek;
	u32	bcdday;
	u32	bcdmon;
	u32	bcdyear;
	u8	res5[4];
	u32 curticcnt 
};

struct exynos_adc {
	u32 adccon;
	u32 res[1];
	u32 adcdly;
	u32 adcdat;
	u32 clrintadc;
	u32 adcmux;
};

typedef enum {
 ADC_0 = 0,
 ADC_1,
 ADC_2,
 ADC_3,
 ADC_CHANNEL_MAX,
} ADC_CHANNEL;

static inline struct exynos_rtc *exynos_get_base_rtc(void)
{
	return (struct exynos_rtc *)EXYNOS_RTC_BASE;
}

static inline struct exynos_adc *exynos_get_base_adc(void)
{
	return (struct exynos_adc *)EXYNOS_ADC_BASE;
}

#endif
