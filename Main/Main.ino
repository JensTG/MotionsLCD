#include <LiquidCrystal_I2C.h>
using namespace std;

#define E4 329
#define G4 392
#define A4 440
#define B4 493
#define PAUSE 0

unsigned int buttonPins[4] = {12, 13, 10, 11};
unsigned int buttonHeld[4] = {0, 0, 0, 0};
bool buttonPressed[4];
int buzzerPin = 6;

int fase = 0;
int dag = 0;
int akt = 0;
int rep = 1;
int done = 0;
int iter = 0;
int mIter = 0;
int mTarget = 0;
bool noTime = false;

unsigned int rockyNotes[12] = {E4, PAUSE, G4, PAUSE, A4, PAUSE, A4, PAUSE, B4, PAUSE, E4, PAUSE,};
float rockyDur[12] = {0.3, 0.0, 0.3, 0.2, 1.8, 0.3, 0.3, 0.0, 0.3, 0.2, 1.8, 0.3};
unsigned int rockyPauses[12];
unsigned int BPM = 80;
int mIndex = 0;

String days[7] = {"Mandag", "Tirsdag", "Onsdag", "Torsdag", "Fredag", "Loerdag", "Soendag"};
String aktivitet[7][3] = {
  {"Gaatur", "Legeplads", "Skovtur"},
  {"Lunges", "Staa paa et ben", "Baad"},
  {"Loebetur", "Knaeloeft", "Spraellemand"},
  {"Englehop", "Burpies", "Sjip"},
  {"Armboejninger", "Pull-ups", "Planke"},
  {"Lunges", "Wall-sit", "Squats"},
  {"Gaa udenfor", "Gaa udenfor", "Gaa udenfor"}
};
int reps[7][3] {
  {0, 0, 0},
  {4, 0, 0},
  {0, 0, 8},
  {6, 2, 0},
  {5, 2, 0},
  {4, 0, 6},
  {0, 0, 0}
};
int tid[7][3] {
  {0, 0, 0},
  {0, 20, 10},
  {0, 30, 0},
  {0, 0, 0},
  {0, 0, 10},
  {0, 15, 0},
  {0, 0, 0}
};

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

  int milliSecsPerNote = (int)(60000 / BPM);
  for(int i = 0; i < 12; i++) {
    rockyPauses[i] = rockyDur[i] * milliSecsPerNote;
  }

  // Intro:
  for(int i = 0; i <= 16 + max(lines[0].length(), lines[1].length()); i++) {
    lcd.clear();
    lcd.setCursor(i > lines[0].length() ? i - lines[0].length() : 0, 0);
    lcd.print(lines[0].substring(i > lines[0].length() ? 0 : lines[0].length() - i));

    lcd.setCursor(i <= 16 ? 16 - i : 0, 1);
    lcd.print(i - 16 < (int)lines[1].length() ? lines[1].substring(i <= 16 ? 0 : i - 16) : "");
    delay(250);
  }
}

void loop() {
  Beginning:
  iter = iter < 5000 ? iter + 1 : 0;
  delay(1);
  // Get button inputs:
  for(int i = 0; i < 4; i++) {
    buttonPressed[i] = false;
    if(digitalRead(buttonPins[i]) == 0 && buttonHeld[i] == 0) {
      buttonPressed[i] = true;
      buttonHeld[i] = 1;
    } else if(digitalRead(buttonPins[i]) == 1) buttonHeld[i] = 0;
  }

  // Play music:
  if(fase == 3) {
    if(mIter >= mTarget){
      if(rockyNotes[mIndex] != 0) tone(buzzerPin, rockyNotes[mIndex]);
      else noTone(buzzerPin);
      mIter = 0;
      mTarget = rockyPauses[mIndex];
      mIndex = mIndex < 12 ? mIndex + 1 : 0;
    }
  } else noTone(buzzerPin);
  mIter++;

  switch(fase) {
    case 0: goto Dag;
    case 1: goto Aktivitet;
    case 2: goto Rep;
    case 3: goto Udfrsel;
    case 4: goto Done;
  }

  Dag:
  noTone(buzzerPin);
  if(iter % 1000 == 0) {
    lcd.setCursor(0, 0);
    lcd.print(days[dag]);
  } else if(iter % 500 == 0) {
    lcd.clear();
  }

  if(buttonPressed[0]) dag = dag == 6 ? 0 : dag + 1;
  else if(buttonPressed[1]) dag = dag == 0 ? 6 : dag - 1;
  else if(buttonPressed[2]) {fase = 1; akt = 0; lcd.setCursor(0, 0); lcd.print(days[dag]);}

  goto Beginning;
  Aktivitet:
  noTone(buzzerPin);
  if(iter % 1000 == 0) {
    lcd.setCursor(0, 1);
    lcd.print(aktivitet[dag][akt]);
  } else if(iter % 500 == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(days[dag]);
  }

  if(buttonPressed[0]) akt = akt == 2 ? 0 : akt + 1;
  else if(buttonPressed[1]) akt = akt == 0 ? 2 : akt - 1;
  else if(buttonPressed[2]) {
    if(reps[dag][akt] == 0 && tid[dag][akt] == 0) {
      noTime = true;
      fase = 3;
      goto Beginning;
    }
    noTime = false;
    fase = 2; 
    rep = 1; 
    lcd.setCursor(0, 1); 
    lcd.print(aktivitet[dag][akt]); 
    lcd.setCursor(0, 0); 
    lcd.print(days[dag]);
    }
  else if(buttonPressed[3]) fase = 0;

  goto Beginning;
  Rep:
  noTone(buzzerPin);
  bool tidsbestemt = reps[dag][akt] == 0;
  int gentagelser = tidsbestemt ? tid[dag][akt] * rep : reps[dag][akt] * rep;
  if(iter % 1000 == 0) {
    if(tidsbestemt) {
      lcd.setCursor(gentagelser < 100 ? 13 : 12, aktivitet[dag][akt].length() < 11 ? 1 : 0);
      lcd.print(gentagelser);
      lcd.setCursor(15, aktivitet[dag][akt].length() < 11 ? 1 : 0);
      lcd.print("s");
    } else {
      lcd.setCursor(gentagelser < 10 ? 15 : 14, aktivitet[dag][akt].length() < 11 ? 1 : 0);
      lcd.print(gentagelser);
      lcd.setCursor(gentagelser < 10 ? 14 : 13, aktivitet[dag][akt].length() < 11 ? 1 : 0);
      lcd.print("x");
    }
  } else if(iter % 500 == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(days[dag]);
    lcd.setCursor(0, 1);
    lcd.print(aktivitet[dag][akt]);
  }

  if(buttonPressed[0]) rep = rep == 4 ? 1 : rep * 2;
  else if(buttonPressed[1]) rep = rep == 1 ? 4 : rep / 2;
  else if(buttonPressed[2]) {fase = 3; done = tidsbestemt ? gentagelser : 0;}
  else if(buttonPressed[3]) fase = 1;

  goto Beginning;
  Udfrsel:
  if(done < gentagelser && iter % 100 == 0 && !tidsbestemt) {
    lcd.clear();
    lcd.setCursor((int)((16 - aktivitet[dag][akt].length()) / 2), 0);
    lcd.print(aktivitet[dag][akt]);
    if(!noTime) {
      lcd.setCursor(6, 1);
      lcd.print(done);
      lcd.setCursor(8, 1);
      lcd.print("/");
      lcd.setCursor(9, 1);
      lcd.print(gentagelser);
    }
  } else if(tidsbestemt && done > 0 && iter % 1000 == 0) {
    lcd.clear();
    lcd.setCursor((int)((16 - aktivitet[dag][akt].length()) / 2), 0);
    lcd.print(aktivitet[dag][akt]);
    lcd.setCursor(6, 1);
    lcd.print(done);
    lcd.setCursor(9, 1);
    lcd.print("s");
    done--;
  }
    else if((!tidsbestemt && done == gentagelser) || (tidsbestemt && done == 0)) {
    lcd.clear();
    fase = 4;
    goto Beginning;
  }
  if(buttonPressed[0]) done++;
  else if(buttonPressed[1]) done = rep > 0 ? done - 1 : 0;
  else if(buttonPressed[2]) {fase = 4; lcd.clear();}
  else if(buttonPressed[3]) fase = 1;

  goto Beginning;
  Done:
  lcd.setCursor(0, 0);
  lcd.print("Godt klaret!");
  if(buttonPressed[2]) {
    dag = 0;
    akt = 0;
    done = 0;
    rep = 1;
    iter = 0;
    fase = 0;
    goto Beginning;
  }
}