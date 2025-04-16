// $Id: AnalogRead.h,v 1.5 2025/04/16 14:27:47 administrateur Exp $

#ifndef __ANALOG_READ__
#define __ANALOG_READ__

#define PIN_ADC2_CH2    17

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

#if USE_SIMULATION
#include "AnalogReadSimu.h"

class AnalogRead : public AnalogReadSimu
#else
class AnalogRead
#endif
{
	private:
    unsigned long m__samples;

    int m__analogResolution;
    int m__valueMax;

    ST_ANALOG_VALUES    m__st_values_previous;
    ST_ANALOG_VALUES    m__st_values_current;

    float m__analogVoltsValue_avg_float;

	public:
		AnalogRead();
		~AnalogRead();

    int getResolution() const { return m__analogResolution; };
    int getValueMax() const { return m__valueMax; };

    unsigned long getNbrSamples() const { return m__samples; };

    bool readValue();

    int getValuePrevious(ENUM_ANALOG_VALUES i__type_value) const;
    int getValueCurrent(ENUM_ANALOG_VALUES i__type_value) const;
  
    void formatValuePrevious(ENUM_ANALOG_VALUES i__type_value, char *o__buffer);
    void formatValueCurrent(ENUM_ANALOG_VALUES i__type_value, char *o__buffer);
};

extern AnalogRead		*g__analog_read;
#endif
