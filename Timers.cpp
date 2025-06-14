// $Id: Timers.cpp,v 1.9 2025/06/02 13:16:01 administrateur Exp $

/* Evolutions:
   - 2025/02/21: Utilisation de 'Serial.printf()'
*/

#if USE_SIMULATION
#include "ArduinoTypes.h"
#include "Arduino.h"
#include "Serial.h"
#else
#include <Arduino.h>
#endif

//#include "Misc.h"

#include "Timers.h"

static ST_TIMERS        g__st_timers[] =
{
  { TIMER_CHENILLARD, false, "Chenillard" },
  { TIMER_CONNECT,    false, "Connect" },

  { TIMER_SDCARD_ACCES,       false, "SD Card acces" },
  { TIMER_SDCARD_ERROR,       false, "SD Card error" },
  { TIMER_SDCARD_RETRY_INIT,  false, "SD Card retry init" },
  { TIMER_SDCARD_INIT_ERROR,  false, "SD Card init error" },

  { TIMER_NMEA_ACTIVITY,      false, "Activite NMEA" },
  { TIMER_NMEA_ERROR,         false, "NMEA error" },

  { TIMER_SEND_GPSPILOT,      false, "Emission GpsPilot" },
  { TIMER_ACTIVATE_SDCARD,    false, "Activite SDCARD" },

#if 0
  { TIMER_CONFIG_RTC_SCROLLING, false, "Config RTC Scrolling" },
  { TIMER_CONFIG_RTC_WAIT_ACQ,  false, "Config RTC Wait Acq" },
#endif

  { TIMER_ANALOG_ACQ,          false, "Analog acq" },

  { TIMER_MENU_SCROLLING,      false, "Menu Scrolling" },
  { TIMER_MENU_WAIT_ACQ,       false, "Menu Wait Acq" },

  { TIMER_CONSO_FLASH,         false, "Conso. flash" },
  { TIMER_CONSO_PERIOD,        false, "Conso. period" },

#if USE_SIMULATION
  { TIMER_WATCHDOG,            false, "Watchdog" },
#endif
};

Timers::Timers()
{
  Serial.printf("Timers::Timers()\n");

  size_t n = 0;
  for (n  = 0; n < NBR_OF_TIMERS; n++) {
    timer[n].in_use       = false;
    timer[n].duration     = -1L;
    timer[n].fct_callback = NULL;
  }

  // Verification des definitions dans 'g__st_timers'
  size_t l__nbr_timers = (sizeof(g__st_timers) / sizeof(ST_TIMERS));
  if (l__nbr_timers != NBR_OF_TIMERS) {
	 Serial.printf("Timers::Timers(): Error of 'g__st_timers' definition\n");
	 Serial.printf("Timers::Timers(): [%d] != [%d]\n", l__nbr_timers, NBR_OF_TIMERS);
  }
}

Timers::~Timers()
{
  Serial.printf("Timers::~Timers()\n");
}

/*  Mise a jour de tous les timers toutes les 10 mS
 */
void Timers::update()
{
  int n = 0;
  for (n = 0; n < NBR_OF_TIMERS; n++) {
    if (timer[n].in_use == true && timer[n].duration > 0) {
      timer[n].duration -= 1L;
    }
  }
}

/*  Test de tous les timers (hors Its)
 *  => Appel de la methode 'callback()' renseignée
 */
void Timers::test()
{
  int n = 0;
  for (n = 0; n < NBR_OF_TIMERS; n++) {
    if (timer[n].in_use == true) {
      if (timer[n].duration == 0L) {
        if (g__st_timers[n].flg_trace) {
          Serial.printf("Timers::test(): Expiration #%d/#%d (%s) -> call [%p]\n",
            n, (NBR_OF_TIMERS - 1), getText((DEF_TIMERS)n), timer[n].fct_callback);
        }

        // Stop this timer and set to 'not use'
        timer[n].duration = -1L;
        timer[n].in_use   = false;

        if (timer[n].fct_callback != NULL) {
          // Execution of 'callback' method
          (*timer[n].fct_callback)();
        }
      }
    }
  }
}

boolean Timers::isInUse(DEF_TIMERS i__def_timer)
{
  return (i__def_timer >= 0 && i__def_timer < NBR_OF_TIMERS) ? timer[i__def_timer].in_use : false;
}

/*  Launch a timer in the list
 */
boolean Timers::start(DEF_TIMERS i__def_timer, long l__duration, void (*i__fct_callback)(void))
{
  boolean l__rtn = false;

  if (i__def_timer >= 0 && i__def_timer < NBR_OF_TIMERS) {
    if (timer[i__def_timer].in_use == false) {
      timer[i__def_timer].in_use        = true;
      timer[i__def_timer].duration_init = l__duration;
      timer[i__def_timer].duration      = l__duration;
      timer[i__def_timer].fct_callback  = i__fct_callback;

      l__rtn = true;

      if (g__st_timers[i__def_timer].flg_trace) {
        Serial.printf("Timers::start(#%d-%s, %ld, %p)\n", i__def_timer, getText(i__def_timer), l__duration, i__fct_callback);
      }
    }
  }
  else {
    Serial.printf("Warning: Timers::start(#%d-%s): Unknown\n", i__def_timer, getText(i__def_timer));
  }

  return l__rtn;
}

/*  Relaunch a timer in the list
 */
boolean Timers::restart(DEF_TIMERS i__def_timer, long l__duration)
{
  boolean l__rtn = false;

  if (i__def_timer >= 0 && i__def_timer < NBR_OF_TIMERS) {
    if (timer[i__def_timer].in_use == true) {
      timer[i__def_timer].duration_init = l__duration;
      timer[i__def_timer].duration      = l__duration;

      l__rtn = true;

      if (g__st_timers[i__def_timer].flg_trace) {
        Serial.printf("Timers::restart(#%d-%s, %ld)\n", i__def_timer, getText(i__def_timer), l__duration);
      }
    }
  }
  else {
    Serial.printf("Warning: Timers::restart(#%d-%s): Unknown\n", i__def_timer, getText(i__def_timer));
  }

  return l__rtn;
}

/*  Stop a timer in the list
 */
boolean Timers::stop(DEF_TIMERS i__def_timer)
{
  boolean l__rtn = false;

  if (i__def_timer >= 0 && i__def_timer < NBR_OF_TIMERS && timer[i__def_timer].in_use == true) {
    timer[i__def_timer].in_use        = false;
    timer[i__def_timer].duration_init = -1L;
    timer[i__def_timer].duration      = -1L;
    timer[i__def_timer].fct_callback  = NULL;

    l__rtn = true;

    if (g__st_timers[i__def_timer].flg_trace) {
      Serial.printf("Timers::stop(#%d-%s)\n", i__def_timer, getText(i__def_timer));
    }
  }
#if 1   // TBC: Fix: Warning: Timers::stop(#4-Activity FIFO/Tx Play): Unknown or not in use
  else {
    Serial.printf("Warning: Timers::stop(#%d-%s): Unknown or not in use\n", i__def_timer, getText(i__def_timer));
  }
#endif

  return l__rtn;
}

/*  Return the duration in 100 mS resolution of timer in progress
 *
 *  Remarque: If return value < 0L; Duration expired at the moment of reading
 *            but the timer still in use ;-)
 *
 *  Examples: - Return 150  =>  1500 mS => 1.5 Sec
 *            - Return 599  =>  5990 mS => 5.9 Sec 
 *            - Return 3520 => 35200 mS => 35.2 Sec (32 Sec + 200 mS)
 */
long Timers::getDuration(DEF_TIMERS i__def_timer)
{
  long l__duration = -1L;

  if (i__def_timer >= 0 && i__def_timer < NBR_OF_TIMERS && timer[i__def_timer].in_use == true) {
    //l__duration = (timer[i__def_timer].duration_init - timer[i__def_timer].duration);
    l__duration = timer[i__def_timer].duration;
  }
  else {
    Serial.printf("Warning: Timers::getDuration(#%d-%s): Unknown or not in use\n", i__def_timer, getText(i__def_timer));
  }

  return l__duration;
}

/* Ajout d'une durée à un timer en cours d'exécution
 * 
 */
void Timers::addDuration(DEF_TIMERS i__def_timer, long i__duration)
{
  if (i__def_timer >= 0 && i__def_timer < NBR_OF_TIMERS && timer[i__def_timer].in_use == true) {
    long l__new_duration = (timer[i__def_timer].duration + i__duration);

    Serial.printf("Timers::addDuration(#%d-%s): [%ld + %ld] -> [%ld]\n",
      i__def_timer, getText(i__def_timer),
      timer[i__def_timer].duration, i__duration, l__new_duration);

    timer[i__def_timer].duration = l__new_duration;
  }
  else {
    Serial.printf("Warning: Timers::addDuration(#%d-%s): Unknown or not in use\n", i__def_timer, getText(i__def_timer));
  }
}

const char *Timers::getText(DEF_TIMERS i__def_timer)
{
  if (i__def_timer >= 0 && i__def_timer < NBR_OF_TIMERS) {
    if (i__def_timer == g__st_timers[i__def_timer].idx) {
      return g__st_timers[i__def_timer].text;
    }
  }

  return "Unknown";
}

