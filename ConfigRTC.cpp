// $Id: ConfigRTC.cpp,v 1.14 2025/06/02 17:32:37 administrateur Exp $

#if USE_SIMULATION
#include "ArduinoTypes.h"
#include "Arduino.h"
#include "Serial.h"
#else
#include <Arduino.h>
#endif

#include "Misc.h"

#include "Timers.h"
#include "Menus.h"
#include "GestionLCD.h"
#include "DateTime.h"
#include "ConfigRTC.h"
#include "AnalogRead.h"

static const char *g__days_in_week[7] = { "Jeu.", "Ven.", "Sam.", "Dim.", "Lun.", "Mar.", "Mer." };
static const int   g__nbr_days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void callback_end_config_rtc_scrolling()
{
  g__config_rtc->nextValue();   // Prochaine valeur d'un etat
}

void callback_end_wait_acq()
{
  g__config_rtc->setDone();     // Fin de l'acquisition => Set RTC wih the values acquired

  // Raz des consommations pour la bonne repartition heures creuses et pleines
  g__analog_read_1->initConsommations(); 

  g__menus->exit();
}

ConfigRTC::ConfigRTC() : m__config_rtc(CONFIG_RTC_NONE), m__flg_presentation(true)
{
  Serial.printf("ConfigRTC::ConfigRTC()\n");

  memset(&m__timeinfo_in_acquisition, '\0', sizeof(struct tm));

  // Valeurs initiales de configuration: 01/01/2020 00:00
  m__timeinfo_in_acquisition.tm_year = (2020 - 1900);
  m__timeinfo_in_acquisition.tm_mon  = (1 - 1);
  m__timeinfo_in_acquisition.tm_mday = 1;
  m__timeinfo_in_acquisition.tm_hour = 0;
  m__timeinfo_in_acquisition.tm_min  = 0;
  m__timeinfo_in_acquisition.tm_sec  = 0;
}

ConfigRTC::~ConfigRTC()
{
  Serial.printf("ConfigRTC::~ConfigRTC()\n");
}

bool ConfigRTC::isInProgress() const
{
  bool l__flg_rtn = false;

  l__flg_rtn |= (m__config_rtc == CONFIG_RTC_IN_PROGRESS_YEAR);
  l__flg_rtn |= (m__config_rtc == CONFIG_RTC_IN_PROGRESS_MONTH);
  l__flg_rtn |= (m__config_rtc == CONFIG_RTC_IN_PROGRESS_MDAY);
  l__flg_rtn |= (m__config_rtc == CONFIG_RTC_IN_PROGRESS_HOUR);
  l__flg_rtn |= (m__config_rtc == CONFIG_RTC_IN_PROGRESS_MIN);
  l__flg_rtn |= (m__config_rtc == CONFIG_RTC_WAIT_OF_ACQ);

  return l__flg_rtn;
}

const char *ConfigRTC::getDayInWeek() const
{
  unsigned long l__epoch = mktime((struct tm*)&m__timeinfo_in_acquisition);

  return g__days_in_week[(l__epoch / 86400L) % 7];
}

void ConfigRTC::presentation()
{
  char l__text_for_lcd[32];
  memset(l__text_for_lcd, '\0', sizeof(l__text_for_lcd));

  // Arret eventuel du timer 'TIMER_MENU_SCROLLING' avant la presentation
  if (m__config_rtc != CONFIG_RTC_WAIT_OF_ACQ) {
    if (g__timers->isInUse(TIMER_MENU_SCROLLING)) {
      g__timers->stop(TIMER_MENU_SCROLLING);
    }
  }

  sprintf(l__text_for_lcd, "%s ", getDayInWeek());
  g__gestion_lcd->Paint_DrawString_EN(6, 8, l__text_for_lcd, &Font16, BLACK, YELLOW);

  if (m__flg_presentation == true) {
    sprintf(l__text_for_lcd, "%02u/%02u/%04u %02u:%02u",
      m__timeinfo_in_acquisition.tm_mday, (m__timeinfo_in_acquisition.tm_mon + 1), (m__timeinfo_in_acquisition.tm_year + 1900),
      m__timeinfo_in_acquisition.tm_hour, m__timeinfo_in_acquisition.tm_min);
    g__gestion_lcd->Paint_DrawString_EN(6 + (11 * 5), 8, l__text_for_lcd, &Font16, BLACK, YELLOW);

    m__flg_presentation = false;
  }

  if (m__config_rtc == CONFIG_RTC_IN_PROGRESS_YEAR) {
    sprintf(l__text_for_lcd, "%04u", (m__timeinfo_in_acquisition.tm_year + 1900));
    g__gestion_lcd->Paint_DrawString_EN(6 + (11 * 11), 8, l__text_for_lcd, &Font16, BLACK, MAGENTA);
  }
  else if (m__config_rtc == CONFIG_RTC_IN_PROGRESS_MONTH) {
    sprintf(l__text_for_lcd, "%02u", (m__timeinfo_in_acquisition.tm_mon + 1));
    g__gestion_lcd->Paint_DrawString_EN(6 + (11 * 8), 8, l__text_for_lcd, &Font16, BLACK, MAGENTA);
  }
  else if (m__config_rtc == CONFIG_RTC_IN_PROGRESS_MDAY) {
    sprintf(l__text_for_lcd, "%02u", m__timeinfo_in_acquisition.tm_mday);
    g__gestion_lcd->Paint_DrawString_EN(6 + (11 * 5), 8, l__text_for_lcd, &Font16, BLACK, MAGENTA);
  }
  else if (m__config_rtc == CONFIG_RTC_IN_PROGRESS_HOUR) {
    sprintf(l__text_for_lcd, "%02u", m__timeinfo_in_acquisition.tm_hour);
    g__gestion_lcd->Paint_DrawString_EN(6 + (11 * 16), 8, l__text_for_lcd, &Font16, BLACK, MAGENTA);
  }
  else if (m__config_rtc == CONFIG_RTC_IN_PROGRESS_MIN) {
    sprintf(l__text_for_lcd, "%02u", m__timeinfo_in_acquisition.tm_min);
    g__gestion_lcd->Paint_DrawString_EN(6 + (11 * 19), 8, l__text_for_lcd, &Font16, BLACK, MAGENTA);
  }

  // Lancement eventuel du timer 'TIMER_MENU_SCROLLING' apres la presentation
  if (m__config_rtc != CONFIG_RTC_WAIT_OF_ACQ) {
    g__timers->start(TIMER_MENU_SCROLLING, DURATION_TIMER_MENU_SCROLLING, &callback_end_config_rtc_scrolling);
  }
}

void ConfigRTC::nextValue()
{
  switch(m__config_rtc) {
  case CONFIG_RTC_IN_PROGRESS_YEAR:
    m__timeinfo_in_acquisition.tm_year++;
    if (m__timeinfo_in_acquisition.tm_year > (2030 - 1900)) {
      m__timeinfo_in_acquisition.tm_year = (2020 - 1900);
    }
    break;
  case CONFIG_RTC_IN_PROGRESS_MONTH:
    m__timeinfo_in_acquisition.tm_mon = ((m__timeinfo_in_acquisition.tm_mon + 1) % 12);
    break;
  case CONFIG_RTC_IN_PROGRESS_MDAY:
    m__timeinfo_in_acquisition.tm_mday++;

    if (LEAP_YEAR(m__timeinfo_in_acquisition.tm_year % 100) && (m__timeinfo_in_acquisition.tm_mon + 1) == 2) {
      if (m__timeinfo_in_acquisition.tm_mday > 29) {
        m__timeinfo_in_acquisition.tm_mday = 1;
      }
    }
    else {
      if (m__timeinfo_in_acquisition.tm_mday > g__nbr_days_in_month[m__timeinfo_in_acquisition.tm_mon]) {
        m__timeinfo_in_acquisition.tm_mday = 1;
      }
    }
    break;
  case CONFIG_RTC_IN_PROGRESS_HOUR:
    m__timeinfo_in_acquisition.tm_hour = ((m__timeinfo_in_acquisition.tm_hour + 1) % 24);
    break;
  case CONFIG_RTC_IN_PROGRESS_MIN:
    m__timeinfo_in_acquisition.tm_min = ((m__timeinfo_in_acquisition.tm_min + 1) % 60);
    break;
  default:
    break;
  }

  presentation();
}

void ConfigRTC::nextState()
{
  m__flg_presentation = true;

  switch(m__config_rtc) {
  case CONFIG_RTC_NONE:
    m__config_rtc = CONFIG_RTC_IN_PROGRESS_YEAR;
    break;
  case CONFIG_RTC_IN_PROGRESS_YEAR:
    m__config_rtc = CONFIG_RTC_IN_PROGRESS_MONTH;
    break;
  case CONFIG_RTC_IN_PROGRESS_MONTH:
    m__config_rtc = CONFIG_RTC_IN_PROGRESS_MDAY;
    break;
  case CONFIG_RTC_IN_PROGRESS_MDAY:
    m__config_rtc = CONFIG_RTC_IN_PROGRESS_HOUR;
    break;
  case CONFIG_RTC_IN_PROGRESS_HOUR:
    m__config_rtc = CONFIG_RTC_IN_PROGRESS_MIN;
    break;
  case CONFIG_RTC_IN_PROGRESS_MIN:
    m__config_rtc = CONFIG_RTC_WAIT_OF_ACQ;

    // Fin de l'acquisition si pas d'appui bouton a l'expiration de 'TIMER_CONFIG_RTC_WAIT_ACQ'
    g__timers->start(TIMER_MENU_WAIT_ACQ, DURATION_TIMER_MENU_WAIT_ACQ, &callback_end_wait_acq);
    break;
  case CONFIG_RTC_WAIT_OF_ACQ:
    m__config_rtc = CONFIG_RTC_IN_PROGRESS_YEAR;

    // Retour au debut de l'acquisition si appui bouton durant 'DURATION_TIMER_MENU_WAIT_ACQ'
    g__timers->stop(TIMER_MENU_WAIT_ACQ);
    break;
  default:
    break;
  }

  presentation();   // Presentation @ etat
}

void ConfigRTC::setDone()
{
  Serial.printf("%s(): Entering...\n", __FUNCTION__);

  char l__text_for_lcd[32];
  memset(l__text_for_lcd, '\0', sizeof(l__text_for_lcd));

  // Retablissement du parametre 'tm_min' acquis
  sprintf(l__text_for_lcd, "%02u", m__timeinfo_in_acquisition.tm_min);
  g__gestion_lcd->Paint_DrawString_EN(6 + (11 * 19), 8, l__text_for_lcd, &Font16, BLACK, YELLOW);

  m__config_rtc = CONFIG_RTC_DONE;

  g__rtc->setTimeStruct(m__timeinfo_in_acquisition);
  g__date_time->setRtcInit();

  // Changement de la symbologie Led GREEN...
  g__chenillard = 0x7F7F;
}

void ConfigRTC::click_button()
{
  // Passage au prochain etat
  nextState();

  if (m__config_rtc != CONFIG_RTC_WAIT_OF_ACQ) {
    if (g__timers->isInUse(TIMER_MENU_SCROLLING)) {
      g__timers->stop(TIMER_MENU_SCROLLING);
    }
   g__timers->start(TIMER_MENU_SCROLLING, DURATION_TIMER_MENU_SCROLLING, &callback_end_config_rtc_scrolling);
  }
}
