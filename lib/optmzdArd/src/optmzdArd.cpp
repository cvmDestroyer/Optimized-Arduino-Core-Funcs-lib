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
        case 3:                           // it works the same but you have to do this for
            if (val == 0) {               // every pwm part thats the only 'hard' thing
                TCCR2A &= ~(1 << COM2B1); // but you just catch if pwm is not needed (0 and 255)
                PORTD &= ~(1 << PORTD3);  // if its not needed just turn off pwm and use digitalWrite
            } else if (val == 255) {      // else just use pwm and done
                TCCR2A &= ~(1 << COM2B1); 
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
    template<uint8_t PIN>
    void pinMode(uint8_t func) {
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
    template<uint8_t PIN>
    void digitalWrite(uint8_t pin, bool HILO) {
        volatile uint8_t* reg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PORT)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};
    
        if (val)
            *reg |= mask;
        else
            *reg &= ~mask;
    }
    template<uint8_t PIN>
    bool digitalRead(uint8_t pin) {
        volatile uint8_t* reg{reinterpret_cast<volatile uint8_t*>(hardwearLvl<PIN>::PIN_REG)};
        constexpr uint8_t mask{(1 << hardwearLvl<PIN>::BIT)};

        return (*reg & mask);
    }
    template<uint8_t PIN>
    uint16_t analogRead(uint8_t pin) {
        static_assert(hardwearLvl<PIN>::ADC_CH != 255, "ERROR: analogRead can only read anlog pins PWM DOES NOT COUNT(~pin) onyl A0 - A5");

        constexpr uint8_t channel = hardwearLvl<PIN>::ADC_CH;
        
        ADMUX = (ADMUX & 0xF0) | (channel & 0x07);
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));

        return ADC;
    }
    template<uint8_t PIN>
    void analogWrite(uint8_t val) {
        analogWriteHelper<PIN>::apply(val);
    }
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

