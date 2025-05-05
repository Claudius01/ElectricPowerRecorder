// $Id: AnalogRead.h,v 1.9 2025/05/04 13:16:17 administrateur Exp $

#ifndef __ANALOG_READ__
#define __ANALOG_READ__

#define PIN_ADC_CH1    16              // Lecture sur GPIO16
#define PIN_ADC_CH2    17              // Lecture sur GPIO17

#define ANALOG_RESOLUTION     12        // 12 bits (0-4095)
#define ANALOG_VALUE_MAX      3300      // 3.3 Volts

typedef enum {
  ANALOG_MIN = 0,
  ANALOG_AVG,
  ANALOG_MAX,
  ANALOG_SNAPSHOT
} ENUM_ANALOG_VALUES;

typedef struct {
    int analogOriginalValue;
    int analogVoltsValue;

    int analogVoltsValue_min;
    int analogVoltsValue_avg;
    int analogVoltsValue_max;
} ST_ANALOG_VALUES;

typedef struct {
  float                     value;
  int                       position;
} ST_ANALOG_VALUE_CURVE_AVG;

typedef struct {
    unsigned int                nbr_samples;

    ST_ANALOG_VALUE_CURVE_AVG   analogVolts;
    ST_ANALOG_VALUE_CURVE_AVG   analogVolts_min;
    ST_ANALOG_VALUE_CURVE_AVG   analogVolts_avg;
    ST_ANALOG_VALUE_CURVE_AVG   analogVolts_max;
} ST_ANALOG_VALUES_CURVES;

#if USE_SIMULATION
#include "AnalogReadSimu.h"

class AnalogRead : public AnalogReadSimu
#else
class AnalogRead
#endif
{
	private:
    int m__adc_channel;
    unsigned long m__samples;

    int m__analogResolution;
    int m__valueMax;

    ST_ANALOG_VALUES          m__st_values_previous;
    ST_ANALOG_VALUES          m__st_values_current;
    ST_ANALOG_VALUES_CURVES   m__st_values_curves;

    float m__analogVoltsValue_avg_float;

	public:
		AnalogRead(int i__pin_adc_channel);
		~AnalogRead();

    int getResolution() const { return m__analogResolution; };
    int getValueMax() const { return m__valueMax; };

    unsigned long getNbrSamples() const { return m__samples; };

    bool readValue();

    int getValuePrevious(ENUM_ANALOG_VALUES i__type_value) const;
    int getValueCurrent(ENUM_ANALOG_VALUES i__type_value) const;
  
    void formatValuePrevious(ENUM_ANALOG_VALUES i__type_value, char *o__buffer);
    void formatValueCurrent(ENUM_ANALOG_VALUES i__type_value, char *o__buffer);

    void calculOfValuesCurves(bool i__flg_raz_samples);
    void getValuesCurves(ST_ANALOG_VALUES_CURVES *o__analog_values_curves); 
};

extern AnalogRead		*g__analog_read_1;
#endif
