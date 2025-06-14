// $Id: SDCard.cpp,v 1.11 2025/06/14 15:32:55 administrateur Exp $

#if USE_SIMULATION
#include "ArduinoTypes.h"
#include "Arduino.h"

#include "Serial.h"
#include "String.h"
#include "SDCardSimu.h"
#else
#include <Arduino.h>
#endif

#include "Misc.h"
#include "Timers.h"
#include "DateTime.h"
#include "Menus.h"
#include "GestionLCD.h"
#include "SDCard.h"

#if !USE_SIMULATION
SPIClass spi1(HSPI); // Use the HSPI bus
#else
#define SDCARD_SIMU_LOCALIZATION     "./SDCARD"    // Localisation de la SDCard simulee
#define SDCARD_SIMU_SIZE             8000000L      // Taille de la SDCard simulee
#endif

#if 0
void callback_activate_sdcard()
{
  g__sdcard->callback_sdcard_retry_init_more();

  g__sdcard->appendFile(NAME_OF_FILE_GPS_FRAMES, "\n\n### New recording ###\n");
}
#endif

SDCard::SDCard() : flg_init(false), nbr_of_init_retry(0),
                   cardType(CARD_NONE), cardSize((uint64_t)-1),
                   flg_sdcard_in_use(false), flg_inh_append_gps_frame(false),
                   gps_frame_size(0), gps_frame_nbr_records(0),
                   m__filename_frames("")
{
  Serial.println("SDCard::SDCard()");
}

SDCard::~SDCard()
{
  Serial.println("SDCard::~SDCard()");
}

/* Allumage Led en debut d'activite
 * => Car non retour dans 'loop()' pendant le temps de traitement de la methode ;-)
 */
void SDCard::startActivity()
{
  flg_sdcard_in_use = true;

#if !USE_SIMULATION
  digitalWrite(STATE_LED_YELLOW, LOW);
#endif

  g__state_leds |= STATE_LED_YELLOW;
  g__gestion_lcd->Paint_DrawSymbol(LIGHTS_POSITION_SDC_YELLOW, LIGHTS_POSITION_Y, LIGHT_FULL_IDX, &Font16Symbols, BLACK, YELLOW);
}

/* 1 - Armement timer 'TIMER_SDCARD_ACCES' en fin d'activite
 *     => Extinction Led a l'expiration du timer (garantie d'une presentation minimale)
 * 2 - Allumage Led RED (via 'g__state_leds') + armement timer 'TIMER_SDCARD_ERROR' en fin d'activite si error
 *     => Extinction Led RED a l'expiration du timer (garantie d'une presentation minimale)
 */
void SDCard::stopActivity(boolean i__flg_no_error)
{
  g__timers->start(TIMER_SDCARD_ACCES, DURATION_TIMER_SDCARD_ACCES, &callback_end_sdcard_acces);

  if (i__flg_no_error == false) {
    g__state_leds |= STATE_LED_RED;
    g__timers->start(TIMER_SDCARD_ERROR, DURATION_TIMER_SDCARD_ERROR, &callback_end_sdcard_error);
    g__gestion_lcd->Paint_DrawSymbol(LIGHTS_POSITION_SDC_RED, LIGHTS_POSITION_Y, LIGHT_FULL_IDX, &Font16Symbols, BLACK, RED);
  }

  flg_sdcard_in_use = false;
}

bool SDCard::init()
{
  bool l__flg_rtn = false;

  startActivity();

#if !USE_SIMULATION
  spi1.begin(SPI1_SCK, SPI1_MISO, SPI1_MOSI, SPI1_SS);
  spi1.setClockDivider(SPI_CLOCK_DIV2);
#endif

#if !USE_SIMULATION
  if (!SD.begin(SPI1_SS, spi1))
#else
  if (!SD.begin(SDCARD_SIMU_LOCALIZATION, SDCARD_SIMU_SIZE))
#endif
  {
    Serial.printf("error SDCard::init(): SD Card mount failed\n");

    g__state_leds |= STATE_LED_RED;
    g__gestion_lcd->Paint_DrawSymbol(LIGHTS_POSITION_SDC_RED, LIGHTS_POSITION_Y, LIGHT_FULL_IDX, &Font16Symbols, BLACK, RED);

    g__timers->start(TIMER_SDCARD_ACCES, DURATION_TIMER_SDCARD_ACCES, &callback_end_sdcard_acces);
  }
  else {
    flg_init = true;
    l__flg_rtn = true;

#if USE_SIMULATION
    Serial.printf("%s(): SD Card mount successful: [%s] [%u] bytes\n", __FUNCTION__, SDCARD_SIMU_LOCALIZATION, SDCARD_SIMU_SIZE);
#endif
  
    // Test de contenu + tri
    printf("%s(): List dir of [%s]\n", __FUNCTION__, "/EPOWER");
    listDir("/EPOWER");

    g__gestion_lcd->Paint_DrawSymbol(LIGHTS_POSITION_SDC_GREEN, LIGHTS_POSITION_Y, LIGHT_FULL_IDX, &Font16Symbols, BLACK, GREEN);

    stopActivity(l__flg_rtn);
  }

  return l__flg_rtn;
}

void SDCard::callback_sdcard_retry_init_more()
{
  if (init() == true) {
    flg_inh_append_gps_frame = false;

    Serial.printf("SDCard initialized\n");

    if (g__timers->isInUse(TIMER_SDCARD_RETRY_INIT)) {
      g__timers->stop(TIMER_SDCARD_RETRY_INIT);
    }

    //  Preparation de la SDCard
    preparing();
  }
  else {
    end();

    if (nbr_of_init_retry > 0) {
      g__timers->start(TIMER_SDCARD_RETRY_INIT, DURATION_TIMER_SDCARD_RETRY_INIT, &callback_sdcard_retry_init);
      nbr_of_init_retry--;

      Serial.printf("\t#Initialization of SDCard (retry #%d)...\n", nbr_of_init_retry);
    }
    else {
      // Clignotement permanent de la Led RED
      g__timers->start(TIMER_SDCARD_INIT_ERROR, DURATION_TIMER_SDCARD_INIT_ERROR, &callback_sdcard_init_error);

      Serial.println("Initialization of SDCard (Fatal error)");
    }
  }
}

void SDCard::init(int i__nbr_retry)
{
  nbr_of_init_retry = i__nbr_retry;
  g__timers->start(TIMER_SDCARD_RETRY_INIT, DURATION_TIMER_SDCARD_RETRY_INIT, &callback_sdcard_retry_init);
}

void SDCard::end()
{
  startActivity();
  SD.end();
  stopActivity();
}

/*  Preparation de la SDCard
 *  - Renommage du fichier 'GpsFrames.txt' avec la datation de la 1st trame GPS
 *    => GPS [AA130910.000B1AC94847.0719D1NEA00157.5195F1EG40.11s40.20H5239.1I6040922K5124.1L1Ma212*3042] (Cks [0x42] Ok)
 *              => Extraction du type 'I' suivi de DDMMYY (ie. 040922)         ^^^^^^ -> 20220904 (YYYMMDD)
 *              => 'GpsFrames.txt' -> 'GpsFrames-20220904-1.txt' (1st suffixe -[0-9]* disponible - fichier inexistant) 
 */
bool SDCard::preparing()
{
  Serial.println("Preparing of SDCard...");

  bool l__flg_rtn = false;

  return l__flg_rtn;
}

size_t SDCard::sizeFile(const char *i__file_name)
{
  if (flg_init == false) {
    return true;
  }

  // Operation non reussie a priori
  bool   l__flg_ope = false;
  size_t l__size = -1;

  startActivity();

  File file = SD.open(i__file_name);

  if (!file) {
    // Failed to open file for appending
    Serial.printf("error SDCard::sizeFile(%s): Failed to open file\n", i__file_name);
  }
  else {
    l__size = file.size();
    file.close();

    l__flg_ope = true;
  }

  stopActivity(l__flg_ope);

  return l__size;
}

/* Ouverture, concatenation et fermeture dans le fichier 'NAME_OF_FILE_GPS_FRAMES'
 */
bool SDCard::appendGpsFrame(const char *i__frame, boolean i__flg_force_append)
{
  /* Pour le test sur les longueurs ecrites a l'appel precedent
   * et fait avant une nouvelle concatenation
   * => Remarque: La longueur n'est pas maj apres de la concatenation 
   *              => Evite de reouvrir le fichier nouvellement ecrit ;-)
  */
  static size_t g__size_pre   = (size_t)-1;
  static size_t g__size_frame = (size_t)0;

  String l__filename_frame = "";
  l__filename_frame.concat(FRAMES_DIRECTORY);
  l__filename_frame.concat(m__filename_frames.c_str());

  Serial.printf("%s(): [%s] Entering...\n", __FUNCTION__, l__filename_frame.c_str());

  if (flg_init == false || flg_inh_append_gps_frame == true
   || (g__flg_inh_sdcard_ope == true && i__flg_force_append == false)) {
    return true;
  }

  bool l__flg_rtn = true;   // Operation reussie a priori

  startActivity();

  File file = SD.open(l__filename_frame.c_str(), FILE_APPEND);

  if (!file) {
    // Failed to open file for appending
#if 1
    Serial.printf("SDCard::appendGpsFrame(): [%s] Failed to open file for appending\n", l__filename_frame.c_str());
#endif

    l__flg_rtn = false;
  }
  else {
    size_t l__size = file.size();

    if (g__size_pre != (size_t)-1) {
      if (l__size == (g__size_pre + g__size_frame)) {
#if 1
        // Trace de l'operation
        Serial.printf("SDCard::appendGpsFrame(): [%s] Write length: %d = (%d + %d) bytes\n",
          l__filename_frame.c_str(), l__size, g__size_pre, g__size_frame);
#endif
      }
      else if (gps_frame_nbr_records >= 2) {
        /* TODO: => Trace "SDCard::appendGpsFrame(): #1: [1852404375] bytes" ?!..
                 => Trace "error SDCard::appendGpsFrame(): [/EPOWER/EPower-01080000-0000.txt] Error length: 50 != (1852404325 + 50) bytes"

           Ensuite Ok:
                 => Trace "SDCard::appendGpsFrame(): [/EPOWER/EPower-01080000-0000.txt] Write length: 812 = (558 + 254) bytes"
        */
#if 1
        // Trace de l'erreur
        Serial.printf("error SDCard::appendGpsFrame(): [%s] Error length: %d != (%d + %d) bytes\n",
          l__filename_frame.c_str(), l__size, g__size_pre, g__size_frame);
#endif

        l__flg_rtn = false;
      }
      else {
        Serial.printf("SDCard::appendGpsFrame(): [%s] No test of size (#%u record)\n",
          l__filename_frame.c_str(), gps_frame_nbr_records);
      }
    }

    g__size_pre = l__size;                        // Longueur du fichier avant maj
    g__size_frame = strlen(i__frame);   // Longueur a ecrire sans le '\0' terminal ;-)

    if (!file.print(i__frame)) {
      // Line not appended

      // Trace de l'ecriture
      Serial.printf("error SDCard::appendGpsFrame(): Line not appended\n");

      l__flg_rtn = false;
    }
    else {
      gps_frame_size = (l__size + strlen(i__frame));
      gps_frame_nbr_records++;

#if 1
      // Trace de l'ecriture
      Serial.printf("SDCard::appendGpsFrame(): #%u: [%u] bytes\n",
        gps_frame_nbr_records, gps_frame_size);
#endif
    }

    file.close();
  }

  stopActivity(l__flg_rtn);

  return l__flg_rtn;
}

// Methods for tests
bool SDCard::printInfos()
{
  bool l__flg_rtn = false;

  startActivity();

  if (flg_init == false) {
    Serial.println("error SDCard::init(): Error: SD Card not initialized");
  }
  else {
    cardType = SD.cardType();

    if (cardType == CARD_NONE) {
      Serial.println("error SDCard::init(): Error: No SD card attached");
    }
    else {
      Serial.print("SD Card Type: ");
      switch (cardType) {
      default: Serial.println("UNKNOWN");
        break;
      case CARD_MMC:
        Serial.println("MMC");
        l__flg_rtn = true;
        break;
      case CARD_SD:
        Serial.println("SDSC");
        l__flg_rtn = true;
        break;
      case CARD_SDHC:
        Serial.println("SDHC");
        l__flg_rtn = true;
        break;
      }
    }

    cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %llu MBytes\n", cardSize);
  }

  stopActivity(l__flg_rtn);

  return l__flg_rtn;
}

/*   Liste des repertoires et fichiers
   + Determination du prochain fichier 'undated_file' non date de la forme '/EPOWER/EPower-NNNN00DD-0000.txt'

   => TODO: Si dernier fichier '/EPOWER/EPower-19990000-0000.txt' trouve
            => Passer en erreur et pas d'ecriture dans 'undated_file' ;-)
 */
bool SDCard::listDir(const char *i__dir)
{
  bool l__flg_rtn = false;
  String l__last_undated_file = UNDATED_FILE_PATTERN;

  startActivity();

  File root = SD.open(i__dir);
  if (!i__dir) {
    // Failed to open directory
     Serial.printf("error SDCard::listDir(NULL): Failed to open directory\n");
  }
  else {
    if (!root.isDirectory()) {
      // Not a directory
      Serial.printf("error SDCard::listDir(%s): Not a directory\n", i__dir);
    }
    else {
      char l__pattern_years[4+1];
      memset(l__pattern_years, '\0', sizeof(l__pattern_years));

      File file = root.openNextFile();

      while (file) {
        if (file.isDirectory()) {
          Serial.print("  Dir:  [");
          Serial.print(file.name());
          Serial.print("]\n");

#if USE_SIMULATION
          // Pas de sortie si repertoire ".", ".." ou "CVS" trouve
          if (strcmp(file.name(), ".") && strcmp(file.name(), "..") && strcmp(file.name(), "CVS")) {
            printf("  Dir:  [%s]\n", file.name());
          }
#endif
        }
        else {
          Serial.print("  File: [");
          Serial.print(file.name());
          Serial.print("] Size: [");
          Serial.print(file.size());
          Serial.print("]\n");

#if USE_SIMULATION
          printf("  File: [%s] Size [%lu]\n", file.name(), file.size());
#endif
          // Dernier fichier non date
          char l__pattern_months[2+1];
          memset(l__pattern_months, '\0', sizeof(l__pattern_months));
          strncpy(l__pattern_months, (file.name() + strlen(UNDATED_FILE_PATTERN_MONTHS)), 2);

          if (!strcmp(l__pattern_months, "00") && strcmp(file.name(), l__last_undated_file.c_str()) > 0) {
            strncpy(l__pattern_years, (file.name() + strlen(UNDATED_FILE_PATTERN_YEARS)), 4);

#if 0 //USE_SIMULATION
            printf("-> [%s] > [%s] (Year [%04u])\n",
              file.name(), l__last_undated_file.c_str(), (int)strtol(l__pattern_years, NULL, 10));
#endif
            l__last_undated_file = file.name();
          }
        }

        file = root.openNextFile();
      }

      if (m__filename_frames.isEmpty()) {
        char *l__new_last_undated_file = strdup(l__last_undated_file.c_str());
        sprintf(l__new_last_undated_file, "%s%04u0000-0000.txt",
          UNDATED_FILE_PATTERN_YEARS, ((int)strtol(l__pattern_years, NULL, 10)) + 1);

        m__filename_frames = l__new_last_undated_file;
        free(l__new_last_undated_file);

        Serial.printf("  New Last Undated File: [%s]\n", m__filename_frames.c_str());

#if USE_SIMULATION
        printf("  New Last Undated File: [%s]\n", m__filename_frames.c_str());
#endif

        // Marquage du nouveau fichier non date...
        if (getInhAppendGpsFrame() == false) {
          String l__text = "";

          l__text.concat("#File [");
          l__text.concat(m__filename_frames);
          l__text.concat("]\n");
          l__text.concat("#Start Recording\n");

          appendGpsFrame(l__text);
        }
        // Fin: Marquage du nouveau fichier non date...
      }
      else {
        Serial.printf("  Last Undated File: [%s]\n", m__filename_frames.c_str());

#if USE_SIMULATION
        printf("  Last Undated File: [%s]\n", m__filename_frames.c_str());
#endif
      }

      l__flg_rtn = true;
    }
  }

  stopActivity(l__flg_rtn);

  return l__flg_rtn;
}

bool SDCard::exists(const char *i__path)
{
  bool l__flg_rtn = false;

  startActivity();

  l__flg_rtn = SD.exists(i__path);

  stopActivity();

  return l__flg_rtn;
}

bool SDCard::readFile(const char *i__file)
{
  bool l__flg_rtn = false;

  startActivity();

  File file = SD.open(i__file);

  if (!file) {
    // Failed to open file for reading
    Serial.printf("SDCard::readFile(%s): Failed to open file for reading\n", i__file);
  }
  else {
    while (file.available()) {
      int l__value = file.read();
      Serial.write(l__value);
    }

    file.close();

    l__flg_rtn = true;
  }

  stopActivity(l__flg_rtn);

  return l__flg_rtn;
}

bool SDCard::getFileLine(const char *i__file, String &o__line, bool i__flg_close)
{
  /* Non initialization ('open' retourne une classe 'File' !..)
   * => TODO: Accueillir une structure avec un attribut 'flg_available'
   *    => Permettra d'ouvrir plusieurs fichiers simultanement et utiliser le 'flush' / 'close'
   */
  static File g__file_line;

  bool l__flg_rtn = false;

  startActivity();

  if (i__file != NULL) {
    g__file_line = SD.open(i__file);

    if (!g__file_line)
    {
      // Failed to open file for reading line
      Serial.printf("SDCard::getFileLine(%s): Failed to open file for reading line\n", i__file);
    }
  }

  if (g__file_line)
  {
    while (g__file_line.available()) {
      int l__value = g__file_line.read();

      if (l__value != '\r') {
        if (l__value != '\n') {
          o__line.concat((char)l__value);
        }
        else {
          l__flg_rtn = true;    // Retour 'Ok' si la ligne est effectivement lue
          break;
        }
      }
    }

    if (i__flg_close == true) {
      g__file_line.close();
    }
  }

  stopActivity(l__flg_rtn);

  return l__flg_rtn;
}

size_t SDCard::readFileLine(const char *i__file, String &o__line, bool i__flg_close)
{
  /* Non initialization ('open' retourne une classe 'File' !..)
   * => TODO: Accueillir une structure avec un attribut 'flg_available'
   *    => Permettra d'ouvrir plusieurs fichiers simultanement et utiliser le 'flush' / 'close'
   */

  static File g__file_line;

#if USE_SIMULATION
  Serial.printf("SDCard::readFileLine([%s], ..., [%d])", i__file, i__flg_close);
#endif

  bool l__flg_rtn = false;
  size_t l__size = 0;

  startActivity();

  if (i__file != NULL) {
    g__file_line = SD.open(i__file);

    if (!g__file_line) {
      // Failed to open file for reading line
      Serial.printf("error SDCard::readFileLine(%s): Failed to open file for reading line\n", i__file);
    }
  }

  if (g__file_line)
  {
    if (i__flg_close == true) {
      g__file_line.close();
      l__flg_rtn = true;
    }
    else {
      while (g__file_line.available()) {
        int l__value = g__file_line.read();

        if (l__value != '\n') {
          o__line.concat((char)l__value);
          l__size += 1;
        }
        else {
          l__flg_rtn = true;    // La ligne complete est effectivement lue
          break;
        }
      }
    }
  }

  stopActivity(l__flg_rtn);

  return l__size;
}

/* Ouverture, concatenation et fermeture dans le fichier passe en argument
 */
bool SDCard::appendFile(const char *i__file, const char *i__line)
{
  bool l__flg_rtn = false;

  startActivity();

  File file = SD.open(i__file, FILE_APPEND);

  if (!file) {
    // Failed to open file for appending
    Serial.printf("error: SDCard::appendFile(%s): Failed to open file for appending [%s]\n",
      i__file, i__line);
  }
  else {
    if (file.print(i__line)) {
      // Line appended
      l__flg_rtn = true;
    }

    file.flush();        // TBC: A tester sans le 'close()' ;-)
    file.close();
  }

  stopActivity(l__flg_rtn);

  return l__flg_rtn;
}

bool SDCard::renameFile(const char *i__path_from, const char *i__path_to)
{
  bool l__flg_rtn = false;

  startActivity();

  if (SD.rename(i__path_from, i__path_to)) {
    l__flg_rtn = true;
  }
  else {
    Serial.printf("SDCard::renameFile([%s], [%s]): Failed\n", i__path_from, i__path_to);
  }

  stopActivity(l__flg_rtn);

  return l__flg_rtn;
}

bool SDCard::deleteFile(const char *i__path)
{
  bool l__flg_rtn = false;

  startActivity();

  if (SD.remove(i__path)) {
    l__flg_rtn = true;
  }
  else {
    Serial.printf("SDCard::deleteFile(%s): Failed\n", i__path);
  }

  stopActivity(l__flg_rtn);

  return l__flg_rtn;
}
// End: Methods for tests

/* Formatage d'une taille; ie:
   -     56 b (chgt 1 byte)
   -   1.8 Kb (chgt 100 bytes -> ~ trame TLV)
   - 750.5 Kb (chgt 100 bytes -> ~ trame TLV)
   - 2.567 Mb (chgt  1 Kbytes -> ~ 10 trames TLV)
   - >= 10 Mb -> Invite a changer de fichier ;-)
                 => Ne se produira pas lorsque le nom du fichier 'GpsFrames.txt'
                    sera estampllle comme 'GpsFrames-20250210.txt'
*/
void SDCard::formatSize(size_t i__value, char *o__buffer) const
{
#define SIZE_1_KILO_BYTE        1000
#define SIZE_1_MEGA_BYTE     1000000
#define SIZE_10_MEGA_BYTE   10000000

  if (i__value < SIZE_10_MEGA_BYTE) {
    if (i__value < SIZE_1_KILO_BYTE) {
#if !USE_SIMULATION
	sprintf(o__buffer, "%3u b", i__value);
#else
	sprintf(o__buffer, "%3lu b", i__value);
#endif
    }
    else if (i__value < SIZE_1_MEGA_BYTE) {
#if !USE_SIMULATION
	sprintf(o__buffer, "%3u.%u Kb", (i__value / SIZE_1_KILO_BYTE), (i__value % SIZE_1_KILO_BYTE) / 100);
#else
	sprintf(o__buffer, "%3lu.%lu Kb", (i__value / SIZE_1_KILO_BYTE), (i__value % SIZE_1_KILO_BYTE) / 100);
#endif
    }
    else {
#if !USE_SIMULATION
	sprintf(o__buffer, "%u.%03u Mb", (i__value / SIZE_1_MEGA_BYTE), (i__value % SIZE_1_MEGA_BYTE) / 1000);
#else
	sprintf(o__buffer, "%lu.%03lu Mb", (i__value / SIZE_1_MEGA_BYTE), (i__value % SIZE_1_MEGA_BYTE) / 1000);
#endif
    }
  }
  else {
    sprintf(o__buffer, ">= 10 Mb");
  }
}
