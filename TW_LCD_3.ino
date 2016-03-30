

//#define LONGPRESS_LEN    200  // Min nr of loops for a long press
#define DEFAULT_LONGPRESS_LEN 500
#define DELAY            20  // Delay per loop in ms

#define HMI_TIME  10
#define LED_TIME  10
#define HMI_COOL  10
#define LCD_TIME  20
#define FAN_TIME  10



#define encoder0PinA  28
#define encoder0PinB  30
#define encoder1PinA 24
#define encoder1PinB  26

#define backlight_pin 6

#include <ButtonHandler.h>
#include <CountUpDownTimer.h>
#include <LiquidCrystal.h>
#include <Rotary.h>
#include <TimerOne.h>

enum {LEFT, RIGHT, ALL};
enum {TOP, MIDDLE};
enum {HMI, LED, FAN, BACKLIGHT};
enum {TIMER_VALUE, COOLDOWN_VALUE, COUNTDOWN_STATE, SSR_STATE, COOLDOWN_STATE};

boolean HMI_ON = false;
boolean HMI_COOLING = false;
boolean LED_ON = false;
boolean FAN_ON = false;

const char* Names[][4] = {
  {"HMI", " OFF", " ON","COOLDOWN"},
  {"LED", " OFF", " ON","COOLDOWN"},
  {"FAN", " OFF", " ON","COOLDOWN"}
};


int TimerStates[4][5] = {
  {420, 420, CD_STOP,false,false},                   // HMI TIMERVALUES, COOLDOWNVALUE, COUNTDOWN_STATE, SSR_STATE, COOLDOWN_STATE
  {400, 0, CD_STOP,false,false},                    // LED TIMERVALUES, COOLDOWNVALUE, COUNTDOWN_STATE, SSR_STATE, COOLDOWN_STATE
  {400, 0, CD_STOP,false,false},                    // FAN VTIMERVALUES, COOLDOWNVALUE, COUNTDOWN_STATE, SSR_STATE, COOLDOWN_STATE
  {5, 0, CD_STOP,false,false}                     // BACKLIGHT TIMERVALUES, COOLDOWNVALUE, COUNTDOWN_STATE, SSR_STATE, COOLDOWN_STATE
};

#define ButtonQuant 4
#define TimerQuant 5
#define SSRQuant 3
#define LcdXSplit 2
#define LcdYSplit 2

volatile boolean SSRStates[SSRQuant] = {false, false, false};
volatile int ButtonPins[ButtonQuant] = {29, 25, 27, 31};
volatile int SSR_Pin [SSRQuant] = {16, 17, 18};
volatile int ButtonEvent[ButtonQuant];
volatile int LcdLabelXPos[LcdXSplit] = {0, 10};
volatile int LcdLabelYPos[LcdYSplit] = {0, 2};


unsigned long previousMillis = 0;
const long interval = 500;

LiquidCrystal lcd(4, 5 , 0, 1, 2, 3);

CountUpDownTimer Timers[TimerQuant] = {
  CountUpDownTimer(DOWN),
  CountUpDownTimer(DOWN),
  CountUpDownTimer(DOWN),
  CountUpDownTimer(DOWN),
  CountUpDownTimer(DOWN)
};


ButtonHandler Buttons[ButtonQuant] = {
  ButtonHandler(ButtonPins[0], DEFAULT_LONGPRESS_LEN),
  ButtonHandler(ButtonPins[1], DEFAULT_LONGPRESS_LEN),
  ButtonHandler(ButtonPins[2], DEFAULT_LONGPRESS_LEN),
  ButtonHandler(ButtonPins[3], DEFAULT_LONGPRESS_LEN)
};

Rotary r = Rotary(encoder0PinA, encoder0PinB);
Rotary r2 = Rotary(encoder1PinA, encoder1PinB);


int LcdState[2];


int TimerCounters[TimerQuant];
int TimerTEMPStates[5];

int countButton = 0;

void TimerIsr() {

  // lcd.print("t");
  //LcdDisplay(RIGHT,TOP);
  //LcdDisplay(LEFT,TOP);

}

void setup()
{
  for (int x = 0; x < ButtonQuant ; x++) {
    pinMode(ButtonPins[x], INPUT);
  }
  for (int x = 0; x < SSRQuant ; x++) {
    pinMode(SSR_Pin[x], OUTPUT);
    digitalWrite(SSR_Pin[x], LOW);
  }

  pinMode(backlight_pin, OUTPUT);
  digitalWrite(backlight_pin, LOW);

  lcd.begin(20, 4);
  Serial.begin(9600);

  //Timer1.initialize(500000);
  //Timer1.attachInterrupt(TimerIsr);

  LcdState[LEFT] = HMI;
  LcdState[RIGHT] = LED;

  Timers[HMI].SetTimer(0, 0, TimerStates[HMI][TIMER_VALUE]);
  Timers[LED].SetTimer(0, 0, TimerStates[LED][TIMER_VALUE]);
  //Timers[COOLDOWN].SetTimer(0,0,TimerStates[COOLDOWN][TIMER_VALUE]);
  Timers[FAN].SetTimer(0, 0, TimerStates[FAN][TIMER_VALUE]);
  Timers[BACKLIGHT].SetTimer(0, 0, TimerStates[BACKLIGHT][TIMER_VALUE]);

  for (int x = 0; x < ButtonQuant ; x++) {
    Buttons[x].init();
  }

  updateTimer(LEFT);
  updateTimer(RIGHT);
  Timers[BACKLIGHT].StartTimer();

  LcdDisplay(LEFT, TOP);
  LcdDisplay(RIGHT, TOP);
 // lcd.setCursor(0, 1);
 // lcd.print(Timers[HMI].ShowClock());
}

void print_event(int button_name, int event)
{
  if (event)
  {
    //lcd.print(button_name);
    Timers[BACKLIGHT].ResetTimer();
    Timers[BACKLIGHT].StartTimer();
    switch (button_name) {
      case 0:
        switch (event) {
          case 1:
            Timers[LcdState[LEFT]].StartTimer();
            break;
          case 2:
            Timers[LcdState[LEFT]].ResetTimer();
            updateTimer(LEFT);
            break;
          break;
        }
        break;
      case  1:
        switch (event) {
          case 1:
            Timers[LcdState[RIGHT]].StartTimer();
            break;
          case 2:
            Timers[LcdState[RIGHT]].ResetTimer();
            updateTimer(RIGHT);
            break;
          break;
        }
        break;
      case 2:
        switch (event) {
          case 1:
            if (LcdState[RIGHT] < 2)
            {
              LcdState[RIGHT]++;
            }
            else  LcdState[RIGHT] = 0;
            break;
          case 2:
            if (LcdState[RIGHT] < 2)
            {
              LcdState[RIGHT]++;
            }
            else  LcdState[RIGHT] = 0;
            break;
          break;
        }
        break;
      case 3:
        switch (event) {
          case 1:
            if (LcdState[LEFT] < 2)
            {
              LcdState[LEFT]++;
            }
            else  LcdState[LEFT] = 0;
            break;
          case 2:
            if (LcdState[LEFT] < 2)
            {
              LcdState[LEFT]++;
            }
            else  LcdState[LEFT] = 0;
            break;
          break;
        }
        break;
      break;
    }
    LcdDisplay(LEFT, TOP);
    LcdDisplay(RIGHT, TOP);
  }
  //Serial.print(".SL"[event]);
  //lcd.print(".SL"[event]);
}

void loop()
{
  unsigned long currentMillis = millis();
  unsigned char result = r.process();
  unsigned char result2 = r2.process();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Timers[HMI].Timer();
    Timers[LED].Timer();
    Timers[FAN].Timer();
    //Timers[COOLDOWN].Timer();
    Timers[BACKLIGHT].Timer();
    LcdDisplay(LEFT, TOP);
    LcdDisplay(RIGHT, TOP);
    updateTimer(ALL);
  }
  if (Timers[BACKLIGHT].ShowClock() > 0) digitalWrite(backlight_pin, HIGH);
  else digitalWrite(backlight_pin, LOW);

  if (countButton > 40)
  {
    for (int i = 0; i < ButtonQuant; i++) {
      ButtonEvent[i] = Buttons[i].handle();
      //LcdDisplay();

      // do other things
      print_event(i, ButtonEvent[i]);

      countButton = 0;
    }
  }
  countButton++;
  static int counter = 0;








  if (result2) {
    Timers[BACKLIGHT].ResetTimer();
    Timers[BACKLIGHT].StartTimer();
    if (result2 == DIR_CW) {
      TimerCounters[LcdState[RIGHT]]++;
    }
    if (result2 == DIR_CCW) {
      TimerCounters[LcdState[RIGHT]]--;
    }
    Timers[LcdState[RIGHT]].SetTimer(TimerCounters[LcdState[RIGHT]]);
    LcdDisplay(RIGHT, TOP);
  }


  if (result /*&& timer1_state != CD_FINISHED*/) {
    Timers[BACKLIGHT].ResetTimer();
    Timers[BACKLIGHT].StartTimer();
    if ((result == DIR_CW) && (TimerStates[LcdState[LEFT]][COOLDOWN_STATE]) == false) {
    TimerCounters[LcdState[LEFT]]++;
    }
    if ((result == DIR_CCW) && (TimerStates[LcdState[LEFT]][COOLDOWN_STATE]) == false){
    TimerCounters[LcdState[LEFT]]--;
    }
    Timers[LcdState[LEFT]].SetTimer(TimerCounters[LcdState[LEFT]]);
    LcdDisplay(LEFT,TOP);
  }
}


void updateTimer(int LcdPos)
{
  int TimerSelect = LcdState[LcdPos];
  TimerCounters[TimerSelect] = Timers[TimerSelect].ShowClock();
  for (int i = 0; i < 3; i++) {
    TimerStates[i][COUNTDOWN_STATE] = Timers[i].StateTimer();

    switch (TimerStates[i][COUNTDOWN_STATE]) {
      case CD_RUNNING:
        Timers[BACKLIGHT].ResetTimer();
        Timers[BACKLIGHT].StartTimer();
        if (TimerStates[i][COOLDOWN_STATE]==false) {
          digitalWrite(SSR_Pin[i], HIGH);
          TimerStates[i][SSR_STATE] = true;
        }
        break;
      case CD_STOP:
        digitalWrite(SSR_Pin[i], LOW);
        TimerStates[i][SSR_STATE] = false;
        break;
      case CD_PAUSED:
        digitalWrite(SSR_Pin[i], LOW);
        TimerStates[i][SSR_STATE] = false;
        break;
      case CD_FINISHED:
        digitalWrite(SSR_Pin[i], LOW);
        TimerStates[i][SSR_STATE] = false;
        //        if (TimerTEMPStates[BACKLIGHT] != CD_FINISHED)
        //        {
        //          Timers[BACKLIGHT].ResetTimer();
        //          Timers[BACKLIGHT].StartTimer();
        //        }
        //lcd.setCursor(10,1);
        //lcd.print(TimerStates[i][COOLDOWN_VALUE]);
        if (TimerStates[i][COOLDOWN_STATE]==true){
          Timers[i].SetTimer(TimerStates[i][TIMER_VALUE]);
          TimerStates[i][COOLDOWN_STATE]=false;
          
          //Timers[i].StartTimer();
        }
        else if (TimerStates[i][COOLDOWN_VALUE]>0){
          Timers[i].SetTimer(TimerStates[i][COOLDOWN_VALUE]);
          TimerStates[i][COOLDOWN_STATE]=true;
          Timers[i].StartTimer();
        }
        else {
          Timers[i].ResetTimer();
        }
        break;
      break;
    }
  }
}


void LcdDisplay (int Index_X, int Index_Y)
{
  lcd.setCursor(LcdLabelXPos[Index_X], LcdLabelYPos[Index_Y]);

  lcd.print("         ");
  lcd.setCursor(LcdLabelXPos[Index_X], LcdLabelYPos[Index_Y]);

  lcd.print(Names[LcdState[Index_X]][0]);
  if (TimerStates[LcdState[Index_X]][SSR_STATE]==true) lcd.print(" ON");
  else lcd.print(" OFF");
  if (TimerStates[LcdState[Index_X]][COOLDOWN_STATE]==true){
    lcd.setCursor(LcdLabelXPos[Index_X],1);
    lcd.print("COOLDOWN");
    lcd.setCursor(LcdLabelXPos[Index_X], LcdLabelYPos[Index_Y]);
  }
  else {
    lcd.setCursor(LcdLabelXPos[Index_X],1);
    lcd.print("READY     ");
    lcd.setCursor(LcdLabelXPos[Index_X], LcdLabelYPos[Index_Y]);
  }
  //lcd.print(LcdState[Index_X]);

  lcd.setCursor(LcdLabelXPos[Index_X], LcdLabelYPos[Index_Y] + 2);
  lcd.print("        ");
  lcd.setCursor(LcdLabelXPos[Index_X], LcdLabelYPos[Index_Y] + 2);

  //lcd.print(LcdState[Index_X]);
  lcd.print(Timers[LcdState[Index_X]].ShowHours());
  lcd.print(":");
  lcd.print(Timers[LcdState[Index_X]].ShowMinutes());
  lcd.print(":");
  lcd.print(Timers[LcdState[Index_X]].ShowSeconds());


}





