// $Id: Menus.cpp,v 1.25 2025/06/15 17:01:07 administrateur Exp $

#if USE_SIMULATION
#include "ArduinoTypes.h"
#include "Arduino.h"
#include "Serial.h"
#else
#include <Arduino.h>
#endif

#if USE_SIMULATION
#include "ArduinoTypes.h"
#include "Serial.h"
#include "String.h"
 
#include "SDCardSimu.h"
#endif

#include "Misc.h"
#include "Timers.h"
#include "Menus.h"
#include "GestionLCD.h"
#include "SDCard.h"
#include "AnalogRead.h"
#include "ConfigRTC.h"

/* Libelles pour les traces et l'ecran LCD
   => Deroulement des menus

   => Padding a blanc pour effacer les proprietes (cf. 'TEXT_LENGTH_PROPERTIES = 10 caracteres')
 */
static const char *g__menu_label_menu[MENU_NBR] = {
  "RTC       ",
  "SDCard    ",
  "Min/Max   ",
  "Periods   ",
  "Unites    ",
  "Exit      "
};

/* Libelles pour les periodes
   => Deroulement des choix de valeurs ('MENU_TYPE_NORMAL')
   => Affichage   des choix de valeurs ('MENU_TYPE_SHORT')
 */
const char *g__menu_periods[SUB_MENU_PERIOD_NBR][MENU_TYPE_NBR] = {
  { "  1 Min", "1'"  },   // SUB_MENU_PERIOD_1_MINUTE
  { "  5 Min", "5'"  },   // SUB_MENU_PERIOD_5_MINUTES
  { " 15 Min", "15'" },   // SUB_MENU_PERIOD_15_MINUTES
  { " 30 Min", "30'" },   // SUB_MENU_PERIOD_30_MINUTES
  { "  1 H  ", "1H"  },   // SUB_MENU_PERIOD_1_HOUR
  { "  3 H  ", "3H"  },   // SUB_MENU_PERIOD_3_HOURS
  { "  6 H  ", "6H"  },   // SUB_MENU_PERIOD_6_HOURS
  { " Exit  ", "???" }    // SUB_MENU_PERIOD_EXIT (non acces a 'g__menu_periods[SUB_MENU_PERIOD_EXIT][SUB_MENU_PERIOD_EXIT]')
};

/* Libelles pour les unites
   => Deroulement des choix de valeurs ('MENU_TYPE_NORMAL')
   => Affichage   des choix de valeurs ('MENU_TYPE_SHORT')
 */
const char *g__menu_units[SUB_MENU_UNIT_NBR][MENU_TYPE_NBR] = {
  { "mV     ", "mV" },    // SUB_MENU_UNIT_MILLIS_VOLTS
  { "Watts  ", "W"  },    // SUB_MENU_UNIT_WATTS
  { "W Hour ", "Wh"  },   // SUB_MENU_UNIT_WATTS_HOUR
  { " Exit  ", "??" }     // SUB_MENU_UNIT_EXIT (non acces a 'g__menu_units[SUB_MENU_UNIT_EXIT][SUB_MENU_PERIOD_EXIT]')
};
// Fin: Libelles pour l'ecran LCD

void callback_menu_scrolling()
{
  if (g__menus->getMenu() == MENU_PERIODS && g__menus->getSubMenuPeriod() != SUB_MENU_PERIOD_NONE) {
    // Scrolling des sous-menus 'periods'
    ENUM_SUB_MENU_PERIOD l__sub_menu_period = g__menus->getSubMenuPeriod();
    g__menus->setSubMenuPeriod((ENUM_SUB_MENU_PERIOD)((l__sub_menu_period + 1 ) % (int)SUB_MENU_PERIOD_NBR));

    Serial.printf("callback_menu_scrolling(): [%d] (%s) -> [%d] (%s)\n",
      l__sub_menu_period, g__menu_periods[l__sub_menu_period][MENU_TYPE_NORMAL],
      g__menus->getSubMenuPeriod(), g__menu_periods[g__menus->getSubMenuPeriod()][MENU_TYPE_NORMAL]);

    // Presentation en WHITE de la periode deja configuree
    g__gestion_lcd->Paint_DrawString_EN(TEXT_POSITION_MENU_X, TEXT_POSITION_MENU_Y,
      g__menu_periods[g__menus->getSubMenuPeriod()][MENU_TYPE_NORMAL], &Font12, BLACK,
      (g__menus->getSubMenuPeriod() == g__menus->getSubMenuPeriodCurrent()) ? WHITE : MAGENTA);
  }
  else if (g__menus->getMenu() == MENU_UNITS && g__menus->getSubMenuUnit() != SUB_MENU_UNIT_NONE) {
    // Scrolling des sous-menus 'units'
    ENUM_SUB_MENU_UNIT l__sub_menu_unit = g__menus->getSubMenuUnit();
    g__menus->setSubMenuUnit((ENUM_SUB_MENU_UNIT)((l__sub_menu_unit + 1 ) % (int)SUB_MENU_UNIT_NBR));

    Serial.printf("callback_menu_scrolling(): [%d] (%s) -> [%d] (%s)\n",
      l__sub_menu_unit, g__menu_units[l__sub_menu_unit][MENU_TYPE_NORMAL],
      g__menus->getSubMenuUnit(), g__menu_units[g__menus->getSubMenuUnit()][MENU_TYPE_NORMAL]);

    // Presentation en WHITE de l'unite deja configuree
    g__gestion_lcd->Paint_DrawString_EN(TEXT_POSITION_MENU_X, TEXT_POSITION_MENU_Y,
      g__menu_units[g__menus->getSubMenuUnit()][MENU_TYPE_NORMAL], &Font12, BLACK,
      (g__menus->getSubMenuUnit() == g__menus->getSubMenuUnitCurrent()) ? WHITE : MAGENTA);
  }
  else {
    // Scrolling des menus
    ENUM_MENU l__menu = g__menus->getMenu();
    g__menus->setMenu((ENUM_MENU)((l__menu + 1 ) % (int)MENU_NBR));

    // Pas de menu RTC si deja configure
    if (g__config_rtc->isDone() && g__menus->getMenu() == MENU_RTC)  {
      l__menu = g__menus->getMenu();
      g__menus->setMenu((ENUM_MENU)((l__menu + 1 ) % (int)MENU_NBR));
    }

    Serial.printf("callback_menu_scrolling(): m__menu [%d] (%s) -> [%d] (%s)\n",
      l__menu, g__menu_label_menu[l__menu], g__menus->getMenu(), g__menu_label_menu[g__menus->getMenu()]);

    g__gestion_lcd->Paint_DrawString_EN(TEXT_POSITION_MENU_X, TEXT_POSITION_MENU_Y, g__menu_label_menu[g__menus->getMenu()], &Font12, BLACK, MAGENTA);
  }
  
  // Armement pour passer au prochain menu a l'expiration de 'TIMER_MENU_SCROLLING'
  g__timers->start(TIMER_MENU_SCROLLING, DURATION_TIMER_MENU_SCROLLING, &callback_menu_scrolling);
}

// Acquitement du choix du menu et sous-menu selectionnes
void callback_menu_end_wait_acq()
{
  switch (g__menus->getMenuInProgress()) {
  case MENU_SDCARD_IN_PROGRESS:
    if (g__sdcard->getInhAppendGpsFrame() == false) {
      /* Le flag 'flg_inh_append_gps_frame' sera maj a 'true'
         => Arret des enregistrements...
      */
      g__sdcard->appendGpsFrame("#Stop Recording\n");
    }

    // Bascule "Autorisation/Inhibition" concatenation infos dans la SDCard
    g__sdcard->setInhAppendGpsFrame(g__sdcard->getInhAppendGpsFrame() ? false : true);

    if (g__sdcard->getInhAppendGpsFrame() == false) {
      /* Le flag 'flg_inh_append_gps_frame' est maj a 'false'
         => Reprise des enregistrements...
      */
      g__sdcard->appendGpsFrame("#Restart Recording\n");
    }

    g__menus->exit();
    break;

  case MENU_VALUES_MIN_MAX_IN_PROGRESS:
    g__analog_read_1->resetMinMaxValues();    // Raz des valeurs min, max + des echantillons
    g__menus->exit();
    break;

  case MENU_PERIODS_IN_PROGRESS:
    Serial.printf("Menus::callback_menu_end_wait_acq(): Acq Sub menu [%d] -> [%d]\n",
      g__menus->getMenuInProgress(), g__menus->getSubMenuPeriod());

    // Reinitialisation des Min/Max si nouvelle periode
    if (g__menus->getSubMenuPeriod() != g__menus->getSubMenuPeriodCurrent()) {
      g__analog_read_1->resetMinMaxValues();  // Raz des valeurs min, max + des echantillons
    }

    // Prise en compte de la periode
    g__menus->setSubMenuPeriodCurrent(g__menus->getSubMenuPeriod());
    g__gestion_lcd->setScreenVirtualPeriod(g__menus->getSubMenuPeriodCurrent());

    g__menus->setSubMenuPeriod(SUB_MENU_PERIOD_NONE);

    g__menus->exit();

    break;

  case MENU_UNITS_IN_PROGRESS:
    Serial.printf("Menus::callback_menu_end_wait_acq(): Acq Sub menu [%d] -> [%d]\n",
      g__menus->getMenuInProgress(), g__menus->getSubMenuUnit());

    // Prise en compte de l'unite
    g__menus->setSubMenuUnitCurrent(g__menus->getSubMenuUnit());
    g__menus->setSubMenuUnit(SUB_MENU_UNIT_NONE);

    switch (g__menus->getSubMenuUnitCurrent()) {
    case SUB_MENU_UNIT_MILLIS_VOLTS:
      g__analog_read_1->setTypeUnit(UNIT_MILLI_VOLTS);

      // Preparation zone d'affichage...
      g__gestion_lcd->Paint_DrawString_EN(2, 29, "XXXX XXXX XXXX XXXX", &Font16, BLACK, BLACK);
      break;
    case SUB_MENU_UNIT_WATTS:
      g__analog_read_1->setTypeUnit(UNIT_WATTS);

      // Preparation zone d'affichage...
      g__gestion_lcd->Paint_DrawString_EN(2, 29, "XXXX XXXX XXXX XXXX", &Font16, BLACK, BLACK);
      break;
    case SUB_MENU_UNIT_WATTS_HOUR:
      g__analog_read_1->setTypeUnit(UNIT_WATTS_HOUR);

      // Preparation zone d'affichage...
      g__gestion_lcd->Paint_DrawString_EN(2, 29, "XXXXX XXXXX XXXXX  ", &Font16, BLACK, BLACK);
      break;
    default:
      Serial.printf("error Menus::callback_menu_end_wait_acq(): Unknow unit type [%d]\n", g__menus->getSubMenuUnitCurrent());
      break;
    }

    g__menus->exit();

    break;

  default:
    Serial.printf("Menus::callback_menu_end_wait_acq(): Sub menu [%d] not implemented\n", g__menus->getMenuInProgress());
    break;
  }
}

Menus::Menus() : m__flg_menu(false), m__menu(MENU_START), m__menu_in_progress(MENU_NONE_IN_PROGRESS),
                 m__sub_menu_period(SUB_MENU_PERIOD_NONE), m__sub_menu_period_current(SUB_MENU_PERIOD_NONE),
                 m__sub_menu_unit(SUB_MENU_UNIT_NONE), m__sub_menu_unit_current(SUB_MENU_UNIT_NONE)
{
  Serial.printf("Menus::Menus()\n");

  // Prise de la periode courante lue depuis l'initialisation de la classe 'g__gestion_lcd'
  m__sub_menu_period_current = g__gestion_lcd->getSubMenuPeriod();

  // Prise de l'unite' courante lue depuis l'initialisation de la classe 'g__gestion_lcd'
  m__sub_menu_unit_current = g__gestion_lcd->getSubMenuUnit();
}

Menus::~Menus()
{
  Serial.printf("Menus::~Menus()\n");
}

void Menus::click_button()
{
  if (m__flg_menu == false) {
    g__gestion_lcd->Paint_DrawString_EN(TEXT_POSITION_MENU_X, TEXT_POSITION_MENU_Y, g__menu_label_menu[m__menu], &Font12, BLACK, MAGENTA);

    // Armement pour passer au prochain etat du menu a l'expiration 
    g__timers->start(TIMER_MENU_SCROLLING, DURATION_TIMER_MENU_SCROLLING, &callback_menu_scrolling);

    m__flg_menu = true;
  }
  else {
    switch (m__menu) {
    case MENU_EXIT:
      m__menu_in_progress = MENU_EXIT_IN_PROGRESS;
      exec_menu_exit();
      break;
    case MENU_RTC:
      m__menu_in_progress = MENU_RTC_IN_PROGRESS;
      exec_menu_rtc();
      break;
    case MENU_SDCARD:
      m__menu_in_progress = MENU_SDCARD_IN_PROGRESS;
      exec_menu_sdcard();
      break;
    case MENU_VALUES_MIN_MAX:
      m__menu_in_progress = MENU_VALUES_MIN_MAX_IN_PROGRESS;
      exec_menu_values_min_max();
      break;
    case MENU_PERIODS:
      m__menu_in_progress = MENU_PERIODS_IN_PROGRESS;
      exec_menu_periods();
      break;
    case MENU_UNITS:
      m__menu_in_progress = MENU_UNITS_IN_PROGRESS;
      exec_menu_units();
      break;
    default:
      Serial.printf("Menus::click_button(): Menu [%d] not implemented\n", m__menu);
      break;
    }
  }
}

// Traitement commun a toutes les executions
void Menus::exec_menu(const char *i__method)
{
  Serial.printf("%s: Entering (%d) in progress...\n", i__method, m__menu_in_progress);

  g__gestion_lcd->Paint_DrawString_EN(TEXT_POSITION_MENU_X, TEXT_POSITION_MENU_Y, g__menu_label_menu[m__menu], &Font12, BLACK, YELLOW);

  if (g__timers->isInUse(TIMER_MENU_SCROLLING)) {
    g__timers->stop(TIMER_MENU_SCROLLING);
  }
}

void Menus::exec_menu_exit()
{
  exec_menu(__FUNCTION__);

  g__gestion_lcd->Paint_DrawString_EN(TEXT_POSITION_MENU_X, TEXT_POSITION_MENU_Y, "       ", &Font12, BLACK, MAGENTA);

  m__menu = MENU_START;       // Menu de demarrage 

  // Pas de menu RTC si deja configure
  if (g__config_rtc->isDone() && m__menu == MENU_RTC)  {
    m__menu = ((ENUM_MENU)((m__menu + 1 ) % (int)MENU_NBR));
  }

  m__menu_in_progress = MENU_NONE_IN_PROGRESS;

  m__flg_menu = false;
}

void Menus::exec_menu_rtc()
{
  exec_menu(__FUNCTION__);

  g__config_rtc->nextState();
}

void Menus::exec_menu_sdcard()
{
  exec_menu(__FUNCTION__);

  // Armement pour la prise compte a l'expiration 
  g__timers->start(TIMER_MENU_WAIT_ACQ, DURATION_TIMER_MENU_WAIT_ACQ, &callback_menu_end_wait_acq);
}

void Menus::exec_menu_values_min_max()
{
  exec_menu(__FUNCTION__);

  // Armement pour la prise compte a l'expiration 
  g__timers->start(TIMER_MENU_WAIT_ACQ, DURATION_TIMER_MENU_WAIT_ACQ, &callback_menu_end_wait_acq);
}

void Menus::exec_menu_periods()
{
  Serial.printf("Menus::exec_menu_periods(): Entering (sub menu [%d])...\n", m__sub_menu_period);

  if (g__timers->isInUse(TIMER_MENU_SCROLLING)) {
    g__timers->stop(TIMER_MENU_SCROLLING);
  }

  // Deroulement des periods
  if (m__sub_menu_period == SUB_MENU_PERIOD_NONE) {
    m__sub_menu_period = (ENUM_SUB_MENU_PERIOD)(SUB_MENU_PERIOD_NONE + 1);    // 1st period a derouler

    g__gestion_lcd->Paint_DrawString_EN(TEXT_POSITION_MENU_X, TEXT_POSITION_MENU_Y, g__menu_periods[m__sub_menu_period][MENU_TYPE_NORMAL], &Font12, BLACK,
      (g__menus->getSubMenuPeriod() == g__menus->getSubMenuPeriodCurrent()) ? WHITE : MAGENTA);

    // Armement pour passer au prochain sous-menu de 'ENUM_SUB_MENU_PERIOD' a l'expiration de 'TIMER_MENU_SCROLLING'
    g__timers->start(TIMER_MENU_SCROLLING, DURATION_TIMER_MENU_SCROLLING, &callback_menu_scrolling);
  }
  else {
    if (m__sub_menu_period == SUB_MENU_PERIOD_EXIT) {
      // Sortie immediate
      g__menus->setSubMenuPeriod(SUB_MENU_PERIOD_NONE);
      g__menus->exit();
    }
    else {
      // Saisie de la periode
      g__gestion_lcd->Paint_DrawString_EN(TEXT_POSITION_MENU_X, TEXT_POSITION_MENU_Y, g__menu_periods[m__sub_menu_period][MENU_TYPE_NORMAL], &Font12, BLACK, YELLOW);

      // Armement pour la prise compte a l'expiration 
      g__timers->start(TIMER_MENU_WAIT_ACQ, DURATION_TIMER_MENU_WAIT_ACQ, &callback_menu_end_wait_acq);  
    }
  }
}

void Menus::exec_menu_units()
{
  Serial.printf("Menus::exec_menu_units(): Entering (sub menu [%d])...\n", m__sub_menu_unit);

  if (g__timers->isInUse(TIMER_MENU_SCROLLING)) {
    g__timers->stop(TIMER_MENU_SCROLLING);
  }

  // Deroulement des unites
  if (m__sub_menu_unit == SUB_MENU_UNIT_NONE) {
    m__sub_menu_unit = (ENUM_SUB_MENU_UNIT)(SUB_MENU_UNIT_NONE + 1);    // 1st unit a derouler

    g__gestion_lcd->Paint_DrawString_EN(TEXT_POSITION_MENU_X, TEXT_POSITION_MENU_Y, g__menu_units[m__sub_menu_unit][MENU_TYPE_NORMAL], &Font12, BLACK,
      (g__menus->getSubMenuUnit() == g__menus->getSubMenuUnitCurrent()) ? WHITE : MAGENTA);

    // Armement pour passer au prochain sous-menu de 'ENUM_SUB_MENU_UNIT' a l'expiration de 'TIMER_MENU_SCROLLING'
    g__timers->start(TIMER_MENU_SCROLLING, DURATION_TIMER_MENU_SCROLLING, &callback_menu_scrolling);
  }
  else {
    if (m__sub_menu_unit == SUB_MENU_UNIT_EXIT) {
      // Sortie immediate
      g__menus->setSubMenuUnit(SUB_MENU_UNIT_NONE);
      g__menus->exit();
    }
    else {
      // Saisie de la unite
      g__gestion_lcd->Paint_DrawString_EN(TEXT_POSITION_MENU_X, TEXT_POSITION_MENU_Y, g__menu_units[m__sub_menu_unit][MENU_TYPE_NORMAL], &Font12, BLACK, YELLOW);

      // Armement pour la prise compte a l'expiration 
      g__timers->start(TIMER_MENU_WAIT_ACQ, DURATION_TIMER_MENU_WAIT_ACQ, &callback_menu_end_wait_acq);  
    }
  }
}

void Menus::exit()
{
  // Arrets eventuels des 2 timers
  if (g__timers->isInUse(TIMER_MENU_SCROLLING)) {
    g__timers->stop(TIMER_MENU_SCROLLING);
  }

  if (g__timers->isInUse(TIMER_MENU_WAIT_ACQ)) {
    g__timers->stop(TIMER_MENU_WAIT_ACQ);
  }

  exec_menu_exit();
};
