// $Id: AnalogRead.cpp,v 1.12 2025/05/05 16:31:25 administrateur Exp $

#if USE_SIMULATION
#include "ArduinoTypes.h"
#include "Arduino.h"
#include "Serial.h"

#include <cstdio>
#include <climits>
#else
#include <Arduino.h>
#endif

#include "AnalogRead.h"

#define USE_TRACE_ANALOG      0

AnalogRead::AnalogRead(int i__adc_channel) : m__adc_channel(i__adc_channel),
                           m__samples(0L), m__analogResolution(0), m__valueMax(ANALOG_VALUE_MAX),
                           m__analogVoltsValue_avg_float(0.0)
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

    Serial.printf("New values:\n");

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
  switch (i__type_value) {
  case ANALOG_MIN:
    sprintf(o__buffer, "%4d", m__st_values_current.analogVoltsValue_min);
    break;
  case ANALOG_AVG:
    sprintf(o__buffer, "%4d", m__st_values_current.analogVoltsValue_avg);
    break;
  case ANALOG_MAX:
    sprintf(o__buffer, "%4d", m__st_values_current.analogVoltsValue_max);
    break;
  case ANALOG_SNAPSHOT:
    sprintf(o__buffer, "%4d", m__st_values_current.analogVoltsValue);
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
