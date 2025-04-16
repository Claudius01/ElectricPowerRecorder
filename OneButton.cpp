#ident "$Id: OneButton.cpp,v 1.1 2025/03/30 18:09:46 administrateur Exp $"

/**
 * @file OneButton.cpp
 *
 * @brief Library for detecting button clicks, doubleclicks and long press
 * pattern on a single button.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *                          Ihor Nehrutsa, Ihor.Nehrutsa@gmail.com
 *
 * This work is licensed under a BSD style license. See
 * http://www.mathertel.de/License.aspx
 *
 * More information on: https://www.mathertel.de/Arduino/OneButtonLibrary.aspx
 *
 * Changelog: see OneButton.h
 */

// 01/03/2025 Reprise du code pour une compilation en Simulation ;-)

#if USE_SIMULATION
#include "ArduinoTypes.h"
#include "Arduino.h"

#include "Serial.h"
#endif

#include "OneButton.h"

// ----- Initialization and Default Values -----

// Statics initialization
int OneButton::_pin = -1;                 // hardware pin number.
int OneButton::_debounce_ms = 50;         // number of msecs for debounce times.
unsigned int OneButton::_click_ms = 400;  // number of msecs before a click is detected.
unsigned int OneButton::_press_ms = 800;  // number of msecs before a long button press is detected
unsigned int OneButton::_idle_ms = 1000;  // number of msecs before idle is detected

int OneButton::_buttonPressed = 0;  // this is the level of the input pin when the button is pressed.
                  			            // LOW if the button connects the input pin to GND when pressed.
                         			    	// HIGH if the button connects the input pin to VCC when pressed.

// These variables will hold functions acting as event source.
callbackFunction OneButton::_pressFunc = NULL;
parameterizedCallbackFunction OneButton::_paramPressFunc = NULL;
void *OneButton::_pressFuncParam = NULL;

callbackFunction OneButton::_clickFunc = NULL;
parameterizedCallbackFunction OneButton::_paramClickFunc = NULL;
void *OneButton::_clickFuncParam = NULL;

callbackFunction OneButton::_doubleClickFunc = NULL;
parameterizedCallbackFunction OneButton::_paramDoubleClickFunc = NULL;
void *OneButton::_doubleClickFuncParam = NULL;

callbackFunction OneButton::_multiClickFunc = NULL;
parameterizedCallbackFunction OneButton::_paramMultiClickFunc = NULL;
void *OneButton::_multiClickFuncParam = NULL;

callbackFunction OneButton::_longPressStartFunc = NULL;
parameterizedCallbackFunction OneButton::_paramLongPressStartFunc = NULL;
void *OneButton::_longPressStartFuncParam = NULL;

callbackFunction OneButton::_longPressStopFunc = NULL;
parameterizedCallbackFunction OneButton::_paramLongPressStopFunc = NULL;
void *OneButton::_longPressStopFuncParam = NULL;

callbackFunction OneButton::_duringLongPressFunc = NULL;
parameterizedCallbackFunction OneButton::_paramDuringLongPressFunc = NULL;
void *OneButton::_duringLongPressFuncParam = NULL;

callbackFunction OneButton::_idleFunc = NULL;

stateMachine_t OneButton::_state = OCS_INIT;

bool OneButton::_idleState = false;

bool OneButton::debouncedLevel = false;
bool OneButton::_lastDebounceLevel = false;      // used for pin debouncing
unsigned long OneButton::_lastDebounceTime = 0;  // millis()
unsigned long OneButton::now = 0;                // millis()

unsigned long OneButton::_startTime = 0;  // start time of current activeLevel change
int OneButton::_nClicks = 0;              // count the number of clicks with this variable
int OneButton::_maxClicks = 1;            // max number (1, 2, multi=3) of clicks of interest by registration of event functions.

unsigned int OneButton::_long_press_interval_ms = 0;    // interval in msecs between calls of the DuringLongPress event
unsigned long OneButton::_lastDuringLongPressTime = 0;  // used to produce the DuringLongPress interval
// End: Statics initialization

/**
 * @brief Construct a new OneButton object but not (yet) initialize the IO pin.
 */
OneButton::OneButton() {
  _pin = -1;
  // further initialization has moved to OneButton.h

  Serial.println("OneButton::OneButton()");
}

// Initialize the OneButton library.
OneButton::OneButton(const int pin, const bool activeLow, const bool pullupActive)
{
  Serial.printf("OneButton::OneButton(%d, %d, %d)\n", pin, activeLow, pullupActive);

  setup(pin, pullupActive ? INPUT_PULLUP : INPUT, activeLow);
}  // OneButton


// initialize or re-initialize the input pin
void OneButton::setup(const uint8_t pin, const uint8_t mode, const bool activeLow) {
  _pin = pin;

  if (activeLow) {
    // the button connects the input pin to GND when pressed.
    _buttonPressed = LOW;

  } else {
    // the button connects the input pin to VCC when pressed.
    _buttonPressed = HIGH;
  }

#if !USE_SIMULATION
  pinMode(pin, mode);
#endif
}


// explicitly set the number of millisec that have to pass by before a click is assumed stable.
void OneButton::setDebounceMs(const int ms) {
  _debounce_ms = ms;
}  // setDebounceMs


// explicitly set the number of millisec that have to pass by before a click is detected.
void OneButton::setClickMs(const unsigned int ms) {
  _click_ms = ms;
}  // setClickMs


// explicitly set the number of millisec that have to pass by before a long button press is detected.
void OneButton::setPressMs(const unsigned int ms) {
  _press_ms = ms;
}  // setPressMs

// explicitly set the number of millisec that have to pass by before button idle is detected.
void OneButton::setIdleMs(const unsigned int ms) {
  _idle_ms = ms;
}  // setIdleMs

// save function for click event
void OneButton::attachPress(callbackFunction newFunction) {
  _pressFunc = newFunction;
}  // attachPress


// save function for parameterized click event
void OneButton::attachPress(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramPressFunc = newFunction;
  _pressFuncParam = parameter;
}  // attachPress

// save function for click event
void OneButton::attachClick(callbackFunction newFunction) {
  _clickFunc = newFunction;
}  // attachClick


// save function for parameterized click event
void OneButton::attachClick(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramClickFunc = newFunction;
  _clickFuncParam = parameter;
}  // attachClick


// save function for doubleClick event
void OneButton::attachDoubleClick(callbackFunction newFunction) {
  _doubleClickFunc = newFunction;
  _maxClicks = max(_maxClicks, 2);
}  // attachDoubleClick


// save function for parameterized doubleClick event
void OneButton::attachDoubleClick(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramDoubleClickFunc = newFunction;
  _doubleClickFuncParam = parameter;
  _maxClicks = max(_maxClicks, 2);
}  // attachDoubleClick


// save function for multiClick event
void OneButton::attachMultiClick(callbackFunction newFunction) {
  _multiClickFunc = newFunction;
  _maxClicks = max(_maxClicks, 100);
}  // attachMultiClick


// save function for parameterized MultiClick event
void OneButton::attachMultiClick(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramMultiClickFunc = newFunction;
  _multiClickFuncParam = parameter;
  _maxClicks = max(_maxClicks, 100);
}  // attachMultiClick


// save function for longPressStart event
void OneButton::attachLongPressStart(callbackFunction newFunction) {
  _longPressStartFunc = newFunction;
}  // attachLongPressStart


// save function for parameterized longPressStart event
void OneButton::attachLongPressStart(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramLongPressStartFunc = newFunction;
  _longPressStartFuncParam = parameter;
}  // attachLongPressStart


// save function for longPressStop event
void OneButton::attachLongPressStop(callbackFunction newFunction) {
  _longPressStopFunc = newFunction;
}  // attachLongPressStop


// save function for parameterized longPressStop event
void OneButton::attachLongPressStop(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramLongPressStopFunc = newFunction;
  _longPressStopFuncParam = parameter;
}  // attachLongPressStop


// save function for during longPress event
void OneButton::attachDuringLongPress(callbackFunction newFunction) {
  _duringLongPressFunc = newFunction;
}  // attachDuringLongPress


// save function for parameterized during longPress event
void OneButton::attachDuringLongPress(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramDuringLongPressFunc = newFunction;
  _duringLongPressFuncParam = parameter;
}  // attachDuringLongPress


// save function for idle button event
void OneButton::attachIdle(callbackFunction newFunction) {
  _idleFunc = newFunction;
}  // attachIdle


void OneButton::reset(void) {
  _state = OCS_INIT;
  _nClicks = 0;
  _startTime = millis();
  _idleState = false;
}


// ShaggyDog ---- return number of clicks in any case: single or multiple clicks
int OneButton::getNumberClicks(void) {
  return _nClicks;
}


/**
 * @brief Debounce input pin level for use in SpesialInput.
 */
bool OneButton::debounce(const bool value) {
  now = millis();  // current (relative) time in msecs.

  // Don't debounce going into active state, if _debounce_ms is negative
  if (value && _debounce_ms < 0)
    debouncedLevel = value;

  if (_lastDebounceLevel == value) {
    if (now - _lastDebounceTime >= abs(_debounce_ms))
      debouncedLevel = value;
  } else {
    _lastDebounceTime = now;
    _lastDebounceLevel = value;
  }
  return debouncedLevel;
};


/**
 * @brief Check input of the configured pin,
 * debounce button state and then
 * advance the finite state machine (FSM).
 */
void OneButton::tick(void) {
  if (_pin >= 0) {
#if !USE_SIMULATION
    _fsm(debounce(digitalRead(_pin) == _buttonPressed));
#endif
  }
}  // tick()


void OneButton::tick(bool activeLevel) {
  _fsm(debounce(activeLevel));
}


/**
 *  @brief Advance to a new state and save the last one to come back in cas of bouncing detection.
 */
void OneButton::_newState(stateMachine_t nextState) {
  _state = nextState;
}  // _newState()


/**
 * @brief Run the finite state machine (FSM) using the given level.
 */
void OneButton::_fsm(bool activeLevel) {
  unsigned long waitTime = (now - _startTime);

  // Implementation of the state machine
  switch (_state) {
    case OCS_INIT:
      // on idle for idle_ms call idle function
      if (!_idleState and (waitTime > _idle_ms))
        if (_idleFunc) {
          _idleState = true;
          _idleFunc();
        }

      // waiting for level to become active.
      if (activeLevel) {
        _newState(OCS_DOWN);
        _startTime = now;  // remember starting time
        _nClicks = 0;

        if (_pressFunc) _pressFunc();
        if (_paramPressFunc) _paramPressFunc(_pressFuncParam);
      }  // if
      break;

    case OCS_DOWN:
      // waiting for level to become inactive.

      if (!activeLevel) {
        _newState(OCS_UP);
        _startTime = now;  // remember starting time

      } else if (waitTime > _press_ms) {
        if (_longPressStartFunc) _longPressStartFunc();
        if (_paramLongPressStartFunc) _paramLongPressStartFunc(_longPressStartFuncParam);
        _newState(OCS_PRESS);
      }  // if
      break;

    case OCS_UP:
      // level is inactive

      // count as a short button down
      _nClicks++;
      _newState(OCS_COUNT);
      break;

    case OCS_COUNT:
      // dobounce time is over, count clicks

      if (activeLevel) {
        // button is down again
        _newState(OCS_DOWN);
        _startTime = now;  // remember starting time

      } else if ((waitTime >= _click_ms) || (_nClicks == _maxClicks)) {
        // now we know how many clicks have been made.

        if (_nClicks == 1) {
          // this was 1 click only.
          if (_clickFunc) _clickFunc();
          if (_paramClickFunc) _paramClickFunc(_clickFuncParam);

        } else if (_nClicks == 2) {
          // this was a 2 click sequence.
          if (_doubleClickFunc) _doubleClickFunc();
          if (_paramDoubleClickFunc) _paramDoubleClickFunc(_doubleClickFuncParam);

        } else {
          // this was a multi click sequence.
          if (_multiClickFunc) _multiClickFunc();
          if (_paramMultiClickFunc) _paramMultiClickFunc(_multiClickFuncParam);
        }  // if

        reset();
      }  // if
      break;

    case OCS_PRESS:
      // waiting for pin being release after long press.

      if (!activeLevel) {
        _newState(OCS_PRESSEND);

      } else {
        // still the button is pressed
        if ((now - _lastDuringLongPressTime) >= _long_press_interval_ms) {
          if (_duringLongPressFunc) _duringLongPressFunc();
          if (_paramDuringLongPressFunc) _paramDuringLongPressFunc(_duringLongPressFuncParam);
          _lastDuringLongPressTime = now;
        }
      }  // if
      break;

    case OCS_PRESSEND:
      // button was released.

      if (_longPressStopFunc) _longPressStopFunc();
      if (_paramLongPressStopFunc) _paramLongPressStopFunc(_longPressStopFuncParam);
      reset();
      break;

    default:
      // unknown state detected -> reset state machine
      _newState(OCS_INIT);
      break;
  }  // if

}  // OneButton.tick()


// end.
