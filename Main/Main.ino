#include <LiquidCrystal_I2C.h>

unsigned int buttonPins[4] = {12, 13, 10, 11};
unsigned int buttonHeld[4] = {0, 0, 0, 0};
bool buttonPressed[4];

int buzzerPin = 6;

// RIGHT_ARROW 0
// LEFT_ARROW 1
// UP_ARROW 2
// DOWN_ARROW 3
// CONFIRM 4
// REGRET 5

byte characters[6][8] = {
{
  0b00000,
  0b00000,
  0b01100,
  0b00110,
  0b00011,
  0b00110,
  0b01100,
  0b00000 
},
{
  0b00000,
  0b00000,
  0b00110,
  0b01100,
  0b11000,
  0b01100,
  0b00110,
  0b00000
},
{
  0b00000,
  0b00000,
  0b00100,
  0b01110,
  0b11011,
  0b10001,
  0b00000,
  0b00000
},
{
  0b00000,
  0b00000,
  0b10001,
  0b11011,
  0b01110,
  0b00100,
  0b00000,
  0b00000
},
{
  0b00000,
  0b00000,
  0b00001,
  0b00011,
  0b10110,
  0b11100,
  0b01000,
  0b00000
}, 
{
  0b00100,
  0b01100,
  0b11111,
  0b01101,
  0b00101,
  0b00001,
  0b11111,
  0b00000
}};

class Display {
  private:
  

  public: 
    Display(LiquidCrystal_I2C lcd) {
    lcd.init();
    lcd.clear();
    lcd.backlight();
    lcd.noBlink();
    lcd.noCursor();
    lcd.setCursor(0, 0);

    for(int i = 0; i < 6; i++) {
      lcd.createChar(i, characters[i]);
      lcd.setCursor(i + 1, 0);      
      lcd.write(i);
    }
  }
};

void setup() {
  Serial.begin(9600);
  
  for(int i = 0; i < 4; i++) 
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(buzzerPin, OUTPUT);

  LiquidCrystal_I2C lcd(0x27, 16, 2);
  Display dis(lcd);
}

void loop() {
  // Get button inputs:
  for(int i = 0; i < 4; i++) {
    buttonPressed[i] = false;
    if(digitalRead(buttonPins[i]) == 0 && buttonHeld[i] == 0) {
      buttonPressed[i] = true;
      buttonHeld[i] = 1;
    } else if(digitalRead(buttonPins[i]) == 1) buttonHeld[i] = 0;
  }

  delay(500);
}
