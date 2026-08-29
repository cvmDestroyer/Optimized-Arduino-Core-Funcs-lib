/*
  pretty simpel just type uno:: infont of the standard
  Arduino functions. example : uno::digitalWrite(...);
  currently there is only a namespace for Uno/Nano
  but im exicted to work on other achitetures and
  hopefuly there will be more options soon not every
  thing is will probably be tested because the lack of
  money i have to buy all chips but if there is an problem
  please contact me or say your criticsm im only 16 at the
  time of writing this so i hope for your understanding
*/
// clang-format off
#include <optmzdArd.h> // if your not using Platformio this must be replaced with[ #include "optmzdUno.h" ]

constexpr uint8_t led{6};
constexpr uint8_t button{3};
constexpr uint8_t analogThingIn{A1};

void setup() {
    uno::pinMode(led, OUTPUT);
    uno::pinMode(button, INPUT_PULLUP);
    uno::pinMode(analogThingIn, INPUT);

    // template variant for even faster execution just make sure 
    // all pind always come in to the <> brackets rest ist the same
    uno::pinMode<led>(OUTPUT);
    uno::pinMode<button>(INPUT_PULLUP);
    uno::pinMode<analogThingIn>(INPUT);
}

void loop() {
    uno::analogWrite(led, 200); // 0-255
    uno::digitalWrite(led, HIGH);

    if (uno::analogRead(analogThingIn) < 200)
        Serial.println("Reading somthing analog");
   
    if (uno::digitalRead(button) == LOW)
        Serial.println("a button was pressed");
    
    if (uno::analogRead(analogThingIn, performance) < 200)
        Serial.println("Reading somthing analog but faster");

    uno::tone(200);     // standard is pin 3 cuz of OCR2B which only works on Pin 3 (timer 2)
    uno::tone(10, 200); // the only reason why it works here with any other than pin 3 is because we 
                        // simulate the turn by turning the thingy on and off (with an ISR)
    uno::noTone(10);
    uno::noTone(3);

    // template variant for even faster execution just remember 
    // all pins come in to the <> brackets rest ist the same
    uno::analogWrite<led>(200);
    uno::digitalWrite<led>(HIGH);

    if (uno::analogRead<analogThingIn>() < 200)
        Serial.println("Reading somthing analog in max speed (almost)");
   
    if (uno::digitalRead<button>() == LOW)
        Serial.println("this button was read in one clock cycle (i think)");
    
    uno::tone<10>(200);
    uno::noTone<10>();

    uint8_t time {uno::millis()};
    while (uno::millis() - time < 200) {
        Serial.println("jo some millis() thingy");
    }

    uno::delay(50);
}