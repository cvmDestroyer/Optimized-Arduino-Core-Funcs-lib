#include "pins_arduino.h"
#include "wiring_private.h"
#include <Arduino.h>
#include <stdint.h>
#include <util/delay.h>
#include "optmzdArd.h"
// clang-format off

#include <avr/io.h>


// THIS HAS HIGH RISK ONLY UNCOMMENT IF YOU KNOW WHAT UR DOING
// THIS COULD KILL OTHER LIBS if you uncomment '#define ENABLE_UNO_HIGH_RISK_HIGH_PRECISION_TIMER_0'
// timer 0 zero will run on ctc NOT fast-pwm and kill pin 5 and 6
// #define ENABLE_UNO_HIGH_RISK_HIGH_PRECISION_TIMER_0

namespace ard 
{
    // normal versions
    void pinMode(uint8_t pin, uint8_t func)                            { ::pinMode(pin, func);                                           }
    void digitalWrite(uint8_t pin, bool HILO)                          { ::digitalWrite(pin, HILO);                                      }
    bool digitalRead(uint8_t pin)                                      { bool ret{::digitalRead(pin)};    return ret;                    }
    uint16_t analogRead(uint8_t pin)                                   { uint16_t ret{::analogRead(pin)}; return ret;                    }
    void analogReference(uint8_t mode)                                 { ::analogReference(mode);                                        }
    void analogWrite(uint8_t pin, uint8_t val)                         { ::analogWrite(pin, val);                                        }

    uint32_t millis(void)                                              { uint32_t ret{::millis()}; return ret;                           }
    uint32_t micros(void)                                              { uint32_t ret{::micros()}; return ret;                           }
    void delay(uint32_t ms)                                            { ::delay(ms);                                                    }
    void delayMicroseconds(uint32_t us)                                { ::delayMicroseconds(us);                                        }                   
    uint32_t pulseIn(uint8_t pin, uint8_t state)                       { uint32_t ret{::pulseIn(pin, state, 1000000UL)    }; return ret; }
    uint32_t pulseInLong(uint8_t pin, uint8_t state)                   { uint32_t ret{::pulseInLong(pin, state, 1000000UL)}; return ret; }
    uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout)     { uint32_t ret{::pulseIn(pin, state, timeout)    }; return ret;   }
    uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout) { uint32_t ret{::pulseInLong(pin, state, timeout)}; return ret;   }

    void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode) { ::attachInterrupt(interruptNum, *userFunc, mode);     }
    void detachInterrupt(uint8_t interruptNum)                                   { ::detachInterrupt(interruptNum);                      }

    void tone(uint8_t pin, uint16_t frequency)                         { ::tone(pin, frequency);                                         }
    void tone(uint8_t pin, uint16_t frequency, uint32_t duration)      { ::tone(pin, frequency, duration);                               }
    void noTone(uint8_t pin)                                           { ::noTone(pin);                                                  }

    // template versions - only for compability reasons

    template<uint8_t PIN>
    void pinMode(uint8_t func)                            { ::pinMode(PIN, func);                                           }
    template<uint8_t PIN>
    void digitalWrite(bool HILO)                          { ::digitalWrite(PIN, HILO);                                      }
    template<uint8_t PIN>
    bool digitalRead()                                    { bool ret{::digitalRead(PIN)};    return ret;                    }
    template<uint8_t PIN>
    uint16_t analogRead()                                 { uint16_t ret{::analogRead(PIN)}; return ret;                    }
    template<uint8_t PIN>
    void analogWrite(uint8_t val)                         { ::analogWrite(PIN, val);                                        }

    template<uint8_t PIN>
    uint32_t pulseIn(uint8_t state)                       { uint32_t ret{::pulseIn(PIN, state, 1000000UL)    }; return ret; }
    template<uint8_t PIN>
    uint32_t pulseInLong(uint8_t state)                   { uint32_t ret{::pulseInLong(PIN, state, 1000000UL)}; return ret; }
    template<uint8_t PIN>
    uint32_t pulseIn(uint8_t state, uint32_t timeout)     { uint32_t ret{::pulseIn(PIN, state, timeout)    }; return ret;   }
    template<uint8_t PIN>
    uint32_t pulseInLong(uint8_t state, uint32_t timeout) { uint32_t ret{::pulseInLong(PIN, state, timeout)}; return ret;   }

    template<uint8_t PIN>
    void tone(uint16_t frequency)                         { ::tone(PIN, frequency);                                         }
    template<uint8_t PIN>
    void tone(uint16_t frequency, uint32_t duration)      { ::tone(PIN, frequency, duration);                               }
    template<uint8_t PIN>
    void noTone()                                         { ::noTone(PIN);
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
        volatile uint8_t currentPin = 0;
        volatile uint32_t timer0_millis = 0;
        uint8_t timer0_fract = 0;
        volatile uint32_t timer0_overflow_count = 0;
    }
    void pinMode(uint8_t pin, uint8_t func)
    {
        if (pin >= 0 && pin <= 7) 
        {
            if (func == _OUTPUT) {
                DDRD |= (1 << pin);
                PORTD &= ~(1 << pin);
            } else if (func == _INPUT) {
                DDRD &= ~(1 << pin);  
                PORTD &= ~(1 << pin);
            } else if (func == _INPUT_PULLUP) {
                DDRD &= ~(1 << pin);
                PORTD |= (1 << pin);
            }
        }
        else if (pin >= 8 && pin <= 13) 
        {
            uint8_t bit = pin - 8;
            if (func == _OUTPUT) {
                DDRB |= (1 << bit);
                PORTB &= ~(1 << bit);
            } else if (func == _INPUT) {
                DDRB &= ~(1 << bit);
                PORTB &= ~(1 << bit);
            } else if (func == _INPUT_PULLUP) {
                DDRB &= ~(1 << bit);
                PORTB |= (1 << bit);
            }
        }
    }
    void digitalWrite(uint8_t pin, bool HILO)
    {
        uint8_t oldSREG = SREG;
        cli();

        if (pin >= 0 && pin <= 7) 
        { 
            if (HILO) 
                PORTD |= (1 << pin);
            else 
                PORTD &= ~(1 << pin);
        }
        else if (pin >= 8 && pin <= 13) 
        {
            if (HILO)
                PORTB |= (1 << (pin - 8));
            else
                PORTB &= ~(1 << (pin - 8));
        }
        else if (pin >= 14 && pin <= 19) 
        {
            if (HILO)
                PORTB |= (1 << (pin - 14));
            else
                PORTB &= ~(1 << (pin - 14));
        }
        
        SREG = oldSREG;
    }
    bool digitalRead(uint8_t pin)
    {
        if (pin >= 0 && pin <= 7)
            return (PIND & (1 << pin)) ? HIGH : LOW;
        
        if (pin >= 8 && pin <= 13)
            return (PINB & (1 << (pin - 8))) ? HIGH : LOW;

        if (pin >= 14 && pin <= 19)
            return (PINC & (1 << (pin - 14))) ? HIGH : LOW;
                                 
        return LOW;
    }
    uint16_t analogRead(uint8_t pin)
    {
        if (pin >= 14)
            pin -= 14;
        
        ADMUX = (1 << REFS0) | (pin & 0x07);

        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

        ADCSRA |= (1 << ADSC);

        while (ADCSRA & (1 << ADSC))
            ;

        return ADC;
    }
    void analogReference(uint8_t mode)
    {
        // okay this is pretty simpel there are the following register REFS0, REFS1 in ADMUX
        // 10 is 5V/Vcc(DEFAULT), 00 is AREF(EXTERNAL), 11 is 1.1V(INTERNAL)
        // THE FIRST NUM IST ALWAYS REFS0 AND THE SECOND IS REFS1
            
        if (mode == _INTERNAL)      // INTERNAL (REFS0 = 1, REFS1 = 1)
            ADMUX |= (1 << REFS0) | (1 << REFS1); 
        else if (mode == _EXTERNAL) // EXTERNAL (REFS0 = 0, REFS1 = 0)
            ADMUX &= ~((1 << REFS0) | (1 << REFS1)); 
        else                        // DEFAULT  (REFS0 = 1, REFS1 = 0)
            ADMUX = (ADMUX & ~(1 << REFS1) | (1 << REFS0)); 
    }
    void analogWrite(uint8_t pin, uint8_t val) // clang-format off
    { 
        switch (pin)
        {
        case 3:
            if (val == 0) {               // this works the same for every pin
                TCCR2A &= ~(1 << COM2B1); // every pin has either diffrent pin register or PWM register
                PORTD &= ~(1 << PORTD3);  // thats why we need an case for everyone
            } else if (val == 255) {      // if val is 0 or 255 we just either complety turn PWM off and give the pin 
                TCCR2A &= ~(1 << COM2B1); // full power for 255 or if its off we also dont need PWM
                PORTD |= (1 << PORTD3);
            } else {
                TCCR2A |= (1 << COM2B1);
                OCR2B = val;
            } break;

        case 11:
            if (val == 0) {
                TCCR2A &= ~(1 << COM2A1);
                PORTB &= ~(1 << PORTB3);
            } else if (val == 255) {
                TCCR2A &= ~(1 << COM2A1);
                PORTB |= (1 << PORTB3);
            } else {
                TCCR2A |= (1 << COM2A1);
                OCR2A = val;
            } break;

        case 5:
            if (val == 0) {
                TCCR0A &= ~(1 << COM0B1);
                PORTD &= ~(1 << PORTD5);
            } else if (val == 255) {
                TCCR0A &= ~(1 << COM0B1);
                PORTD |= (1 << PORTD5);
            } else {
                TCCR0A |= (1 << COM0B1);
                OCR0B = val;
            } break;

        case 6:
            if (val == 0) {
                TCCR0A &= ~(1 << COM0A1);
                PORTD &= ~(1 << PORTD6);
            } else if (val == 255) {
                TCCR0A &= ~(1 << COM0A1);
                PORTD |= (1 << PORTD6);
            } else {
                TCCR0A |= (1 << COM0A1);
                OCR0A = val;
            } break;

        case 9:
            if (val == 0) {
                TCCR1A &= ~(1 << COM1A1);
                PORTB &= ~(1 << PORTB1);
            } else if (val == 255) {
                TCCR1A &= ~(1 << COM1A1);
                PORTB |= (1 << PORTB1);
            } else {
                TCCR1A |= (1 << COM1A1);
                OCR1A = val;
            } break;

        case 10:
            if (val == 0) {
                TCCR1A &= ~(1 << COM1B1);
                PORTB &= ~(1 << PORTB2);
            } else if (val == 255) {
                TCCR1A &= ~(1 << COM1B1);
                PORTB |= (1 << PORTB2);
            } else {
                TCCR1A |= (1 << COM1B1);
                OCR1B = val;
            } break;

        default: // incase someone trys it on digital pin
            digitalWrite(pin, (val <= 128) ? false : true);
            break;

        }
    }

    uint32_t millis() {
        uint8_t oldSREG = SREG;
        cli();
        uint32_t ms{uno::privat::timer0_millis};
        SREG = oldSREG;
        return ms;
    }
    uint32_t micros() {
        uint8_t oldSREG = SREG;
        cli();
        
        uint32_t m{uno::privat::timer0_overflow_count};
        uint8_t t{TCNT0};

        #ifdef ENABLE_UNO_HIGH_RISK_HIGH_PRECISION_TIMER_0
            // in ctc mode we look out for OCF0A NOT TOV0
            if ((TIFR0 & (1 << OCF0A)) && (t < 249)) {
                m++;
            }
            SREG = oldSREG;
            return ((m * 250 + t) * 4);
        #else
            // in fast pwm we look for TOV0
            if ((TIFR0 & (1 << TOV0)) && (t < 255)) {
                m++;
            }
            SREG = oldSREG;
            return (((m << 8) + t) * 4);
        #endif
    }
    void delay(uint32_t ms) {
        uint32_t temp{uno::millis()};
        while (uno::millis() - temp < ms)
            ;
    }
    void delayMicroseconds(uint16_t us) {
        // if we use the millis way the call alone talkes to long (2us)
        // and the precisest way is to use the delay_us() function
        // the other method is the following (still not perfectly precise)
        // #if F_CPU >= 16000000L
        //     if (us <= 2) return;
        //     us -= 2; 
        // #endif

        // while (us--) {
        //     __asm__ __volatile__ (
        //         "nop\n\t"
        //         "nop\n\t"
        //         "nop\n\t"
        //         "nop\n\t"
        //         "nop\n\t"
        //         "nop\n\t"
        //         "nop\n\t"
        //         "nop\n\t"
        //         "nop\n\t"
        //         "nop\n\t"
        //     );
        // }
        while (us--) {
            _delay_us(1);
        }
    }

    void tone(uint8_t pin, uint16_t frequency)
    {
        // here we do the other method whith the ISR
        // but its basicly the same thing as 'void tone(uint16_t frequency);'
        currentPin = pin;
        pinMode(currentPin, OUTPUT);

        TCCR2A = 0;
        TCCR2B = 0;
        TCNT2 = 0;

        TCCR2A |= (1 << WGM21);

        TCCR2B |= (1 << CS22) | (1 << CS21);

        unsigned long ocrVal = (16000000 / (2 * 256 * frequency)) - 1; // oops just relized i didnt to this one yet
                                                                       // i'll do do it some other time its easy anyways
        OCR2A = static_cast<uint8_t>(ocrVal);

        TIMSK2 |= (1 << OCIE2A);
    }
    void noTone(uint8_t pin) {
        TIMSK2 &= ~(1 << OCIE2A);
        digitalWrite(currentPin, LOW);
    }
    
    // this just for the template part cool ik :)
    template<uint8_t PIN> struct hardwearLvl;
    #define DEFINE_PIN_D(p) \
    template<> struct hardwearLvl<p> { \
        static constexpr uintptr_t PORT = (uintptr_t)&PORTD; \
        static constexpr uintptr_t PIN_REG = (uintptr_t)&PIND; \
        static constexpr uintptr_t DDR = (uintptr_t)&DDRD; \
        static constexpr uint8_t BIT = p; \
    };
    #define DEFINE_PIN_B(p, bit) \
    template<> struct hardwearLvl<p> { \
        static constexpr uintptr_t PORT = (uintptr_t)&PORTB; \
        static constexpr uintptr_t PIN_REG = (uintptr_t)&PINB; \
        static constexpr uintptr_t DDR = (uintptr_t)&DDRB; \
        static constexpr uint8_t BIT = bit; \
    };
    DEFINE_PIN_D(0) DEFINE_PIN_D(1) DEFINE_PIN_D(2) DEFINE_PIN_D(3)
    DEFINE_PIN_D(4) DEFINE_PIN_D(5) DEFINE_PIN_D(6) DEFINE_PIN_D(7)

    DEFINE_PIN_B(8, 0) DEFINE_PIN_B(9, 1) DEFINE_PIN_B(10, 2)
    DEFINE_PIN_B(11, 3) DEFINE_PIN_B(12, 4) DEFINE_PIN_B(13, 5)

    // templates
    template<uint8_t PIN>
    void pinMode(uint8_t func)                                        {}
    template<uint8_t PIN>
    void digitalWrite(uint8_t pin, bool HILO) {
        volatile uint8_t* port = reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PORT);
        constexpr uint8_t mask = (1 << hardwearLvl<PIN>::BIT);
    
        if (val)
            *port |= mask;
        else
            *port &= ~mask;
    }
    template<uint8_t PIN>
    bool digitalRead(uint8_t pin)                                     {}
    template<uint8_t PIN>
    uint16_t analogRead(uint8_t pin)                                  {}
    template<uint8_t PIN>
    void analogWrite(uint8_t pin, uint8_t val)                        {}
    template<uint8_t PIN>
    uint32_t pulseIn(uint8_t pin, uint8_t state)                      {}
    template<uint8_t PIN>
    uint32_t pulseInLong(uint8_t pin, uint8_t state)                  {}
    template<uint8_t PIN>
    uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout)    {}
    template<uint8_t PIN>
    uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout){}
    template<uint8_t PIN>
    void tone(uint8_t pin, uint16_t frequency)                        {}
    template<uint8_t PIN>
    void tone(uint8_t pin, uint16_t frequency, uint32_t duration)     {}
    template<uint8_t PIN>
    void noTone(uint8_t pin)                                          {}

    // extras
    uint16_t analogRead(uint8_t pin, bool modePeformance)
    {
        ADCSRA |= (1 << ADPS2);
        ADCSRA &= ~(1 << ADPS1);
        ADCSRA &= ~(1 << ADPS0);

        ADMUX = (ADMUX & 0xF0) | (pin & 0x0F);

        ADCSRA |= (1 << ADSC);

        while (ADCSRA & (1 << ADSC))
            ;

        return ADC;
    }
    void tone(uint16_t frequency)
    {
        // this hole thing is aliitle complexer than the rest but not realy
        // so basicly you cant use ONE prescaler or it will be very out of tune so you just use one
        // till it gets unpresice than you just switch prescaler prescaler can be ajusted with
        // CS2 there are 3 CS20 CS21 CS22 and with them you can create prescaler from 1024 to 8
        // this only works on pin 3 because its the only pin which lets do actual waves BUT there is another way with
        // ISR (Interrupt Serviece Routine) which can be used with every pin cuz it just turns thing on and off wich makes the wave 

        DDRD |= (1 << PB3);
        if (frequency < 61) { // our prescaler is too bad for anthing under 61 so we just turn it off
            digitalWrite(3, LOW);
            return;
        }
                                   // we can save the prescaler bits here for example if i want a prescaler of 1024 i could just write 0b00000110
        uint8_t prescalerBits = 0; // but its easier to understand it with CS2x or it just a bif mess         
        uint32_t ocrValue = 0;

        if (frequency < 244) {
            ocrValue = (16000000UL / (1024UL * frequency)) - 1; // the formula is: clock frequency / (prescaler * frequency) - 1 (because machines start at 0 NOT 1 and the clock frequnxy is 16 mio(UNO))
            prescalerBits = (1 << CS22) | (1 << CS21) | (1 << CS20);
        } else if (frequency < 488) {
            ocrValue = (16000000UL / (256UL * frequency)) - 1;
            prescalerBits = (1 << CS22) | (1 << CS21);
        } else if (frequency < 976) {
            ocrValue = (16000000UL / (128UL * frequency)) - 1;
            prescalerBits = (1 << CS22) | (1 << CS20);
        } else if (frequency < 1953) {
            ocrValue = (16000000UL / (64UL * frequency)) - 1;
            prescalerBits = (1 << CS22);
        } else if (frequency < 7812) {
            ocrValue = (16000000UL / (32UL * frequency)) - 1;
            prescalerBits = (1 << CS21) | (1 << CS20);
        } else {
            ocrValue = (16000000UL / (8UL * frequency)) - 1;
            prescalerBits = (1 << CS21);
        }

        TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20); // WGM22 ist in control-register b weil
        TCCR2B = (1 << WGM22) | prescalerBits;                // weil kein platz mehr in a war (wurde im nachhinein hinzugefügt)
        // prescaler wird wieder geladen
        OCR2A = static_cast<uint8_t>(ocrValue);
        OCR2B = static_cast<uint8_t>(ocrValue / 2); // macht ne welle draus das halt auch funktioniert
    }
    void noTone() {
        TIMSK2 &= ~(1 << OCIE2A);
        digitalWrite(3, LOW);
    }
    void initTimer()
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

ISR(TIMER2_COMPA_vect) { digitalWrite(uno::privat::currentPin, !digitalRead(uno::privat::currentPin)); } 

// uno timer0
#ifdef ENABLE_UNO_HIGH_RISK_HIGH_PRECISION_TIMER_0
    ISR(TIMER0_COMPA_vect) {
        uno::privat::timer0_millis++;
        uno::privat::timer0_overflow_count++;
    }
#else
    ISR(TIMER0_OVF_vect) {
        uno::privat::timer0_overflow_count++;
        uint32_t m = uno::privat::timer0_millis;
        uint8_t f = uno::privat::timer0_fract;

        m += 1;
        f += 3; // theres alittle error this is just the correction(24/1000 * 125 = 3)
        if (f >= 125) {
            f -= 125;
            m += 1;
        }

        uno::privat::timer0_fract = f;
        uno::privat::timer0_millis = m;
    }
#endif

