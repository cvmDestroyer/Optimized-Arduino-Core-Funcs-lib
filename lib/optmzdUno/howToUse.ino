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
#include <optmzdUno.h> // if your not using Platformio this must be replaced with[ #include "optmzdUno.h" ]

constexpr uint8_t led{2};
constexpr uint8_t button{3};
constexpr uint8_t analogThingIn{6};
constexpr uint8_t analogThingOut{6};

void loop() {
    uno::pinMode(led, OUTPUT);
    uno::pinMode(button, INPUT_PULLUP);
    uno::pinMode(analogThingIn, INPUT);
    uno::pinMode(analogThingOut, OUTPUT);
}

void setupt() {
    uno::analogWrite(analogThingOut, 200); // 0-255
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
}