#include <Arduino.h>
#include <stdint.h>

void registerDigitalWrite(uint8_t pin, bool HILO);

uint16_t normalAnalogRead(uint8_t pin);

// 3 register pro port (NUR ARDUINO UNO REGISTER SIND FÜR FAST ALLES MICRO KONTROLLER VERSCHIEDEN !!!!)

//   PORTD = digital pins 0 - 7
//   PORTB = digital pins 8 - 13
//   PORTC = analoge pins A0 - A5

//   TCCR0A, TCNT1, OCR2A = die drei ports für zeit aka millis(), delay(uint32_t)

//   ADMUX, ADCSRA, ADCW = 'analog-to-digital converter'
//     ein mikrocontroller kann intern nur 0 und 1. der ADC misst die fließende Spannung an deinem Sensor,
//     vergleicht sie mit einer referenzspannung (meist 5V)
//     und wandelt das in eine Zahl von 0 bis 1023 um.

//   UDR0, UCSR0A, UBRR0 = serial alles eigentlich

//   EEAR, EEDR, EECR = einfach für EEPROM daten reinschreiben / auslesen

// _____digitalWrite() == PORT -> (data register zbsp B)           = 'PORTB' | funktion == schaltet den ausgang auf LOW (0) oder HIGH (1)
// _____pinMode() == DDR       -> (data direction register zbsp B) = 'DDRB'  | funktion == bestimmt ob die pins Eingang (0) oder Ausgang (1) sind [EINGANG == INPUT, AUSGANG == OUTPUT]
// _____digitalRead() == PIN   -> (input pins adress zbsp B)       = 'PINB'  | funktion == liest den zustand live vom pin ab
// bsp:
void setup()
{
    DDRB |= (1 << PB1) | (1 << PB3); // |= EXREM WICHTIG!! das sorgt dafür das man nur den bestimmten pin steuert sonst würde das ganze register geteuert werden (nur '=')
    // (1 << PB1) '1' ist ob es INPUT(0) oder OUTPUT(1) sein soll, '<<' ist so wie '=', 'PB1' ist bit1 von PORTB ('P'ort'B'bit'1') wichtig es beginnt auch hier mit 0 also PB0 ist pin 8
    // also um pin 5 zu steurn benutzt man :
    DDRD |= (1 /*(OUTPUT) oder 0 (für INPUT)*/ << PD4); // technischgesehen braucht man PD-zahl garnicht die zahl alliene reicht aus aber für lesbarkeit schreibt man den piort dazu

    digitalWrite(5, HIGH);
    // ||
    PORTD |= (1 << PD4);

    // um sofort den zustand von einem pin zu ändern benutzt man " '&' = '~' "
    // also um pin 5 jz sofort auf LOW zusetzen schreibt man folgendes :
    PORTB &= ~(1 << PB4);

    //
    // DIGITAL READ (schwer)
    //    step 1 : ADMUX (ADC multiplexer Selection Register) daten geben für welchen pin benutzt wird und
    //             was unsere vergleichspannung ist

    //    step 2 : ADCSRA (ADC Control and Status Register A) daten geben wie messung sterten und geschwindigkeit einstellen

    //    step 3 : ADC (oder aufgeteilt in ADCL und ADCH) ließt die ergebnisse aus dem register aus (0 - 1023)

    // bsp:
    // step 1 : den pin und die referenzspannung wählen (ADMUX)
    //      1. REFS0 auf 1 setzen = Nutze 5V als Referenzspannung
    //      2. MUX0 auf 1 setzen = Wähle den analogen Pin A1 (Kanal 0001)
    ADMUX = (1 << REFS0) | (1 << MUX0);

    // step 2 : den ADC konfigurieren und starten
    //      ADC aktivieren + taktteiler auf 128 stellen
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS2);

    // step 3 : die messung triggern (startschluss)
    ADCSRA |= (1 << ADSC); // startschuss!

    // step 4 : warten, bis die maschine fertig ist
    while (ADCSRA & (1 << ADSC))
    {
        // hier passiert nichts bis die hardware fertig ist mit was auch immer
    }

    // ergebniss in Value ist nur eine zahl zwischen 0 und 1023 gespeichewrt
    uint16_t Value = ADC;

    //
    pinMode(1, OUTPUT);
    //
}
// hier ist eine funktion die man so benutzen kann für analogRead

uint16_t normalAnalogRead(uint8_t pin)
{
    if (pin >= 14)
    {
        pin -= 14;
    }

    // 1. pin-Kanal maskieren (A0 bis A5 entsprechen den Kanälen 0 bis 5)
    // wir löschen die alten pin-bits in ADMUX und setzen den neuen Pin ein
    ADMUX = (1 << REFS0) | (pin & 0x07);

    // 2. Startschuss geben!
    ADCSRA |= (1 << ADSC);

    // 3. warten, bis die hardware fertig ist
    while (ADCSRA & (1 << ADSC))
        ;

    // 4. wert returnen
    return ADC;
}

uint16_t tunedAnalogRead(uint8_t pin)
{
    // --- DIE BIT-BREMSE JOSLASSEN (PRESCALER ÄNDERN) ---
    // Standardmäßig stehen ADPS2, ADPS1 und ADPS0 alle auf 1 (Teiler 128).
    // Wir setzen den Teiler auf 16 (ADPS2 = 1, ADPS1 = 0, ADPS0 = 0).
    // Das erhöht den ADC-Takt von 125 kHz auf 1 MHz!
    ADCSRA |= (1 << ADPS2);  // Bit 2 auf 1 setzen
    ADCSRA &= ~(1 << ADPS1); // Bit 1 auf 0 löschen
    ADCSRA &= ~(1 << ADPS0); // Bit 0 auf 0 löschen
    // 1. MUX-Schalter einstellen
    // Wir löschen die alten Pin-Bits (die untersten 4 Bit) in ADMUX
    // und setzen die Nummer unseres Wunsch-Pins ein (0 für A0, 1 für A1 usw.)
    ADMUX = (ADMUX & 0xF0) | (pin & 0x0F);

    // 2. Messung starten (ADSC-Bit auf 1 setzen)
    ADCSRA |= (1 << ADSC);

    // 3. Warten, bis die Hardware fertig ist
    // Solange die Messung läuft, bleibt das ADSC-Bit auf 1.
    // Sobald sie fertig ist, springt es von alleine auf 0 zurück.
    while (ADCSRA & (1 << ADSC))
        ; // Hier machen wir nichts, außer warten (dauert nur 13 Mikrosekunden!)

    // 4. Das fertige 10-Bit-Ergebnis auslesen und zurückgeben
    // C++ fügt ADCL und ADCH praktischerweise im Hintergrund automatisch als "ADC" zusammen
    return ADC;
}

// ATmega328P (Arduino Uno) – ADC & MUX Architektur
// │
// ├── 🎛️ DER MUX (Multiplexer | Der elektronische Drehschalter)
// │   ├── 📌 Aufgabe: Schaltet einen der analogen Eingänge (A0-A5) auf den internen Wandler.
// │   └── ⚙️ ADMUX-Register (Die Steuerung des Drehschalters)
// │       ├── 🏷️ REFS1 & REFS0: Wählt die Referenzspannung (z.B. 5V VCC oder interne 1.1V)
// │       ├── 📐 ADLAR:         Richtet das Ergebnis links- oder rechtsbündig aus (Bit-Sortierung)
// │       └── 🎛️ MUX3 bis MUX0: Die "Schalter"-Bits. Bestimmen, welcher Pin aktiv ist:
// │                             0000 = Pin A0
// │                             0001 = Pin A1
// │                             0010 = Pin A2
// │                             0011 = Pin A3
// │                             0100 = Pin A4
// │                             0101 = Pin A5
// │
// └── 🔬 DER ADC (Analog-Digital-Converter | Das Messwerk)
//     ├── 📌 Aufgabe: Wandelt die analoge Spannung (0-5V) in eine digitale Zahl (0-1023) um.
//     ├── ⚙️ ADCSRA-Register (Control and Status Register A – Der Motor des ADC)
//     │   ├── ⚡ ADEN:  ADC Enable (Schaltet den gesamten Wandler ein oder aus)
//     │   ├── 🚀 ADSC:  ADC Start Conversion (Gibt den Befehl: "Jetzt einmal messen!")
//     │   ├── 🔄 ADATE: ADC Auto Trigger Enable (Misst automatisch immer wieder im Hintergrund)
//     │   ├── 🏁 ADIF:  ADC Interrupt Flag (Wird 1, sobald die Messung fertig ist)
//     │   └── ⏱️ ADPS2 bis ADPS0: ADC Prescaler (Die Bremse für das Messwerk – genau wie beim Timer!)
//     │
//     └── 📦 DIE ERGEBNIS-REGISTER (Hier landet deine Zahl)
//         ├── ADCL (Die unteren 8 Bit des Ergebnisses)
//         └── ADCH (Die oberen 2 Bit des Ergebnisses -> Da der ADC 10 Bit hat)

//

//

// ADPS2 | ADPS1 | ADPS1 | Teiler (Prescaler) | ADC-Takt (bei 16 MHz CPU) |              Was bedeutet das in der Praxis?                    |
// -----------------------------------------------------------------------------------------------------------------------------------------
//   0   |   0   |   0   |          2         |            8 MHz          | Viel zu schnell! Der Wandler kommt nicht mehr mit.
//   0   |   0   |   1   |          2         |            8 MHz          | (Das Gleiche wie oben, Teiler 2 ist doppelt belegt).
//   0   |   1   |   0   |          4         |            4 MHz          | Extrem schnell, aber starker Präzisionsverlust.
//   0   |   1   |   1   |          8         |            2 MHz          | Sehr schnell, Messwerte schwanken stark.
//   1   |   0   |   0   |         16         |            1 MHz          | Unser Turbo-Modus! Guter Kompromiss aus Speed und Genauigkeit.
//   1   |   0   |   1   |         32         |          500 kHz          | Schön schnell, Ergebnisse sind noch ziemlich stabil.
//   1   |   1   |   0   |         64         |          250 kHz          | Fast maximale Genauigkeit, doppelt so schnell wie normal.
//   1   |   1   |   1   |        128         |          125 kHz          | Arduino Standard! Maximale Präzision, aber sehr langsam.