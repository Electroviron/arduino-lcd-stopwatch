// import module
#include <LiquidCrystal.h>

// :) tell the arduino which pins we chose
#define RS 9
#define EN 8

// bus pins
#define D4 4
#define D5 5
#define D6 6
#define D7 7

// Stop watch buttons
const int startButton = 12;
const int resetButton = 11;
const int stopButton = 10;

// create LCD object
LiquidCrystal lcd(9, 8, 4, 5, 6, 7);

// screen variables
int minCounter = 0;
int secCounter = 0;

// 
int lastStartButtonState = HIGH;
int lastResetButtonState = HIGH;
int lastStopButtonState = HIGH;

void setup() {
  lcd.begin(16, 2);        // this tells the library that the lcd has 16 columns and 2 rows
  lcd.setCursor(0, 1);     // column 0, row 1
  lcd.print("Iimer->00:00"); // initial screen state
  
  
  // configure button
  pinMode(startButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);
  pinMode(stopButton, INPUT_PULLUP);

}

// timing variable
unsigned long current_tms = 0;
unsigned long initial_tms = 0;
unsigned long current_ts = 0;
unsigned long current_tm = 0;

// stop watch state
bool running = false;

// custom control functions
void startBTNClick()
{

    if(current_tm >= 60){
      resetBTNClick();
    }
    if ((current_tms - initial_tms) >= 1000)
    {
        current_ts++;

        if (current_ts >= 60)
        {
            current_ts = 0;
            current_tm++;
            lcd.setCursor(7, 1);

            if(current_tm < 10){
              lcd.print("0");
            }
            lcd.print(current_tm);
        }

        lcd.setCursor(10, 1);

        if (current_ts < 10)
        {
            lcd.print("0");
        }

        lcd.print(current_ts);

        initial_tms = current_tms;
    }
}


void resetBTNClick(){
  running = false;
  current_ts = 0;
  current_tm = 0;

  lcd.setCursor(10, 1);
  lcd.print("00");

  lcd.setCursor(7, 1);
  lcd.print("00");
}

void loop() {
  current_tms = millis();

  // put your main code here, to run repeatedly:
  int startButtonState = digitalRead(startButton);
  int resetButtonState = digitalRead(resetButton);
  int stopButtonState = digitalRead(stopButton);

  // start button pressed
  if(startButtonState == LOW && lastStartButtonState == HIGH){
    // set state to running
    running = true;
  }

  // reset button pressed
  if(resetButtonState == LOW && lastResetButtonState == HIGH){
    resetBTNClick();
  }

  // stop button pressed
  if(stopButtonState == LOW && lastStopButtonState == HIGH){
     running = false;
  }

  if(running){
    startBTNClick();
  }

  lastStartButtonState = startButtonState;
  lastResetButtonState = resetButtonState;
  lastStopButtonState = stopButtonState;

}
