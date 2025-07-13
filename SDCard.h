// $Id: SDCard.h,v 1.12 2025/07/13 14:14:03 administrateur Exp $

#ifndef __SDCARD__
#define __SDCARD__

#if !USE_SIMULATION
/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32-S3-GEEK
 *    D2       38
 *    D3       34(SS)
 *    CMD      35(MOSI)
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      36(SCK)
 *    VSS      GND
 *    D0       37(MISO)
 *    D1       33
 */
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Define the pins of the first SPI bus
#define SPI1_SCK  36
#define SPI1_MISO 37
#define SPI1_MOSI 35
#define SPI1_SS   34
#endif

#define FILE_PATTERN_YEARS            "EPower-"                     // Debut du fichier (pointage sur 'Year')
#define FILE_PATTERN_MONTHS           "EPower-0000"                 // Suite (pointage sur 'Month')
#define FILE_PATTERN                  "EPower-00000000-0000.txt"    // Forme complete et nommage du 1st fichier
#define FRAMES_DIRECTORY              "/EPOWER/"                    // Repertoire des fichiers d'enregistrements (dates et non dates)

#define TEXT_POSITION_PROPERTIES_X    180
#define TEXT_POSITION_PROPERTIES_Y    117
#define TEXT_LENGTH_PROPERTIES         10     // Longueur des proprietes

// Proprietes des fichiers d'enregistrement en cours
typedef struct {
  size_t      test_size_pre;
  size_t      test_frame_size;

  size_t      frame_size;
  uint16_t    frame_nbr_records;
  uint16_t    frame_nbr_errors;
} ST_FILE_PROPERTIES;

#if !USE_SIMULATION
class SDCard
#else
class SDCard : public SD
#endif
{
  private:
    bool        flg_init;
    int         nbr_of_init_retry;
    uint8_t     cardType;
    uint64_t    cardSize;

    bool        flg_sdcard_in_use;
    bool        flg_inh_append_epower_frame;

    int         m__index_filename_not_dated;      // Index des fichiers non dates

    ST_FILE_PROPERTIES    m__file_properties;

    /* Nom du fichier d'enregistrement en cours
       => Celui-ci est change suite:
          - Au passage a minuit avec changement du jour (fichier date et non date)
          - A la configuration du RTC
            => Chgt d'un fichier non date a un fichier date
    */
    String      m__filename_frames;

    // Liste des fichiers d'enregistrements utilises depuis le lancement ;-)
    std::vector<String>    m__file_record_name;

    bool preparing();

#if USE_SIMULATION
    class SD    SD;
#endif

  public:
    SDCard();
    ~SDCard();

    bool init();
    void init(int i__nbr_retry);
    void end();

    void startActivity();
    void stopActivity(boolean i__flg_no_error = true);

    void callback_sdcard_retry_init_more();

    bool isInit()  { return flg_init; };
    bool isInUse() { return flg_sdcard_in_use; };

    // Gestion des trames d'enregistrements
    void setInhAppendEPowerFrame(bool i__flg) { flg_inh_append_epower_frame = i__flg; };
    bool getInhAppendEPowerFrame() const { return flg_inh_append_epower_frame; };

    bool appendEPowerFrame(const char *i__frame, boolean i__flg_force_append = false);
    bool appendEPowerFrame(const String &i__frame, boolean i__flg_force_append = false) { return appendEPowerFrame(i__frame.c_str()); };
    size_t getEPowerFrameSize() const { return m__file_properties.frame_size; };

    void formatSize(size_t i__value, char *o__buffer) const ;
    void formatValue(uint16_t i__value, char *o__buffer) const { sprintf(o__buffer, "#%u", i__value); };
  
    size_t sizeFile(const char *i__file_name);
    uint16_t getNbrRecords() const { return m__file_properties.frame_nbr_records; };

    // Methods for tests
    bool printInfos();   
    bool listDir(const char *i__dir);
    bool exists(const char *i__path);
    bool readFile(const char *i__file);
    bool getFileLine(const char *i__file, String &o__line, bool i__flg_close = false);
    size_t readFileLine(const char *i__file, String &o__line, bool i__flg_close = false);
    bool appendFile(const char *i__file, const char *i__line);
    bool renameFile(const char *i__path_from, const char *i__path_to);
    bool deleteFile(const char *i__path);
    // End: Methods for tests

    void printNameFile();
    void printSizeFile();
    void printNbrRecords();
    void printNbrErrors();

    void resetFileProperties();
    int  getIndexOfFileNameNotDated() const { return m__index_filename_not_dated; };
    void addNewFileRecordName(const char *i__value);
    void printFileRecordName();
};

extern void callback_end_sdcard_acces();
extern void callback_end_sdcard_error();
extern void callback_sdcard_retry_init();
extern void callback_sdcard_init_error();

extern SDCard     *g__sdcard;
extern bool       g__flg_inh_sdcard_ope;

extern void       callback_activate_sdcard();
#endif
