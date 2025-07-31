// $Id: AnalogRead.cpp,v 1.41 2025/07/31 12:53:44 administrateur Exp $

#if USE_SIMULATION
#include "ArduinoTypes.h"
#include "Arduino.h"
#include "Serial.h"

#include <cstdio>
#include <climits>
#else
#include <Arduino.h>
#endif

#include "Timers.h"
#include "DateTime.h"
#include "Menus.h"
#include "ConfigRTC.h"
#include "GestionLCD.h"
#include "AnalogRead.h"

#if USE_SIMULATION
#include "SDCardSimu.h"
#endif

#include "SDCard.h"

#define USE_TRACE_ANALOG      0

void callback_analog_acq()
{
  g__gestion_lcd->Paint_DrawSymbol(LIGHTS_POSITION_ACQ_GRAY, LIGHTS_POSITION_Y, LIGHT_BORD_IDX, &Font16Symbols, BLACK, GREEN);
}

void callback_conso_flash()
{
  UWORD l__color = (g__config_rtc->isDone() == true) ?
    ((g__date_time->getRtcSecInDayLocal() >= BEGIN_OF_OFF_PEAK_HOURS && g__date_time->getRtcSecInDayLocal() < END_OF_OFF_PEAK_HOURS) ? WHITE : DARKGRAY) : YELLOW;

  g__gestion_lcd->Paint_DrawSymbol(225, 27, LIGHT_BORD_IDX, &Font16Symbols, BLACK, l__color);
}

void callback_conso_period()
{
  float l__conso_watts_hour = g__analog_read_1->getConsoCurrentHour();

  if (l__conso_watts_hour > 0.01) {
    unsigned long l__duration_conso_period = (unsigned long)((1 / l__conso_watts_hour) * 100L);  // Duree en x 10mS

    g__timers->start(TIMER_CONSO_PERIOD, l__duration_conso_period, &callback_conso_period);
    g__timers->start(TIMER_CONSO_FLASH, DURATION_TIMER_CONSO_FLASH, &callback_conso_flash);

  UWORD l__color = (g__config_rtc->isDone() == true) ?
    ((g__date_time->getRtcSecInDayLocal() >= BEGIN_OF_OFF_PEAK_HOURS && g__date_time->getRtcSecInDayLocal() < END_OF_OFF_PEAK_HOURS) ? WHITE : DARKGRAY) : YELLOW;

    g__gestion_lcd->Paint_DrawSymbol(225, 27, LIGHT_FULL_IDX, &Font16Symbols, BLACK, l__color);

#if USE_CONSO_METHOD_NEW
    // Comptabilisation des consommations a chaque armement du timer 'TIMER_CONSO_PERIOD'
    g__analog_read_1->incConsomations();
#endif

    if (g__analog_read_1->getTypeUnit() == UNIT_WATTS_HOUR) {
      g__analog_read_1->drawConsommations(29);
    }
  }
  else {
    g__gestion_lcd->Paint_DrawSymbol(225, 27, LIGHT_BORD_IDX, &Font16Symbols, BLACK, YELLOW);
  }
}

AnalogRead::AnalogRead(int i__adc_channel) : m__adc_channel(i__adc_channel),
                           m__samples(0L), m__analogResolution(0), m__valueMax(ANALOG_VALUE_MAX),
                           m__analogVoltsValue_avg_float(0.0), m__type_unit(UNIT_MILLI_VOLTS)
{
  Serial.printf("AnalogRead::AnalogRead(%d)\n", m__adc_channel);

  memset(&m__st_values_previous, '\0', sizeof(m__st_values_previous));
  memset(&m__st_values_current, '\0', sizeof(m__st_values_current));

  m__st_values_previous.analogVoltsValue_min = INT_MAX;
  m__st_values_current.analogVoltsValue_min  = INT_MAX;

  memset(&m__st_values_curves, '\0', sizeof(ST_ANALOG_VALUES_CURVES));
  m__st_values_curves.analogVolts_min.value    = (float)INT_MAX;
  m__st_values_curves.analogVolts_min.position = ((ANALOG_VALUE_MAX / 100) - 1);
  m__st_values_curves.analogVolts_avg.value    = 0.0;
  m__st_values_curves.analogVolts_max.value    = 0.0;
  m__st_values_curves.analogVolts_avg.value    = 0.0;

  analogReadResolution(ANALOG_RESOLUTION);              // Set ADC resolution to 12 bits (0-4095)
  m__analogResolution = (1 << ANALOG_RESOLUTION) - 1;

  Serial.printf("   Resolution: [%d] bits (0-%d)\n", ANALOG_RESOLUTION, getResolution());
  Serial.printf("   Value Max.: [%d]\n", getValueMax());

  // Initialisation des consommations
  initConsommations();

  // Initialisation de la trame a ecrire sur la SDCard
  initFrameRecording();
}

AnalogRead::~AnalogRead()
{
  Serial.printf("AnalogRead::~AnalogRead()\n");
}

/* Reinitialisation de l'acquisition
   => TODO: La presentation des valeurs (courante, min, avg et max) dans le bargraphe
            apparaitront apres un changement de la valeur courante
            => Dans la pratique, la valeur courante varie constamment ;-)
*/
void AnalogRead::resetMinMaxValues(bool i__flg_lcd)
{
  m__st_values_current.analogVoltsValue_min =
  m__st_values_current.analogVoltsValue_max = m__st_values_current.analogVoltsValue;

  /* Reinitialisation des calculs de moyenne des valeurs (courante, min, avg et max) dans les cas de:
     - Reset des valeurs Min/Max (via le menu)
     - Changement de la periode de glissement (via le menu)
  */
  m__samples = 0L;
  m__st_values_curves.nbr_samples = 0L;

  if (i__flg_lcd == true) {
    /* Marquage de la reinitialisation des valeurs Min/max
       => Partie haute en RED
       => Partie basse en WHITE
    */
    g__gestion_lcd->Paint_DrawLine(231, 65, 231, 81, RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID, true);
    g__gestion_lcd->Paint_DrawLine(231, 83, 231, 98, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID, true);

    // Decalage et raffraichissement de l'ecran virtuel
    g__gestion_lcd->Paint_ShiftAndRefreshScreenVirtual(true, true);
  }

  // Bargraphe des valeurs min, avg, max et courante
  g__gestion_lcd->Paint_DrawBarGraph(29 + 16, &Font16Symbols, GRAY);
}

/* Lecture de la valeur analogique et calcul du min/moyenne/max

   Retour: - false: Valeurs inchangees
           - true:  Nouvelles valeurs
 */
bool AnalogRead::readValue()
{
  bool l__flg_rtn = false;

  // Conservation des valeurs precedentes pour effaccement de leur presentation
  memcpy(&m__st_values_previous, &m__st_values_current, sizeof(ST_ANALOG_VALUES));

  m__st_values_current.analogOriginalValue = analogRead(m__adc_channel);          // Read the ADC raw value
  m__st_values_current.analogVoltsValue = analogReadMilliVolts(m__adc_channel);   // Read ADC voltage values (millivolt range)

  m__samples++;

  if (m__st_values_current.analogVoltsValue < m__st_values_current.analogVoltsValue_min) {
    m__st_values_current.analogVoltsValue_min = m__st_values_current.analogVoltsValue;
  }
  else if (m__st_values_current.analogVoltsValue > m__st_values_current.analogVoltsValue_max) {
    m__st_values_current.analogVoltsValue_max = m__st_values_current.analogVoltsValue;
  }

  // Calcul de la moyenne en flottant et presentation de la valeur tronquee ;-)
  m__analogVoltsValue_avg_float = ((((m__samples - 1) * m__analogVoltsValue_avg_float) + m__st_values_current.analogVoltsValue) / m__samples);
  m__st_values_current.analogVoltsValue_avg = (int)(m__analogVoltsValue_avg_float + 0.5);

  if (memcmp(&m__st_values_previous, &m__st_values_current, sizeof(ST_ANALOG_VALUES))) {

#if USE_TRACE_ANALOG
    Serial.printf("\n");

    Serial.printf("New values: m__samples [%lu]\n", m__samples);

    Serial.printf("Previous values:\n");
    Serial.printf("#%lu: ADC analog value [%d] (0x%x)\n", m__samples, m__st_values_previous.analogOriginalValue, m__st_values_previous.analogOriginalValue);
    Serial.printf("#%lu: ADC millivolts value [%d] mV (min [%d] avg [%d] max [%d])\n",
      m__samples, m__st_values_previous.analogVoltsValue,
      m__st_values_previous.analogVoltsValue_min, m__st_values_previous.analogVoltsValue_avg, m__st_values_previous.analogVoltsValue_max);

    Serial.printf("Current values:\n");
    Serial.printf("#%lu: ADC analog value [%d] (0x%x)\n", m__samples, m__st_values_current.analogOriginalValue, m__st_values_current.analogOriginalValue);
    Serial.printf("#%lu: ADC millivolts value [%d] mV (min [%d] avg [%d] (%.1f) max [%d])\n",
      m__samples, m__st_values_current.analogVoltsValue,
      m__st_values_current.analogVoltsValue_min, m__st_values_current.analogVoltsValue_avg, m__analogVoltsValue_avg_float, m__st_values_current.analogVoltsValue_max);
#endif

    l__flg_rtn = true;
  }

  // Presentation de l'acquisition
  g__timers->start(TIMER_ANALOG_ACQ, DURATION_TIMER_ANALOG_ACQ, &callback_analog_acq);
  g__gestion_lcd->Paint_DrawSymbol(LIGHTS_POSITION_ACQ_GRAY, LIGHTS_POSITION_Y, LIGHT_FULL_IDX, &Font16Symbols, BLACK, GREEN);

  /* Calcul des consommations des Wh au moyen d'un "flash" de 100mS
     => ie. Flash a 1Hz si la consommation est de 3600 Watts
            => 'm__conso_watts_hour.current_hour' represente la consommation en Wh car 'readValue()' appelee toutes les secondes ;-)
   */
  unsigned int l__watts = (m__st_values_current.analogVoltsValue * RATIO_MILLI_VOLTS_TO_WATTS);
  m__conso_watts_hour.current_hour = (l__watts / 3600.0);

#if USE_TRACE_ANALOG
  if (g__config_rtc->isDone() == true) {
    Serial.printf("%s(): Epoch [%lu]: Conso. [%u] W ([%.2f] -> [%.2f] Wh) -> [%u] Sec (%u x 10mS)\n", __FUNCTION__,
      g__date_time->getRtcSecInDayLocal(),
      l__watts,
      m__conso_watts_hour.current_hour_previous, m__conso_watts_hour.current_hour,
      (unsigned int)(1 / m__conso_watts_hour.current_hour), (unsigned int)((1 / m__conso_watts_hour.current_hour) * 100L));
  }
  else {
    Serial.printf("%s(): Conso. [%u] W ([%.2f] -> [%.2f] Wh) -> [%u] Sec (%u x 10mS)\n", __FUNCTION__,
      l__watts,
      m__conso_watts_hour.current_hour_previous, m__conso_watts_hour.current_hour,
      (unsigned int)(1 / m__conso_watts_hour.current_hour), (unsigned int)((1 / m__conso_watts_hour.current_hour) * 100L));
  }
#endif

  // Presentation des 'Wh'
  float l__conso_watts_hour = m__conso_watts_hour.current_hour;
  bool l__flg_start = false;

  if (l__conso_watts_hour > 0.01) {
    if (g__timers->isInUse(TIMER_CONSO_PERIOD) == false) {
      l__flg_start = true;

#if USE_TRACE_ANALOG
      Serial.printf("   %s(): No timer inuse...\n", __FUNCTION__);
#endif
    }

    // Forcage rearmement timer si les 'Wh' sont superieurs de 20% de la valeur precedente
    if (l__conso_watts_hour > (1.2 * m__conso_watts_hour.current_hour_previous)) {
      if (g__timers->isInUse(TIMER_CONSO_PERIOD) == true) {
        g__timers->stop(TIMER_CONSO_PERIOD);
      }

      l__flg_start = true;

#if USE_TRACE_ANALOG
      Serial.printf("   %s(): Stop timer...\n", __FUNCTION__);
#endif
    }

    m__conso_watts_hour.current_hour_previous = m__conso_watts_hour.current_hour;
  }
  else {
    g__gestion_lcd->Paint_DrawSymbol(225, 27, LIGHT_FULL_IDX, &Font16Symbols, BLACK, WHITE);
  }

  if (l__flg_start == true) {    
    unsigned long l__duration_conso_period = (unsigned long)((1 / l__conso_watts_hour) * 100L);  // Duree en x 10mS

    g__timers->start(TIMER_CONSO_PERIOD, l__duration_conso_period, &callback_conso_period);
    g__timers->start(TIMER_CONSO_FLASH, DURATION_TIMER_CONSO_FLASH, &callback_conso_flash);

    g__gestion_lcd->Paint_DrawSymbol(225, 27, LIGHT_FULL_IDX, &Font16Symbols, BLACK, WHITE);

#if USE_CONSO_METHOD_NEW
    // Comptabilisation des consommations a chaque armement du timer 'TIMER_CONSO_PERIOD'
    incConsomations();
#endif

    if (getTypeUnit() == UNIT_WATTS_HOUR) {
      drawConsommations(29);
    }

#if USE_TRACE_ANALOG
    Serial.printf("   %s(): Start timer...\n", __FUNCTION__);
#endif
  }

#if !USE_CONSO_METHOD_NEW
  if (g__config_rtc->isDone() == true) {
    if (g__date_time->getRtcSecInDayLocal() >= BEGIN_OF_OFF_PEAK_HOURS && g__date_time->getRtcSecInDayLocal() < END_OF_OFF_PEAK_HOURS) {
      // Heures creuses entre '00H00' et '07:59'
      m__conso_watts_hour.off_peak_hours += m__conso_watts_hour.current_hour;
    }
    else {
      // Heures pleines entre '08H00' et '23:59'
      m__conso_watts_hour.full_hours += m__conso_watts_hour.current_hour;
    }
  }

  m__conso_watts_hour.total_hours += m__conso_watts_hour.current_hour;

#if USE_TRACE_ANALOG
  Serial.printf("%s(): (HCreuses [%.1f] + HPleines [%.1f]) = Total [%.1f] Wh\n", __FUNCTION__,
    m__conso_watts_hour.off_peak_hours, m__conso_watts_hour.full_hours, m__conso_watts_hour.total_hours);
#endif
#endif
  // Fin: Calcul des consommations des Wh...

  return l__flg_rtn;
}

int AnalogRead::getValuePrevious(ENUM_ANALOG_VALUES i__type_value) const
{
  switch (i__type_value) {
  case ANALOG_MIN:
    return m__st_values_previous.analogVoltsValue_min;
  case ANALOG_AVG:
    return m__st_values_previous.analogVoltsValue_avg;
  case ANALOG_MAX:
    return m__st_values_previous.analogVoltsValue_max;
  case ANALOG_SNAPSHOT:
    return m__st_values_previous.analogVoltsValue;
  default:
    return 0;
  }
}

void AnalogRead::formatValuePrevious(ENUM_ANALOG_VALUES i__type_value, char *o__buffer)
{
  switch (i__type_value) {
  case ANALOG_MIN:
    sprintf(o__buffer, "%4d", m__st_values_previous.analogVoltsValue_min);
    break;
  case ANALOG_AVG:
    sprintf(o__buffer, "%4d", m__st_values_previous.analogVoltsValue_avg);
    break;
  case ANALOG_MAX:
    sprintf(o__buffer, "%4d", m__st_values_previous.analogVoltsValue_max);
    break;
  case ANALOG_SNAPSHOT:
    sprintf(o__buffer, "%4d", m__st_values_previous.analogVoltsValue);
    break;
  default:
    break;
  }
}

int AnalogRead::getValueCurrent(ENUM_ANALOG_VALUES i__type_value) const
{
  switch (i__type_value) {
  case ANALOG_MIN:
    return m__st_values_current.analogVoltsValue_min;
  case ANALOG_AVG:
    return m__st_values_current.analogVoltsValue_avg;
  case ANALOG_MAX:
    return m__st_values_current.analogVoltsValue_max;
  case ANALOG_SNAPSHOT:
    return m__st_values_current.analogVoltsValue;
  default:
    return 0;
  }
}

void AnalogRead::formatValueCurrent(ENUM_ANALOG_VALUES i__type_value, char *o__buffer)
{
  int l__value = 0;
  ENUM_TYPE_UNIT l__flg_unit = getTypeUnit();

  switch (i__type_value) {
  case ANALOG_MIN:
    l__value = m__st_values_current.analogVoltsValue_min;
    if (l__flg_unit == UNIT_WATTS) {
      l__value = (int)(l__value * RATIO_MILLI_VOLTS_TO_WATTS);
    }
    sprintf(o__buffer, "%4d", l__value);
    break;
  case ANALOG_AVG:
    l__value = m__st_values_current.analogVoltsValue_avg;
    if (l__flg_unit == UNIT_WATTS) {
      l__value = (int)(l__value * RATIO_MILLI_VOLTS_TO_WATTS);
    }
    sprintf(o__buffer, "%4d", l__value);
    break;
  case ANALOG_MAX:
    l__value = m__st_values_current.analogVoltsValue_max;
    if (l__flg_unit == UNIT_WATTS) {
      l__value = (int)(l__value * RATIO_MILLI_VOLTS_TO_WATTS);
    }
    sprintf(o__buffer, "%4d", l__value);
    break;
  case ANALOG_SNAPSHOT:
    l__value = m__st_values_current.analogVoltsValue;
    if (l__flg_unit == UNIT_WATTS) {
      l__value = (int)(l__value * RATIO_MILLI_VOLTS_TO_WATTS);
    }
    sprintf(o__buffer, "%4d", l__value);
    break;

#if !USE_CONSO_METHOD_NEW
  case ANALOG_CONSO_OFF_PEAK_HOURS:
    l__value = (int)(m__conso_watts_hour.off_peak_hours + 0.5);
    sprintf(o__buffer, "%5d", l__value);
    break;
  case ANALOG_CONSO_FULL_HOURS:
    l__value = (int)(m__conso_watts_hour.full_hours + 0.5);
    sprintf(o__buffer, "%5d", l__value);
    break;
  case ANALOG_CONSO_TOTAL_HOURS:
    l__value = (int)(m__conso_watts_hour.total_hours + 0.5);
    sprintf(o__buffer, "%5d", l__value);
    break;
#else
  case ANALOG_CONSO_OFF_PEAK_HOURS:
    l__value = (int)m__conso_watts_hour.new_off_peak_hours;
    sprintf(o__buffer, "%5d", l__value);
    break;
  case ANALOG_CONSO_FULL_HOURS:
    l__value = (int)m__conso_watts_hour.new_full_hours;
    sprintf(o__buffer, "%5d", l__value);
    break;
  case ANALOG_CONSO_TOTAL_HOURS:
    l__value = (int)m__conso_watts_hour.new_total_hours;
    sprintf(o__buffer, "%5d", l__value);
    break;
#endif

  default:
    sprintf(o__buffer, "????");
    break;
  }
}

void AnalogRead::calculOfValuesCurves(bool i__flg_raz_samples)
{
  //Serial.printf("AnalogRead::calculOfValuesCurves(%d)\n", i__flg_raz_samples);

  m__st_values_curves.nbr_samples++;

  // Calcul des valeurs a presenter
  // Calcul de la moyenne des min sur la periode
  m__st_values_curves.analogVolts_min.value =
    ((((m__st_values_curves.nbr_samples - 1) * m__st_values_curves.analogVolts_min.value)
     +  m__st_values_current.analogVoltsValue_min) / m__st_values_curves.nbr_samples);

  // Calcul de la moyenne des moyennes sur la periode
  m__st_values_curves.analogVolts_avg.value =
    ((((m__st_values_curves.nbr_samples - 1) * m__st_values_curves.analogVolts_avg.value)
      + m__st_values_current.analogVoltsValue_avg) / m__st_values_curves.nbr_samples);

  // Calcul de la moyenne des max sur la periode
  m__st_values_curves.analogVolts_max.value =
    ((((m__st_values_curves.nbr_samples - 1) * m__st_values_curves.analogVolts_max.value)
      + m__st_values_current.analogVoltsValue_max) / m__st_values_curves.nbr_samples);

  // Calcul de la moyenne de l'instantane sur la periode
  m__st_values_curves.analogVolts.value =
    ((((m__st_values_curves.nbr_samples - 1) * m__st_values_curves.analogVolts.value)
      + m__st_values_current.analogVoltsValue) / m__st_values_curves.nbr_samples);
  // Fin: Calcul des valeurs a presenter

  if (i__flg_raz_samples == true) {
    /* Remise a 1 du nombre d'echantillons pour tenir compte de la derniere valeur moyennee
       qui n'aura qu'un poids de 1
       => TODO: Pas constate dans la realite ?!..
     */
    m__st_values_curves.nbr_samples = 1;
  }
}

void AnalogRead::getValuesCurves(ST_ANALOG_VALUES_CURVES *o__analog_values_curves)
{
  memcpy(o__analog_values_curves, &m__st_values_curves, sizeof(ST_ANALOG_VALUES_CURVES));
}

void AnalogRead::initConsommations()
{
#if USE_CONSO_METHOD_NEW
  m__conso_watts_hour.new_off_peak_hours = 0;
  m__conso_watts_hour.new_full_hours     = 0;
  m__conso_watts_hour.new_total_hours    = 0;
  m__conso_watts_hour.current_hour       = 0.0;
  m__conso_watts_hour.current_hour_previous = -1.0;
  #else
  m__conso_watts_hour.off_peak_hours = 0.0;
  m__conso_watts_hour.full_hours     = 0.0;
  m__conso_watts_hour.total_hours    = 0.0;
  m__conso_watts_hour.current_hour   = 0.0;
  m__conso_watts_hour.current_hour_previous = -1.0;
#endif
}

#if USE_CONSO_METHOD_NEW
void AnalogRead::incConsomations()
{
  if (g__config_rtc->isDone() == true) {
    if (g__date_time->getRtcSecInDayLocal() >= BEGIN_OF_OFF_PEAK_HOURS && g__date_time->getRtcSecInDayLocal() < END_OF_OFF_PEAK_HOURS) {
      // Heures creuses entre '00H00' et '07:59' comprises
      m__conso_watts_hour.new_off_peak_hours += 1;
    }
    else {
      // Heures pleines entre '08H00' et '23:59'
      m__conso_watts_hour.new_full_hours += 1;
    }
  }

  m__conso_watts_hour.new_total_hours += 1;

#if USE_TRACE_ANALOG
  Serial.printf("%s(): (HCreuses [%u] + HPleines [%u]) = Total [%u] Wh\n", __FUNCTION__,
    m__conso_watts_hour.new_off_peak_hours, m__conso_watts_hour.new_full_hours, m__conso_watts_hour.new_total_hours);
#endif
}
#endif

void AnalogRead::drawConsommations(UWORD i__y)
{
  char l__text_for_lcd[32];
  memset(l__text_for_lcd, '\0', sizeof(l__text_for_lcd));

  formatValueCurrent(ANALOG_CONSO_OFF_PEAK_HOURS, l__text_for_lcd);
  g__gestion_lcd->Paint_DrawString_EN(2, i__y, l__text_for_lcd, &Font16, BLACK, WHITE);

  formatValueCurrent(ANALOG_CONSO_FULL_HOURS, l__text_for_lcd);
  g__gestion_lcd->Paint_DrawString_EN(2 + (11 * 6), i__y, l__text_for_lcd, &Font16, BLACK, DARKGRAY);

  formatValueCurrent(ANALOG_CONSO_TOTAL_HOURS, l__text_for_lcd);
  g__gestion_lcd->Paint_DrawString_EN(2  + (11 * 12), i__y, l__text_for_lcd, &Font16, BLACK, YELLOW);
}

/* Methodes de gestion du buffer d'enregistrement qui sera ecrit sur la SDCard
   => #Commentaires + Codage CSV

19:14:56.033 -> #File [EPower-01100000-0000.txt]
19:14:56.033 -> #Start Recording
19:14:56.033 -> [00:01:00;60;395;375;356;375;393;342;387;352;372;359;350;409;342;387;398;365;359;404;391;366;364;404;385;364;351;396;413;358;370;395;383;343;416;363;352;389;380;350;341;352;488;366;321;396;481;310;422;400;323;489;489;297;466;428;495;476;294;323;372;450]
19:14:56.033 -> [00:02:00;60;296;482;372;422;363;377;437;393;350;409;393;413;353;380;393;372;361;375;366;407;351;387;404;413;344;352;353;391;377;361;345;420;470;395;357;383;386;354;376;409;338;302;464;295;474;400;341;389;480;503;489;297;481;393;333;397;482;501;489;296]
19:14:56.033 -> [00:03:00;60;470;361;307;435;451;496;471;288;489;387;330;386;467;490;496;320;404;483;437;294;352;438;426;410;314;497;494;306;294;329;372;462;311;463;452;385;324;307;359;366;462;299;294;489;501;491;437;294;496;336;296;452;480;496;440;299;491;323;296;476]
...
*/
void AnalogRead::initFrameRecording()
{
  m__frame_recording.flg_in_use  = false;
  m__frame_recording.nbr_records = 0;
  m__frame_recording.hhmmss      = "";
  m__frame_recording.nbr_values  = 0;
  m__frame_recording.values      = "";
}

void AnalogRead::buildFrameRecording(const char *i__text)
{
  m__frame_recording.hhmmss = i__text;

  m__frame_recording.flg_in_use = true;
}

void AnalogRead::buildFrameRecording()
{
  m__frame_recording.nbr_values++;

  // Prise de la valeur courante en Volts
  char l__value[32];
  memset(l__value, '\0', sizeof(l__value));
  sprintf(l__value, ";%u", m__st_values_current.analogVoltsValue);

  m__frame_recording.values.concat(l__value);
}

// Formatage au format CSV
void AnalogRead::writeFrameRecording()
{
  String l__buffer = "[";

  // HH:MM:SS
  l__buffer.concat(m__frame_recording.hhmmss.c_str());

  // Nbr de valeurs
  char l__value[32];
  memset(l__value, '\0', sizeof(l__value));
  sprintf(l__value, ";%u", m__frame_recording.nbr_values);
  l__buffer.concat(l__value);

  // Liste des valeurs
  l__buffer.concat(m__frame_recording.values.c_str());
  l__buffer.concat("]\n");

  Serial.printf("%s(): Write Frame Recording %s", __FUNCTION__, l__buffer.c_str());

#if USE_SIMULATION
  printf("%s", l__buffer.c_str());
#endif

  if (g__sdcard->getInhAppendEPowerFrame() == false) {
    m__frame_recording.nbr_records++;
  }

  g__sdcard->appendEPowerFrame(l__buffer);

  initFrameRecording();
}
// Fin:  Methodes de gestion du buffer d'enregistrement qui sera ecrit sur la SDCard
