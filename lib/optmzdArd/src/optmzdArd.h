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

namespace ard // <- done
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

    void tone(uint8_t pin, uint16_t frequency);
    void tone(uint8_t pin, uint16_t frequency, uint32_t duration);
    void noTone(uint8_t pin);

    // templates 
    template<uint8_t PIN> void pinMode(uint8_t func)    { ::pinMode(PIN, func);                        }
    template<uint8_t PIN> void digitalWrite(bool val)   { ::digitalWrite(PIN, val);                    }
    template<uint8_t PIN> bool digitalRead(void)        { bool ret{::digitalRead(PIN)};    return ret; }
    template<uint8_t PIN> uint16_t analogRead(void)     { uint16_t ret{::analogRead(PIN)}; return ret; }
    template<uint8_t PIN> void analogWrite(uint8_t val) { ::analogWrite(PIN, val);                     }

    template<uint8_t PIN> uint32_t pulseIn(uint8_t state)                       { uint32_t ret{::pulseIn(PIN, state, 1000000UL)    }; return ret; }
    template<uint8_t PIN> uint32_t pulseInLong(uint8_t state)                   { uint32_t ret{::pulseInLong(PIN, state, 1000000UL)}; return ret; }
    template<uint8_t PIN> uint32_t pulseIn(uint8_t state, uint32_t timeout)     { uint32_t ret{::pulseIn(PIN, state, timeout)    };   return ret; }
    template<uint8_t PIN> uint32_t pulseInLong(uint8_t state, uint32_t timeout) { uint32_t ret{::pulseInLong(PIN, state, timeout)};   return ret; }

    template<uint8_t PIN> void tone(uint16_t frequency)                   { ::tone(PIN, frequency);           }
    template<uint8_t PIN> void tone(uint16_t frequency, uint32_t duration){ ::tone(PIN, frequency, duration); }
    template<uint8_t PIN> void noTone(void)                               { ::noTone(PIN);                    }

    // this is if someone wants to write a lib with optmzdArd.h included
    // the thought proccess is if someone writes a lib for every chip they might use somting like this:
    // #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
    // namespace gpio = uno;
    // #else
    // namespace gpio = ard;
    // #endif
    // and than the template variants fall short and you are forced to use the 'normal' version
    // but if i do it like this both version work
}

namespace uno
{
    namespace privat
    {
        extern volatile uint8_t currentPin;
        extern volatile uint32_t timer0_millis;
        extern volatile uint8_t timer0_fract;
        extern volatile uint32_t timer0_overflow_count;
    }
    void pinMode(uint8_t pin, uint8_t func);     // <- done
    void digitalWrite(uint8_t pin, bool HILO);   // <- done
    bool digitalRead(uint8_t pin);               // <- done
    uint16_t analogRead(uint8_t pin);            // <- done
    void analogReference(uint8_t mode);          // <- done
    void analogWrite(uint8_t pin, uint8_t uint); // <- done

    uint32_t millis(void);                       // <- done
    uint32_t micros(void);                       // <- done
    void delay(uint32_t ms);                     // <- done
    void delayMicroseconds(uint16_t us);         // <- done
    uint32_t pulseIn(uint8_t pin, bool state, uint32_t timeout);                   // <- done
    uint32_t pulseInLong(uint8_t pin, bool state, uint32_t timeout);            //
    uint32_t pulseIn(uint8_t pin, bool state, uint32_t timeout = 1000000L);        // <- done
    uint32_t pulseInLong(uint8_t pin, bool state, uint32_t timeout = 1000000L); //
    
    void tone(uint8_t pin, uint16_t frequency); // <- done
    void noTone(uint8_t pin);                   // <- done

    // templates ------------------------------------------------------------------------------------------------
    // this just for the template part cool ik :)
    template<uint8_t PIN> struct hardwearLvl;
    #define DEFINE_PIN(pin, port_letter, bit, adc_ch, is_pwm, pwm_reg, tccr_reg, com_bit) \
    template<> struct hardwearLvl<pin> { \
        static constexpr uintptr_t PORT    = (uintptr_t)&PORT##port_letter; \
        static constexpr uintptr_t PIN_REG = (uintptr_t)&PIN##port_letter; \
        static constexpr uintptr_t DDR     = (uintptr_t)&DDR##port_letter; \
        static constexpr uint8_t   BIT     = bit; \
        static constexpr uint8_t   ADC_CH  = adc_ch; \
        static constexpr bool      HAS_PWM  = is_pwm; \
        static constexpr uintptr_t PWM_REG  = (uintptr_t)(pwm_reg); \
        static constexpr uintptr_t TCCR_REG = (uintptr_t)(tccr_reg); \
        static constexpr uint8_t   COM_BIT  = com_bit; \
    };

    template<uint8_t PIN, bool HAS_PWM = hardwearLvl<PIN>::HAS_PWM> // these are for anlogRead cuz
    struct analogWriteHelper {                                      // if constexpr only works cince C++ 17 and 
        static void apply(uint8_t val) {                            // avr-compiler are stuck on C++ 11
            digitalWrite<PIN>(val >= 128);
        }
    };

    template<uint8_t PIN>
    struct analogWriteHelper<PIN, true> {
        // i feel like this doesnt need explaination its in normal analogWrite if you want it
        static void apply(uint8_t val) {
            if (val == 0) {
                *reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::TCCR_REG) &= ~hardwearLvl<PIN>::COM_BIT;
                digitalWrite<PIN>(LOW);
            }
            else if (val == 255) {
                *reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::TCCR_REG) &= ~hardwearLvl<PIN>::COM_BIT;
                digitalWrite<PIN>(HIGH);
            }
            else {
                *reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PWM_REG) = val;
                *reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::TCCR_REG) |= hardwearLvl<PIN>::COM_BIT;
            }
        }
        
    };

    // this took me half an hour ✌️🫩
    // pin : just the arduino pin yk? 
    // reg : this is what of the 3 register the pin is placed (B, C, D)
    // bit : where the bit is placed in the byte
    // adc : basicly the bit thingy just for Analog Digital Converter (255 = no adc/analogPin) 
    // pwm : does the pin support pwm? (true/false)
    // OCR : just saves the pwm val 0-255 (Output Compare Register)
    // TCCR: control-register of timers (Timer/Counter Control Register)
    // COM : it is the bitmask to wire the pin to the hardware timer (Compare Output Mode)
    // ---------------------------------------------------------------------------------------
    //         pin  reg  bit  adc   pwm     OCR       TCCR           COM
    DEFINE_PIN( 0,   D,   0,  255, false,    0,        0,             0        )
    DEFINE_PIN( 1,   D,   1,  255, false,    0,        0,             0        )
    DEFINE_PIN( 2,   D,   2,  255, false,    0,        0,             0        )
    DEFINE_PIN( 3,   D,   3,  255, true , &OCR2B,  &TCCR2A,   (1 << COM2B1)    ) // com bit 5
    DEFINE_PIN( 4,   D,   4,  255, false,    0,        0,             0        ) 
    DEFINE_PIN( 5,   D,   5,  255, true , &OCR0B,  &TCCR0A,   (1 << COM0B1)    ) // com bit 5
    DEFINE_PIN( 6,   D,   6,  255, true , &OCR0A,  &TCCR0A,   (1 << COM0A1)    ) // com bit 7
    DEFINE_PIN( 7,   D,   7,  255, false,    0,        0,             0        )

    DEFINE_PIN( 8,   B,   0,  255, false,    0,        0,             0        )
    DEFINE_PIN( 9,   B,   1,  255, true , &OCR1A,  &TCCR1A,   (1 << COM1A1)    ) // com bit 7
    DEFINE_PIN( 10,  B,   2,  255, true , &OCR1B,  &TCCR1A,   (1 << COM1B1)    ) // com bit 5
    DEFINE_PIN( 11,  B,   3,  255, true , &OCR2A,  &TCCR2A,   (1 << COM2A1)    ) // com bit 7
    DEFINE_PIN( 12,  B,   4,  255, false,    0,        0,             0        )
    DEFINE_PIN( 13,  B,   5,  255, false,    0,        0,             0        )

    DEFINE_PIN( 14,  C,   0,   0,  false,    0,        0,             0        )
    DEFINE_PIN( 15,  C,   1,   1,  false,    0,        0,             0        )
    DEFINE_PIN( 16,  C,   2,   2,  false,    0,        0,             0        )
    DEFINE_PIN( 17,  C,   3,   3,  false,    0,        0,             0        )
    DEFINE_PIN( 18,  C,   4,   4,  false,    0,        0,             0        )
    DEFINE_PIN( 19,  C,   5,   5,  false,    0,        0,             0        )

    // templates
    template<uint8_t PIN> void pinMode(uint8_t func) {
        volatile uint8_t* outputReg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::DDR)};
        volatile uint8_t* pullupReg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PORT)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};

        if (func == _OUTPUT) {
           *outputReg |= mask;
           *pullupReg &= ~mask;
        } else if (func == _INPUT_PULLUP) {
           *outputReg &= ~mask;
           *pullupReg |= mask;
        } else if (func == _INPUT) {
           *outputReg &= ~mask;
           *pullupReg &= ~mask;
        }
    }
    template<uint8_t PIN> void digitalWrite(bool val) {
        volatile uint8_t* reg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PORT)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};
    
        if (val)
            *reg |= mask;
        else
            *reg &= ~mask;
    }
    template<uint8_t PIN> bool digitalRead(void) {
        volatile uint8_t* reg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PIN_REG)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};

        return (*reg & mask);
    }
    template<uint8_t PIN> uint16_t analogRead(void) {
        static_assert(hardwearLvl<PIN>::ADC_CH != 255, "ERROR: analogRead can only read anlog pins PWM DOES NOT COUNT(~pin) onyl A0 - A5");

        constexpr uint8_t channel = hardwearLvl<PIN>::ADC_CH;
        
        ADMUX = (ADMUX & 0xF0) | (channel & 0x07);
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));

        return ADC;
    }
    template<uint8_t PIN> void analogWrite(uint8_t val) {
        analogWriteHelper<PIN>::apply(val);
    }
    template<uint8_t PIN> uint32_t pulseIn(bool state, uint32_t timeout) {
        volatile uint8_t* reg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PIN_REG)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};

        uint32_t cycles = 0;
            
        uint32_t maxCycles = timeout * (F_CPU / 1000000L) / 16;

        while ((*reg & mask) == (state ? mask : 0)) {
            if (cycles++ >= maxCycles) return 0;
        } 
        while ((*reg & mask) == (state ? mask : 0)) {
            if (cycles++ >= maxCycles) return 0;
        }
    
        uint32_t pulseCycles = 0;
        while ((*reg & mask) == (state ? mask : 0)) {
            if (pulseCycles++ >= maxCycles) return 0;
        }
    
        return (pulseCycles * 16) / (F_CPU / 1000000L);
    }
    template<uint8_t PIN> uint32_t pulseInLong(bool state, uint32_t timeout) {
        volatile uint8_t* reg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PIN_REG)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};
        
        const uint32_t startMicros = uno::micros();

        while ((*reg & mask) == (state ? mask : 0)) {
            if (uno::micros() - startMicros >= timeout) return 0;
        }
        while ((*reg & mask) != (state ? mask : 0)) {
            if (uno::micros() - startMicros >= timeout) return 0;
        }

        const uint32_t pulseStart = uno::micros();

        while ((*reg & mask) == (state ? mask : 0)) {
            if (uno::micros() - startMicros >= timeout) return 0;
        }

        return uno::micros() - pulseStart;
    }
    template<uint8_t PIN> uint32_t pulseIn(bool state) {
        return pulseIn<PIN>(state, 1000000L);
    }
    template<uint8_t PIN> uint32_t pulseInLong(bool state) {
        return pulseInLong<PIN>(state, 1000000UL);
    }
    template<uint8_t PIN> void tone(uint16_t frequency) {
        if (frequency < 31) { TIMSK2 &= ~(1 << OCIE2A); digitalWrite<PIN>(_LOW); return; }
        uno::privat::currentPin = PIN;
        pinMode<PIN>(_OUTPUT);

        uint32_t prescaler = (frequency < 244) ? 1024 : (frequency < 488) ? 256 :
                             (frequency < 976) ? 128  : (frequency < 1953) ? 64 :
                             (frequency < 7812) ? 32  : 8; 
        uint8_t presBit    = (frequency < 244)  ? ((1 << CS22) | (1 << CS21) | (1 << CS20)) :
                             (frequency < 488)  ? ((1 << CS22) | (1 << CS21)) :
                             (frequency < 976)  ? ((1 << CS22) | (1 << CS20)) :
                             (frequency < 1953) ? (1 << CS22) :
                             (frequency < 7812) ? ((1 << CS21) | (1 << CS20)) : (1 << CS21);

        TCCR2A = (1 << WGM21);
        TCCR2B = presBit;
        OCR2A = static_cast<uint8_t>((16000000UL / (2UL * prescaler * frequency)) - 1);
        TIMSK2 |= (1 << OCIE2A);
    }
    template<uint8_t PIN> void tone(uint16_t frequency, uint32_t duration) {
        tone<PIN>(frequency);

        if (duration > 0) 
            uno::privat::toggleCount = (2UL * frequency * duration) / 1000UL;
        else 
            uno::privat::toggleCount = 0; 
    
    }
    template<uint8_t PIN> void noTone(void) {
        TIMSK2 &= ~(1 << OCIE2A);
        digitalWrite<PIN>(_LOW);
    }

    // extras
    uint16_t analogRead(uint8_t pin, bool modePeformance);
    void tone(uint16_t frequency);
    void noTone(void);
    void initTimer(void)
    {
        #ifdef ENABLE_UNO_HIGH_RISK_HIGH_PRECISION_TIMER_0
            TCCR0A = (1 << WGM01); // ctc NOT pwm (not safe libs like servo might not work inlcuding pin 5 and 6)  
            TCCR0B = (1 << CS01) | (1 << CS00);
  
            OCR0A = 249; // (250 steps are exactly 1s (machines start at 0 so thats why 249))
  
            TIMSK0 |= (1 << OCIE0A);
        #else
            TCCR0A = (1 << WGM01) | (1 << WGM00); // fast pwm NOT ctc (safe)
            TCCR0B = (1 << CS01) | (1 << CS00);   
        
            TIMSK0 |= (1 << TOIE0);
        #endif
    }
}

#endif