// $Id: AnalogRead.cpp,v 1.25 2025/05/31 15:51:56 administrateur Exp $

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
#include "Menus.h"
#include "AnalogRead.h"
#include "GestionLCD.h"

#define USE_TRACE_ANALOG      0

void callback_analog_acq()
{
  g__gestion_lcd->Paint_DrawSymbol(LIGHTS_POSITION_ACQ_GRAY, LIGHTS_POSITION_Y, LIGHT_BORD_IDX, &Font16Symbols, BLACK, GRAY);
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
void AnalogRead::resetMinMaxValues()
{
  m__st_values_current.analogVoltsValue_min =
  m__st_values_current.analogVoltsValue_max = m__st_values_current.analogVoltsValue;

  /* Reinitialisation des calculs de moyenne des valeurs (courante, min, avg et max) dans les cas de:
     - Reset des valeurs Min/Max (via le menu)
     - Changement de la periode de glissement (via le menu)
  */
  m__samples = 0L;
  m__st_values_curves.nbr_samples = 0L;

  /* Marquage de la reinitialisation des valeurs Min/max
     => Partie haute en RED
     => Partie basse en WHITE
   */
  g__gestion_lcd->Paint_DrawLine(231, 65, 231, 81, RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID, true);
  g__gestion_lcd->Paint_DrawLine(231, 83, 231, 98, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID, true);

  // Decalage et raffraichissement de l'ecran virtuel
 
  g__gestion_lcd->Paint_ShiftAndRefreshScreenVirtual(true, true);

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
  g__gestion_lcd->Paint_DrawSymbol(LIGHTS_POSITION_ACQ_GRAY, LIGHTS_POSITION_Y, LIGHT_FULL_IDX, &Font16Symbols, BLACK, GRAY);

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
  bool l__flg_watts = (getTypeUnit() == UNIT_WATTS) ? true : false;

  switch (i__type_value) {
  case ANALOG_MIN:
    l__value = m__st_values_current.analogVoltsValue_min;
    if (l__flg_watts == true) {
      l__value = (int)(l__value * RATIO_MILLI_VOLTS_TO_WATTS);
    }
    sprintf(o__buffer, "%4d", l__value);
    break;
  case ANALOG_AVG:
    l__value = m__st_values_current.analogVoltsValue_avg;
    if (l__flg_watts == true) {
      l__value = (int)(l__value * RATIO_MILLI_VOLTS_TO_WATTS);
    }
    sprintf(o__buffer, "%4d", l__value);
    break;
  case ANALOG_MAX:
    l__value = m__st_values_current.analogVoltsValue_max;
    if (l__flg_watts == true) {
      l__value = (int)(l__value * RATIO_MILLI_VOLTS_TO_WATTS);
    }
    sprintf(o__buffer, "%4d", l__value);
    break;
  case ANALOG_SNAPSHOT:
    l__value = m__st_values_current.analogVoltsValue;
    if (l__flg_watts == true) {
      l__value = (int)(l__value * RATIO_MILLI_VOLTS_TO_WATTS);
    }
    sprintf(o__buffer, "%4d", l__value);
    break;
  default:
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
