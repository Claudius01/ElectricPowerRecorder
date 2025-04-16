// $Id: Timers.h,v 1.5 2025/04/13 17:25:15 administrateur Exp $

#ifndef __TIMERS__
#define __TIMERS__

#if !USE_SIMULATION
/* La resolution des timers est de 10mS
   => 100L correspond a 100 * 10mS = 1Sec
*/
constexpr long DURATION_TIMER_CHENILLARD        = 12L;          // 120mS
constexpr long DURATION_TIMER_CONNECT           = 10 * 100L;    // 10" before state 'NOT_CONNECTED'

constexpr long DURATION_TIMER_SDCARD_ACCES      = 100L;         // 1"
constexpr long DURATION_TIMER_SDCARD_ERROR      = 100L;         // 1"
constexpr long DURATION_TIMER_SDCARD_RETRY_INIT = 200L;         // 2" 
constexpr long DURATION_TIMER_SDCARD_INIT_ERROR = 100L;         // 1"

constexpr long DURATION_TIMER_NMEA_ACTIVITY = 100L;             // 1"
constexpr long DURATION_TIMER_NMEA_ERROR = 100L;                // 1"

constexpr long DURATION_TIMER_SEND_GPSPILOT = 25L;              // 250mS
constexpr long DURATION_TIMER_ACTIVATE_SDCARD = 100L;           // 1"

constexpr long DURATION_TIMER_WATCHDOG = 100L;  		            // 1"

constexpr long  DURATION_TIMER_CONFIG_RTC_SCOLLING = 200;       // 2"
constexpr long  DURATION_TIMER_CONFIG_RTC_WAIT_ACQ = 300;       // 3"
#else
#define DURATION_TIMER_CHENILLARD          12L          // 120mS
#define DURATION_TIMER_CONNECT             10 * 100L    // 10" before state 'NOT_CONNECTED'

#define DURATION_TIMER_SDCARD_ACCES        100L         // 1"
#define DURATION_TIMER_SDCARD_ERROR        100L         // 1"
#define DURATION_TIMER_SDCARD_RETRY_INIT   200L         // 2" 
#define DURATION_TIMER_SDCARD_INIT_ERROR   100L         // 1"

#define DURATION_TIMER_NMEA_ACTIVITY   100L             // 1"
#define DURATION_TIMER_NMEA_ERROR   100L                // 1"

#define DURATION_TIMER_SEND_GPSPILOT   25L              // 250mS
#define DURATION_TIMER_ACTIVATE_SDCARD   100L           // 1"

#define DURATION_TIMER_WATCHDOG   100L 	 		            // 1"

#define DURATION_TIMER_CONFIG_RTC_SCOLLING    100L       // 1
#define DURATION_TIMER_CONFIG_RTC_WAIT_ACQ    300L       // 3"
#endif

typedef enum {
  TIMER_CHENILLARD = 0,         // Presentation sur la Led
  TIMER_CONNECT,                // Gestion etats "connecte/non connecte"

  TIMER_SDCARD_ACCES,           // Timer pour l'acces a la SD Card (allumage d'une duree minimun de 'DURATION_TIMER_SDCARD_ACCES')
  TIMER_SDCARD_ERROR,           // Timer pour la presentation de l'erreur d'acces a la SD Card (allumage d'une duree minimun de 'DURATION_TIMER_SDCARD_ERROR')
  TIMER_SDCARD_RETRY_INIT,      // Timer pour les tentatives d'initialisation
  TIMER_SDCARD_INIT_ERROR,      // Timer pour la presentation de l'erreur d'initialisation

  TIMER_NMEA_ACTIVITY,          // Timer pour la presentation de l'emission TLV
  TIMER_NMEA_ERROR,             // Timer pour la presentation des erreurs de reception NMEA

  TIMER_SEND_GPSPILOT,          // Timer pour le cadencement ligne a ligne de 'GpsPilot.txt'
  TIMER_ACTIVATE_SDCARD,        // Timer pour l'activation de la SDCard apres l'emission de 'GpsPilot.txt'

  TIMER_CONFIG_RTC_SCOLLING,    // Timer pour le defilement des valeurs de configuration RTC a acquerir
  TIMER_CONFIG_RTC_WAIT_ACQ,    // Timer pour l'attente de fin d'acquisition des valeurs de configuration RTC a acquerir

#if USE_SIMULATION
  TIMER_WATCHDOG,			        // Timer pour le chien de garde en simulation
#endif

  NBR_OF_TIMERS                 // Numbers of timers defined
} DEF_TIMERS;

typedef struct {
  DEF_TIMERS      idx;
  bool            flg_trace;                // Trace Yes/No: 0/1
  const char      *text;
} ST_TIMERS;

typedef struct {
  bool            in_use;
  long            duration_init;            // Value of duration at the initialization (allows to calculate the current duration) 
  long            duration;                 // Duration in 10 mS and -1L for not armed
  void            (*fct_callback)(void);    // Callback function without argument and no return
} ST_TIMER;

class Timers {
  private:
    ST_TIMER      timer[NBR_OF_TIMERS];
    long          prompts_duration;         // Duration of prompts for 'TIMER_SHORT_FAMINE_FIFO_TX_PLAY'
    size_t        num_prompt_played;        // Numéro du prompt joué sous la contrainte de 'TIMER_SHORT_FAMINE_FIFO_TX_PLAY'
    long          prompts_duration_total;   // Total duration of prompts for 'TIMER_SHORT_FAMINE_FIFO_TX_PLAY'

  public:
    Timers();
    ~Timers();

    void      update();
    void      test();

    bool    isInUse(DEF_TIMERS i__def_timer);
    bool    start(DEF_TIMERS i__def_timer, long i__duration, void (*i__fct_callback)(void));
    bool    restart(DEF_TIMERS i__def_timer, long i__duration);
    long    getDuration(DEF_TIMERS i__def_timer);
    void    addDuration(DEF_TIMERS i__def_timer, long i__duration);

    const char *getText(DEF_TIMERS i__def_timer);

    bool    stop(DEF_TIMERS i__def_timer);
};

extern Timers                  *g__timers;
#endif
