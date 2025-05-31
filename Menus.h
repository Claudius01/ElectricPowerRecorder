// $Id: Menus.h,v 1.12 2025/05/31 13:47:57 administrateur Exp $

#ifndef __MENUS__
#define __MENUS__

#define TEXT_POSITION_MENU_X     192
#define TEXT_POSITION_MENU_Y     117

typedef enum {
  MENU_TYPE_NORMAL = 0,
  MENU_TYPE_SHORT,
  MENU_TYPE_NBR
} ENUM_MENU_TYPE_NORMAL_SHORT;

/* Deroulement des 5 menus:
   - Configuration RTC
   - Autorisation/Inhibition ecriture dans la SDCard
   - Reinitialisation des valeurs Min et Max a la valeur courante
   - Configuration de la periode de presentation
   - Configuration des unites (mV ou Watts)
   - Sortie du menu
 */
typedef enum {
	MENU_RTC = 0,
	MENU_SDCARD,
	MENU_VALUES_MIN_MAX,
	MENU_PERIODS,
  MENU_UNITS,
	MENU_EXIT,
  MENU_NBR
} ENUM_MENU;

#define MENU_START     MENU_RTC

typedef enum {
  MENU_NONE_IN_PROGRESS = -1,     // Pour aligner les 'in progress' sur les 'menus'
	MENU_SDCARD_IN_PROGRESS,
	MENU_RTC_IN_PROGRESS,
	MENU_VALUES_MIN_MAX_IN_PROGRESS,
	MENU_PERIODS_IN_PROGRESS,
	MENU_UNITS_IN_PROGRESS,
	MENU_EXIT_IN_PROGRESS,
  MENU_IN_PROGRESS_NBR
} ENUM_MENU_IN_PROGRESS;

typedef enum {
  SUB_MENU_PERIOD_NONE = -1,
  SUB_MENU_PERIOD_1_MINUTE,
  SUB_MENU_PERIOD_5_MINUTES,
  SUB_MENU_PERIOD_15_MINUTES,
  SUB_MENU_PERIOD_30_MINUTES,
  SUB_MENU_PERIOD_1_HOUR,
  SUB_MENU_PERIOD_3_HOURS,
  SUB_MENU_PERIOD_6_HOURS,
  SUB_MENU_PERIOD_EXIT,
  SUB_MENU_PERIOD_NBR
} ENUM_SUB_MENU_PERIOD;

typedef enum {
  SUB_MENU_UNIT_NONE = -1,
  SUB_MENU_UNIT_MILLIS_VOLTS,
  SUB_MENU_UNIT_WATTS,
  SUB_MENU_UNIT_EXIT,
  SUB_MENU_UNIT_NBR
} ENUM_SUB_MENU_UNIT;

class Menus {
	private:
    bool                            m__flg_menu;
    ENUM_MENU                       m__menu;
    ENUM_MENU_IN_PROGRESS           m__menu_in_progress;

    ENUM_SUB_MENU_PERIOD            m__sub_menu_period;
    ENUM_SUB_MENU_PERIOD            m__sub_menu_period_current;

    ENUM_SUB_MENU_UNIT              m__sub_menu_unit;
    ENUM_SUB_MENU_UNIT              m__sub_menu_unit_current;

    void exec_menu(const char *i__method);

    void exec_menu_rtc();
    void exec_menu_sdcard();
    void exec_menu_values_min_max();
    void exec_menu_periods();
    void exec_menu_units();
    void exec_menu_exit();

	public:
		Menus();
		~Menus();

    ENUM_MENU_IN_PROGRESS getMenuInProgress() const { return m__menu_in_progress; };

    ENUM_MENU getMenu() const { return m__menu; };
    void      setMenu(ENUM_MENU i__value) { m__menu = i__value; };

    // Gestion des periodes
    ENUM_SUB_MENU_PERIOD getSubMenuPeriod() const { return m__sub_menu_period; };
    void                 setSubMenuPeriod(ENUM_SUB_MENU_PERIOD i__value) { m__sub_menu_period = i__value; };

    ENUM_SUB_MENU_PERIOD getSubMenuPeriodCurrent() const { return m__sub_menu_period_current; };
    void                 setSubMenuPeriodCurrent(ENUM_SUB_MENU_PERIOD i__value) { m__sub_menu_period_current = i__value; };

    // Gestion des unites
    ENUM_SUB_MENU_UNIT   getSubMenuUnit() const { return m__sub_menu_unit; };
    void                 setSubMenuUnit(ENUM_SUB_MENU_UNIT i__value) { m__sub_menu_unit = i__value; };

    ENUM_SUB_MENU_UNIT   getSubMenuUnitCurrent() const { return m__sub_menu_unit_current; };
    void                 setSubMenuUnitCurrent(ENUM_SUB_MENU_UNIT i__value) { m__sub_menu_unit_current = i__value; };

    void click_button();
    void exit();
};

extern Menus        *g__menus;
extern const char   *g__menu_periods[SUB_MENU_PERIOD_NBR][MENU_TYPE_NBR];
extern const char   *g__menu_units[SUB_MENU_UNIT_NBR][MENU_TYPE_NBR];
#endif
