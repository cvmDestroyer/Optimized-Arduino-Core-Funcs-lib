// clang-format off
/*
  optmzdArd - Fast Register Library for Arduino
 
  Copyright (c) 2026 Tim Seiffer - cvmDestroyer
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
 
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#ifndef optmzd_Uno_hpp
#define optmzd_Uno_hpp

#include <Arduino.h>
#include <stdint.h>

#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  constexpr uint8_t _DEFAULT           {0};
  constexpr uint8_t _EXTERNAL          {1};
  constexpr uint8_t _INTERNAL1V1       {2};
  constexpr uint8_t _INTERNAL{INTERNAL1V1};
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  constexpr uint8_t _DEFAULT              {0};
  constexpr uint8_t _EXTERNAL             {4};
  constexpr uint8_t _INTERNAL1V1          {8};
  constexpr uint8_t _INTERNAL   {INTERNAL1V1};
  constexpr uint8_t _INTERNAL2V56         {9};
  constexpr uint8_t _INTERNAL2V56_EXTCAP {13};
#else  
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
  constexpr uint8_t _INTERNAL1V1  {2};
  constexpr uint8_t _INTERNAL2V56 {3};
#else
  constexpr uint8_t _INTERNAL{3};
#endif
  constexpr uint8_t _DEFAULT {1};
  constexpr uint8_t _EXTERNAL{0};
#endif

constexpr uint8_t _INPUT         {0x00};
constexpr uint8_t _OUTPUT        {0x01};
constexpr uint8_t _INPUT_PULLUP  {0x02};

constexpr uint8_t _HIGH          {0x01};
constexpr uint8_t _LOW           {0x00};

constexpr bool performance      {true};

extern volatile uint8_t currentPin;

namespace ard 
{
    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, bool val);
    bool digitalRead(uint8_t pin);
    uint16_t analogRead(uint8_t pin);
    void analogReference(uint8_t mode);
    void analogWrite(uint8_t pin, uint8_t val);

    uint32_t millis(void);
    uint32_t micros(void);
    void delay(uint32_t ms);
    void delayMicroseconds(uint32_t us);
    uint32_t pulseIn(uint8_t pin, uint8_t state);
    uint32_t pulseInLong(uint8_t pin, uint8_t state);
    uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout);
    uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout);

    void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode);
    void detachInterrupt(uint8_t interruptNum);

    void tone(uint8_t pin, uint16_t frequency);
    void tone(uint8_t pin, uint16_t frequency, uint32_t duration);
    void noTone(uint8_t pin);
}

namespace uno
{
    void pinMode(uint8_t pin, uint8_t func);
    void digitalWrite(uint8_t pin, bool HILO);
    bool digitalRead(uint8_t pin);
    uint16_t analogRead(uint8_t pin);
    uint16_t analogRead(uint8_t pin, bool modePeformance);
    void analogReference(uint8_t mode);
    void analogWrite(uint8_t pin, uint8_t uint);

    uint32_t millis(void);
    uint32_t micros(void);
    void delay(uint32_t ms);
    void delayMicroseconds(uint32_t us);
    uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout);
    uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout);

    void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode);
    void detachInterrupt(uint8_t interruptNum);

    void tone(uint16_t frequency);
    void tone(uint8_t pin, uint16_t frequency);
    void noTone(uint8_t pin);
    void noTone();
}

#endif