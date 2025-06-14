// $Id: AnalogRead.h,v 1.20 2025/06/14 14:09:22 administrateur Exp $

#ifndef __ANALOG_READ__
#define __ANALOG_READ__

#if USE_SIMULATION
#include "String.h"
#endif

#define USE_CONSO_METHOD_NEW      1

#define PIN_ADC_CH1    16              // Lecture sur GPIO16
#define PIN_ADC_CH2    17              // Lecture sur GPIO17

#define ANALOG_RESOLUTION     12        // 12 bits (0-4095)
#define ANALOG_VALUE_MAX      3300      // 3.3 Volts

#define RATIO_MILLI_VOLTS_TO_WATTS    1.5

typedef enum {
  ANALOG_MIN = 0,
  ANALOG_AVG,
  ANALOG_MAX,
  ANALOG_SNAPSHOT,

  ANALOG_CONSO_OFF_PEAK_HOURS,
  ANALOG_CONSO_FULL_HOURS,
  ANALOG_CONSO_TOTAL_HOURS
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

typedef enum {
  UNIT_MILLI_VOLTS = 0,
  UNIT_WATTS,
  UNIT_WATTS_HOUR
} ENUM_TYPE_UNIT;

typedef struct {
#if USE_CONSO_METHOD_NEW
  unsigned int                  new_off_peak_hours;         // Durant les heures creuses
  unsigned int                  new_full_hours;             // Durant les heures pleines
  unsigned int                  new_total_hours;            // Total 'heures creuses' + 'heures pleines'
#else
  float                         off_peak_hours;         // Durant les heures creuses
  float                         full_hours;             // Durant les heures pleines
  float                         total_hours;            // Total 'heures creuses' + 'heures pleines'
#endif
  float                         current_hour;           // Consommation courante
  float                         current_hour_previous;
} ST_CONSOMMATION_WATTS_HOUR;

typedef struct {
  bool                          flg_in_use;
  String                        hhmmss;
  unsigned int                  nbr_values;
  String                        values;
} ST_FRAME_RECORDING;

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

    ST_ANALOG_VALUES            m__st_values_previous;
    ST_ANALOG_VALUES            m__st_values_current;
    ST_ANALOG_VALUES_CURVES     m__st_values_curves;

    float                       m__analogVoltsValue_avg_float;

    ENUM_TYPE_UNIT              m__type_unit;
    ST_CONSOMMATION_WATTS_HOUR  m__conso_watts_hour;

    ST_FRAME_RECORDING          m__frame_recording;       // Enregistrement a ecrire sur le SDCard

	public:
		AnalogRead(int i__pin_adc_channel);
		~AnalogRead();

    void resetMinMaxValues(bool i__flg_lcd = true);

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

    ENUM_TYPE_UNIT getTypeUnit() const { return m__type_unit; };
    void           setTypeUnit(ENUM_TYPE_UNIT i__value) { m__type_unit = i__value; };

    void initConsommations();
    void drawConsommations(UWORD i__y);

#if USE_CONSO_METHOD_NEW
    unsigned int getConsoOffPeakHours() const { return m__conso_watts_hour.new_off_peak_hours; };
    unsigned int getConsoFullHours()    const { return m__conso_watts_hour.new_full_hours; };
    unsigned int getConsoTotalHours()   const { return m__conso_watts_hour.new_total_hours; };
    float        getConsoCurrentHour()  const { return m__conso_watts_hour.current_hour; };

    void incConsomations();
#else
    float getConsoOffPeakHours() const { return m__conso_watts_hour.off_peak_hours; };
    float getConsoFullHours()    const { return m__conso_watts_hour.full_hours; };
    float getConsoTotalHours()   const { return m__conso_watts_hour.total_hours; };
    float getConsoCurrentHour()  const { return m__conso_watts_hour.current_hour; };
#endif

    // Methodes de gestion de la trame d'enregistrement
    void initFrameRecording();
    bool isFrameRecordingInUse() const { return m__frame_recording.flg_in_use; };
    void buildFrameRecording();
    void buildFrameRecording(const char *i__text);
    void writeFrameRecording();
};

extern AnalogRead		*g__analog_read_1;
#endif
