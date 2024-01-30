#include <LiquidCrystal_I2C.h>
using namespace std;

unsigned int buttonPins[4] = {12, 13, 10, 11};
unsigned int buttonHeld[4] = {0, 0, 0, 0};
bool buttonPressed[4];

int buzzerPin = 6;
int d = 25;

int fase = 0;
int dag = 0;
int aktivitet = 0;
int reps = 0;

String days[7] = {"Mandag", "Tirsdag", "Onsdag", "Torsdag", "Fredag", "Loerdag", "Soendag"};

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

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  
  for(int i = 0; i < 4; i++) 
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(buzzerPin, OUTPUT);

  lcd.init();
  lcd.clear();
  lcd.backlight();

  String lines[2] = {
    "Velkommen til",
    "Sundhedsskaermen"
  };

  for(int i = 0; i <= 16 + max(lines[0].length(), lines[1].length()); i++) {
    lcd.clear();
    lcd.setCursor(i > lines[0].length() ? i - lines[0].length() : 0, 0);
    lcd.print(lines[0].substring(i > lines[0].length() ? 0 : lines[0].length() - i));

    lcd.setCursor(i <= 16 ? 16 - i : 0, 1);
    lcd.print(i - 16 < (int)lines[1].length() ? lines[1].substring(i <= 16 ? 0 : i - 16) : "");
    delay(d * 10);
  }
}

void loop() {
  Beginning:
  delay(d);
  lcd.clear();
  // Get button inputs:
  for(int i = 0; i < 4; i++) {
    buttonPressed[i] = false;
    if(digitalRead(buttonPins[i]) == 0 && buttonHeld[i] == 0) {
      buttonPressed[i] = true;
      buttonHeld[i] = 1;
    } else if(digitalRead(buttonPins[i]) == 1) buttonHeld[i] = 0;
  }
  switch(fase) {
    case 0: goto Dag;
    case 1: goto Aktivitet;
    case 2: goto Rep;
  }

  Dag:
  lcd.setCursor(0, 0);
  lcd.print(days[dag]);

  if(buttonPressed[0]) dag = dag == 6 ? 0 : dag + 1;
  else if(buttonPressed[1]) dag = dag == 0 ? 6 : dag - 1;
  else if(buttonPressed[2]) fase = 1;

  goto Beginning;
  Aktivitet:

  if(buttonPressed[0]) dag = dag == 6 ? 0 : dag + 1;
  else if(buttonPressed[1]) dag = dag == 0 ? 6 : dag - 1;
  else if(buttonPressed[2]) fase = 2;
  else if(buttonPressed[3]) fase = 0;

  goto Beginning;
  Rep:;
}
