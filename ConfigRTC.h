// $Id: ConfigRTC.h,v 1.2 2025/04/04 17:08:27 administrateur Exp $

#ifndef __CONFIG_RTC__
#define __CONFIG_RTC__

typedef enum {
	CONFIG_RTC_NONE = 0,
	CONFIG_RTC_IN_PROGRESS_YEAR,
	CONFIG_RTC_IN_PROGRESS_MONTH,
	CONFIG_RTC_IN_PROGRESS_MDAY,
	CONFIG_RTC_IN_PROGRESS_HOUR,
	CONFIG_RTC_IN_PROGRESS_MIN,
	CONFIG_RTC_WAIT_OF_ACQ,
	CONFIG_RTC_DONE
} ENUM_CONFIG_RTC;

class ConfigRTC {
	private:
		ENUM_CONFIG_RTC   m__config_rtc;
    struct tm         m__timeinfo_in_acquisition;
    bool              m__flg_presentation;

    const char *getDayInWeek() const;
    void nextState();
    void presentation();

	public:
		ConfigRTC();
		~ConfigRTC();

    void click_button();
    bool isInProgress() const;
    bool isDone() const { return m__config_rtc == CONFIG_RTC_DONE; };
    void setDone();
    void nextValue();
};

extern ConfigRTC		*g__config_rtc;
#endif
