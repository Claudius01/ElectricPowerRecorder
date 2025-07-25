// $Id: DateTime.h,v 1.9 2025/07/13 14:14:03 administrateur Exp $

#ifndef __DATE_TIME__
#define __DATE_TIME__

#include <sstream>

#if !USE_SIMULATION
#include <ESP32Time.h>
#else
#include "ESP32TimeSimu.h"
#endif

// Années bissextiles
#define LEAP_YEAR(year) ((year % 4) == 0)

#define BEGIN_OF_OFF_PEAK_HOURS     (0 * 3600L)   // Debut des Heures Creuses (00h00)
#define END_OF_OFF_PEAK_HOURS       (8 * 3600L)   // Fin   des Heures Creuses (08h00)

typedef enum {
	GMT,
	LOCALTIME
} DEF_GMT_LOCAL;

typedef enum {
  NO_SOMMER_WINTER = 0,
  SOMMER,
  WINTER
} ENUM_SOMMER_WINTER;

typedef struct {
  int tm_sec;    /* Seconds. [0-60] (1 leap second) */
  int tm_min;    /* Minutes. [0-59] */
  int tm_hour;   /* Hours. [0-23] */
  int tm_mday;   /* Day.   [1-31] */
  int tm_mon;    /* Month. [0-11] */
  int tm_year;   /* Year - 1900.  */
} ST_TM;

typedef struct {
  char                  seconds;              // Seconde [0..59]
  char                  minutes;
  char                  hours;
  char                  day;
  char                  month;
  char                  year;                 // Year same 20XX
  char                  day_of_week;
  byte                  nbr_days_before;      // Nombre du même jour avant et après dans le mois
  byte                  nbr_days_after;       // (nbr_days_before + 1 + nbr_days_after) = Nbr total du même jour
  ENUM_SOMMER_WINTER    sommer_winter;
  long                  epoch;
} ST_DATE_AND_TIME;

typedef struct {
  time_t                epoch;
  char                  t_date_time[132];       // Date/Time formated (ie. [2025/01/31 18:08:14 (#5/#5 Vendredi) (Heure d'hiver)])
  char                  t_date_time_lcd[32];    // Date/Time formated (ie. [Ven. 31/01/25 18:08])
} ST_UNIX_TIME;

/* Définitions pour la détermination du changement d'heure été/hiver avec
   le numéro du jour dans la semaine (dernier dimance de mars et octobre)
*/
typedef enum {
  JEUDI = 0,
  VENDREDI,
  SAMEDI,
  DIMANCHE,
  LUNDI,
  MARDI,
  MERCREDI
} ENUM_DAYS_OF_WEEK;

typedef struct {
  ENUM_DAYS_OF_WEEK   num_day;              // 0: Jeudi, 1: Vendredi, ..., 6: Mercredi
  const char          *name;                // car le 01/01/1970 est un Jeudi ;-)
} ST_DAYS_IN_WEEK;

typedef struct {
  bool                flg_available;        // Dates de basculement disponibles (true/false)
  ST_DATE_AND_TIME    st_date_sommer;       // Date de basculement heure d'été
  ST_DATE_AND_TIME    st_date_winter;       // Date de basculement heure d'hiver
  ST_DAYS_IN_WEEK     st_days_in_week[7];   // => Si nbr_days_after = 0, ce jour est le dernier du mois ;-)
} ST_FOR_SOMMER_TIME_CHANGE;
// Fin: Définitions pour la détermination du changement d'heure été/hiver

typedef enum {
  ENUM_PERIOD_NONE = 0,
  ENUM_MI_PERIOD_DONE,
  ENUM_PERIOD_DONE
} ENUM_IN_THE_PERIOD;

class DateTime {
	private:
    bool flg_rtc_init;

    long epoch_start;
    long epoch;
    long epoch_diff;

    long duration_deconnexion;

    unsigned long epoch_rtc_gmt;
    unsigned int  epoch_rtc_offset;
  
    unsigned long duration_in_use;
 
		long my_mktime(ST_TM *timeptr);
		long calculatedEpochTime(ST_DATE_AND_TIME *i__st_date_and_time);
		bool setSommerWinterTimeChange(ST_DATE_AND_TIME *i__dateAndTime);
		void calcSommerTimeChange(ST_DATE_AND_TIME *io__dateAndTime);
		ENUM_SOMMER_WINTER getSommerWinterTimeChange(ST_DATE_AND_TIME *i__dateAndTime);
		void applySommerWinterHour(ST_DATE_AND_TIME *io__dateAndTime_presentation);

	public:
		DateTime();
 		~DateTime();

    void setRtcInit() { flg_rtc_init = true; };
    bool isRtcInit() { return flg_rtc_init; };
  
		long buildEPowerDateTime(const char i__date[], const char i__time[], char *o_date_time, ST_DATE_AND_TIME *o__dateAndTime,
      char *o__text_for_lcd = NULL, char *o__text_for_file_record = NULL);

    long getEpochStart() const { return epoch_start; };
    void setEpochStart(long i__value) { epoch_start = i__value; };
    long getEpoch() const { return epoch; };
    void setEpochAndDiff(long i__value) { epoch = i__value; epoch_diff = (epoch - epoch_start); };
    long getEpochDiff() const { return epoch_diff; };

    void formatEpochDiff(char *o__buffer) const;

    void incDurationDeconnexion() { duration_deconnexion++; };
    long getDurationDeconnexion() const { return duration_deconnexion; };
    void formatDuration(char *o__buffer, long i__value, char *o__text_for_file_record = NULL) const;
    void formatDurationDeconnexion(char *o__buffer) const;

    void          setRtcSecInDayGmt();
    unsigned long getRtcSecInDayGmt() const { return (epoch_rtc_gmt % 86400L); };
    unsigned long getRtcSecInDayLocal() const { return ((epoch_rtc_gmt + (epoch_rtc_offset * 3600L)) % 86400L); };
    unsigned int  getRtcSecInDayOffset() const { return epoch_rtc_offset; };
    ENUM_IN_THE_PERIOD isRtcSecInDayInRange() const;
 
    void getRtcTimeForLcd(char *o__text_for_lcd, char *o__text_for_file_record);

    void          setDurationInUse(unsigned long i__value) { duration_in_use = i__value; };
    unsigned long getDurationInUse() const { return duration_in_use; };
    void          incDurationInUse() { duration_in_use++; };
};

extern DateTime   *g__date_time;
extern ESP32Time  *g__rtc;

#endif
