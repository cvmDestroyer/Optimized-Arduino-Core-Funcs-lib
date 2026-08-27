#include <Arduino.h>
#include <stdint.h>

// der ATmega328P(uno chip) hat drei timer (timer 0, timer 1, timer 2)
// diese laufen IMMER und sind für alles zeit 'related' also delay millis usw

//     CTC = Clear Timer on Compare Match
//         ohne: die timer sind eigentlich auf 8bit gestellt also geht sie nur auf
//               255 hoch und starten dann wieder bei 0

//         mit : CTC modus macht forgendes -> zehlt bis zahl x danach geht er zurück auf
//                  danach geht er zurück auf 0 und schaltet pin um also von HIGH wird LOW und umgekehrt

//         ergebnis : durch das ständige umschalten von high zu low und dann von low zu high                    __    __    __
//                    passiert folgendes : das an aus erzeugt am ouput pin eine Rechteckwelle aka unser ton  __|  |__|  |__|

void selfmTone(uint16_t frequency) // nur beispiel für echten muss man prescaler immer anpassen guck optmzdUno.cpp
{
    TCCR2A = 0; // alte einstellungen löschen
    TCCR2B = 0; // alte einstellungen löschen
    TCNT2 = 0;  // setzten den aktuellen zählerstand auf 0

    TCCR2A |= (1 << WGM21); // CTC aktivieren (WGM21 ist ctc modus und 1 bedeuted das er jz an ist)

    TCCR2A |= (1 << COM2A0); // pin 11 benutzen pin 3 bugt dann benutzt man fast pwm (Top: OCR2A)
    // OC2B ist pin 3 auf uno, COM2B0 ist basicly das bloß das er bei jedem 'treffer' sein zustand switched

    TCCR2B |= (1 << CS22); // nun aktivieren wir den CS22 (Prescaler)
    // der prozessor von uno läuft mit 16MHz aka 16 mio ticks per sec
    // das ist viel zu schnell der CS22 macht einfach aus 16MHz 250Hz
    // jz können wir ihn benutzen

    // der nächste schritt ist die grenze zu setzen (CTC)
    // dann schreiben wir ihn ins vergleichregister OCR2A
    // das ist die formel : OCR2A = 16.000.000/2 * 64 * frequenz | -1
    // 16 mio ist die tacktrate von arduino / 2 ist der toggle und 64 ist der prescaler (CS22)
    // -1 weil er bei 0 anfängt zu zählen nicht 1

    uint32_t ocrVal = (16000000 / (2 * 64 * frequency)) - 1;

    OCR2A = (uint8_t)ocrVal;

    // 5. Timer starten durch Setzen des Prescalers auf 64
    TCCR2B |= (1 << CS22);
}

// Prescaler	Mindest-Frequenz	 Höchst-Frequenz
// 1              ~62.500 Hz          ~8.000.000 Hz
// 8               ~7.812 Hz          ~1.000.000 Hz
// 32	           ~1.953 Hz            ~250.000 Hz
// 64                ~976 Hz            ~125.000 Hz
// 128	             ~488 Hz             ~62.500 Hz
// 256	             ~244 Hz             ~31.250 Hz
// 1024	              ~61 Hz              ~7.800 Hz
//                mit ctc 31 Hz
//

// CS (Clock Select) erklärt:
// die erste zahl nach CS ist für welchen timer man benutzt also für timer 0 : CS0x, timer 1 : CS1x, timer 2 : CS2x
// für die zeite zahl ist es koplizierter dafür guck einfach die tabelle unten
// aber es gibt drei 2, 1, 0 und dann muss man in der tabele nachgucken was man will

//  CSx2 |  CSx1 |  CSx0 | Bedeutung / Prescaler |  Was der Timer macht
//-------------------------------------------------------------------------------------------------------
//   0   |   0   |   0   |   Timer gestoppt      |    Der Ton geht aus.
//   0   |   0   |   1   |   Direkter Takt (1)   |    Zählt ungebremst mit 16 Millionen Schritten/Sek.
//   0   |   1   |   0   |   Teilen durch 8      |    Zählt mit 2 Millionen Schritten/Sek.
//   0   |   1   |   1   |   Teilen durch 32     |    Zählt mit 500.000 Schritten/Sek.
//   1   |   0   |   0   |   Teilen durch 64     |    Unser erstes Beispiel (250.000 Schritte/Sek.)
//   1   |   0   |   1   |   Teilen durch 128    |    Zählt mit 125.000 Schritten/Sek.
//   1   |   1   |   0   |   Teilen durch 256    |    Unser zweites Beispiel für 440 Hz.
//   1   |   1   |   1   |   Teilen durch 1024   |    Maximale Bremse für ganz tiefe Töne.

// ATmega328P (Arduino Uno) – Hardware-Timer-Architektur
// │
// ├── 📂 TIMER 0 (8-Bit Zähler | Zählt von 0 bis 255)
// │   ├── 📌 Standard-Nutzung: Liefert den Takt für millis(), micros(), delay()
// │   ├── 🔌 Hardware-Pins:    Pin 6 (OC0A) und Pin 5 (OC0B) -> Für PWM (analogWrite)
// │   ├── ⚙️ Wichtige Register:
// │   │   ├── TCNT0  (Zählerstand: Hier drin wird hochgezählt)
// │   │   ├── TCCR0A (Kontroll-Register A: Bestimmt den Modus, z.B. CTC oder PWM)
// │   │   ├── TCCR0B (Kontroll-Register B: Hier sitzen die CS-Bits)
// │   │   ├── OCR0A  (Vergleichsregister A: Schwellenwert für Pin 6)
// │   │   └── OCR0B  (Vergleichsregister B: Schwellenwert für Pin 5)
// │   └── ⏱️ Takt-Schalter:   CS02, CS01, CS00 (Clock Select Bits für Timer 0)
// │
// ├── 📂 TIMER 1 (16-Bit Zähler | Zählt von 0 bis 65.535 -> Extrem präzise!)
// │   ├── 📌 Standard-Nutzung: Wird von der <Servo.h> Bibliothek für Servomotoren genutzt
// │   ├── 🔌 Hardware-Pins:    Pin 9 (OC1A) und Pin 10 (OC1B)
// │   ├── ⚙️ Wichtige Register:
// │   │   ├── TCNT1  (Zählerstand: 16-Bit groß)
// │   │   ├── TCCR1A (Kontroll-Register A)
// │   │   ├── TCCR1B (Kontroll-Register B: Enthält die CS-Bits)
// │   │   ├── OCR1A  (Vergleichsregister A: Schwellenwert für Pin 9)
// │   │   └── OCR1B  (Vergleichsregister B: Schwellenwert für Pin 10)
// │   └── ⏱️ Takt-Schalter:   CS12, CS11, CS10 (Clock Select Bits für Timer 1)
// │
// └── 📂 TIMER 2 (8-Bit Zähler | Zählt von 0 bis 255 | Unser "Ton-Timer")
//     ├── 📌 Standard-Nutzung: Wird von der echten tone()-Funktion besetzt
//     ├── 🔌 Hardware-Pins:    Pin 11 (OC2A) und Pin 3 (OC2B) -> Unser Ton-Ausgang!
//     ├── ⚙️ Wichtige Register:
//     │   ├── TCNT2  (Zählerstand)
//     │   ├── TCCR2A (Kontroll-Register A: Schaltet den CTC-Modus und Pin-Toggle ein)
//     │   ├── TCCR2B (Kontroll-Register B: Enthält die CS-Bits)
//     │   ├── OCR2A  (Vergleichsregister A: Bestimmt die Frequenz / Obergrenze)
//     │   └── OCR2B  (Vergleichsregister B: Löst das Umschalten an Pin 3 aus)
//     └── ⏱️ Takt-Schalter:   CS22, CS21, CS20 (Clock Select Bits für Timer 2)
//
//
// Waveform Generation Mode erklärt :
// WGMx2 | WGMx1 | WGMx0 |      Modus-Name       |                             Was macht der Timer hier?
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//   0   |   0   |   0   |        Normal         | Er zählt einfach stumpf von 0 bis 255, fängt von vorne an. Gut für reine Zeitmessung.
//   0   |   1   |   0   |         CTC           | Er zählt von 0 bis zu einem Zielwert in OCR2A und springt dann sofort zurück auf 0. (Gut für fixe Frequenzen ohne PWM).
//   0   |   1   |   1   |  Fast PWM (Standard)  | Er zählt bis 255. Der Pin schaltet ab, wenn er OCR2B erreicht. Das nutzt der normale Arduino-Befehl analogWrite().
//   1   |   1   |   1   | Fast PWM (Top: OCR2A) | Er zählt nicht bis 255, sondern nur bis zum Wert in OCR2A. Dadurch können wir die Frequenz extrem präzise bestimmen.
//
// =========================================================================================================================================================================

// FLEXIBLEN TONE (WIE DAS ORIGINAL) IM DETAIL
// step 1: eine globlage var mit dem 'tag' volatile dafor das sagt dem
// compiler das die var unberechenbar ist und sich sehr schnell ändern könnte
volatile uint8_t currentPin = 0;

void flexTone(uint8_t pin, unsigned int frequency)
{
    currentPin = pin;
    pinMode(currentPin, OUTPUT);

    // timer 2 löschen
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;

    // ctc wieder aktivieren so wie bei normalen
    TCCR2A |= (1 << WGM21);

    // precaler auf 256 muss aber noch angepasst werden
    TCCR2B |= (1 << CS22) | (1 << CS21);

    // frequenz(zielwert) berechnen mit formel von oben
    unsigned long ocrVal = (16000000 / (2 * 256 * frequency)) - 1;
    OCR2A = (uint8_t)ocrVal;

    // neu: interrupt einfügen
    // TIMSK2 = Timer Interrupt Mask Register 2
    // OCIE2A = Output Compare Match A Interrupt Enable
    TIMSK2 |= (1 << OCIE2A);
}

void flexNoTone()
{
    // interrupt aus
    TIMSK2 &= ~(1 << OCIE2A);
    digitalWrite(currentPin, LOW);
}

// -----------------------------------------------------------------
// das ist ein ISR (Interrupt Service Routine)
// hier muss nichts 'gerufen' werden da dieses automatisch vom chip gemacht wird,
// sobald der timer seinen zielwert (OCR2A) erreicht hat.
// -----------------------------------------------------------------
ISR(TIMER2_COMPA_vect)
{
    // pin wert angucken und auf gegenteil setzten (für wellen du checkst) mit '!'
    // HIGH und LOW ist wie ein bool deswegen funktioniert das (nicht wirklich aber eig schon)
    // gegensatzt zu COM2B0
    digitalWrite(currentPin, !digitalRead(currentPin));
}

// bsp:
void setup()
{
    // 440 hz auf pin 8
    flexTone(8, 440);
    // macht ton solange bis 'flexNoTone();' gerufen wird (wegen interrupt)
}
// ==========================================================================================================================

// die drei unterschide zusammen gefasst

// 1. kein COM2B0

// im oberen Code hatten wir die zeile TCCR2A |= (1 << COM2B0); aka der schalter jz wird das manuel gemacht
//

// 2. plus TIMSK2-register (interrupt)

// Stattdessen haben wir TIMSK2 |= (1 << OCIE2A); hinzugefügt. Das schaltet den Interrupt ein. Du sagst dem Arduino damit: "Jedes Mal, wenn du den berechneten Zielwert erreichst, schick mir ein Signal!"
//

// 3. plus ISR

// ISR steht für Interrupt Service Routine. Du kannst diese Funktion nicht selbst im Code aufrufen (wie z. B. delay()). Der Prozessor springt hier automatisch rein, sobald das Signal von TIMSK2 kommt.
// TIMER2_COMPA_vect ist der name der ISR – bedeutung : 'timer 2 hat das compare-register A erreicht'.

// machen basacly einfach das was der COM2B0 gemacht hat bloß es geht für jeden pin und es ist manuel

//     (der echte tone() ist genau so gebaut bloß noch mit merh rechnen)