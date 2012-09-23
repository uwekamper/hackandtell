#include <TVout.h>
#include <fontALL.h>
#include <avr/pgmspace.h>
#include "applause.h"
#include "hntlogo.h"
#include "pizza.h"
#include "hntlogo.h"

/**
 * A countdown timer for Berlin Hack-And-Tell.  
 */

TVout TV;

#define LEDPIN 3
#define FIREPIN 2
#define UPPIN 5
#define DOWNPIN 4

#define menuStart 8 // y coordinate where the menu border starts

int timerDefault = 300;
int timerState = timerDefault;

// byte inByte;
byte joyval;
byte selection;
// boolean state = false;
unsigned long now; // used for waiting loops

void setup() {
  pinMode(FIREPIN, INPUT);           // set pin to input
  digitalWrite(FIREPIN, HIGH);       // turn on pullup resistors
  pinMode(UPPIN, INPUT);           // set pin to input
  digitalWrite(UPPIN, HIGH);       // turn on pullup resistors
  pinMode(DOWNPIN, INPUT);           // set pin to input
  digitalWrite(DOWNPIN, HIGH);       // turn on pullup resistors

  pinMode(LEDPIN, OUTPUT);

  TV.begin(PAL, 120, 96);
  TV.select_font(font8x8);
}

void loop() {
  TV.clear_screen();
  TV.bitmap(10,0, hntlogo_bmp);
  TV.delay(500);

  analogWrite(LEDPIN, 254);

  TV.delay(500);
  analogWrite(LEDPIN, 0);
  TV.delay(500);

  joyval = showMainMenu();

  if (joyval == 0) {
    timerLoop();
  }
  else if (joyval == 1) {
    serialLoop();  
  }
  else if (joyval == 2) {
    pizzaLoop();
  }
  else if (joyval == 3) {
    logoLoop();
  }
}

void timerLoop() {
  while(true) {
    joyval = showTimerMenu();

    if (joyval == 0) {
      showApplause();
    }
    else if (joyval == 1) {
      countDown();
    }
    else if (joyval == 2) {
      timerState = timerDefault;
      countDown();
    }
    else if (joyval == 3) {
      changeTimeLoop();
    }
    else if (joyval == 4) {
      return;
    }
  }
}

void serialLoop() {
  byte inByte;
  Serial.begin(9600);
  TV.clear_screen();
  //TV.select_font(font8x8);
  TV.println(0,0, "Serial Monitor 9600 baud READY");
  while(true) {
    if (checkJoystick() == 1) {
      Serial.end();
      //TV.select_font(font8x8);
      return;
    }
    if (Serial.available() > 0) {
      inByte = Serial.read();
      TV.print(inByte);
    }

  } // end while
}

void logoLoop() {
  TV.clear_screen();
  TV.bitmap(10,0, hntlogo_bmp);

  // wait for 10 second (1000 ms)
  //unsigned long 
  now = millis();
  while( true ) { 
    if (checkJoystick() == 1) {
      return;
    } 
  }
}

void pizzaLoop() {
  boolean state = false;

  while (true) {
    if (!state) {
      TV.clear_screen();
      TV.print(0,0, "ENJOY THE FOOD");
      TV.bitmap(30,20, pizza_bmp);
    }
    else {
      TV.clear_screen();
      TV.bitmap(10,0, hntlogo_bmp);
    }

    // wait for 10 second (1000 ms)
    //unsigned long 
    now = millis();
    for(int i = 0; i < 100; i++) {

      while( millis() < (now + 10000)) { 
        if (checkJoystick() == 1) {
          return;
        } 
      }
    }
    state = !state;
  }
}

void changeTimeLoop() {
  TV.clear_screen();
  TV.print(0,0, "Use up/down");
  TV.print(0,10, "to change");
  TV.print(0,30, "Fire to save");
  TV.print(25, 62, "seconds");
  boolean state = true; // if changed, will redraw
  while(true) {

    if (state) {
      TV.draw_rect(25, 48, 55, 12, WHITE, BLACK);
      TV.print(40, 50, timerDefault);
      state = false;
    }

    joyval = checkJoystick();
    if (joyval == 0) {
      continue;
    }
    else if (joyval == 1) {
      break;
    }
    else if (joyval == 2) {
      timerDefault = timerDefault + 1;
      state = true;
    }
    else if (joyval == 3) {
      timerDefault = timerDefault - 1;
      state = true;
    }

    TV.delay(20);
  } // end while
  return;
}

int showMainMenu() {

  selection = 0;
  
  TV.draw_rect(0, 0, 120, 7, WHITE, WHITE);  
  TV.print(2,0, "MAIN MENU");
  TV.draw_line(0, menuStart, 119, menuStart, BLACK);
  TV.draw_rect(0, menuStart+1, 110, 60, WHITE, BLACK);  
  TV.println(12,menuStart+10, "COUNT-DOWN");
  TV.println(12,menuStart+20, "SERIAL MON.");
  TV.println(12,menuStart+30, "PIZZA");
  TV.println(12,menuStart+40, "F#$KING LOGO");

  TV.delay(500);
  drawMenuSelection(selection);
  while(true) {
    joyval = checkJoystick();
    if (joyval == 0) {
      continue;
    }
    else if (joyval == 1) {
      return selection;
    }
    else if (joyval == 2) {
      selection = (selection - 1) % 4;
    }
    else if (joyval == 3) {
      selection = (selection + 1) % 4;
    }
    if (selection < 0) 
      selection = 3;
    if (selection > 3)
      selection = 0;
    drawMenuSelection(selection);
    TV.delay(20);
  }

  return selection;
}

int showTimerMenu() {

  selection = 0;
  TV.draw_rect(0, 0, 128, 7, WHITE, WHITE);  
  TV.print(2,0, "TIMER");
  TV.draw_line(0, menuStart, 127, menuStart, BLACK);
  TV.draw_rect(0, menuStart+1, 102, 60, WHITE, BLACK); 
  TV.println(12,menuStart+10, "APPLAUSE"); 
  if (timerState == timerDefault) {
    TV.println(12,menuStart+20, "BEGIN");
  }
  else {
    TV.println(12,menuStart+20, "CONTINUE");
  }
  TV.println(12,menuStart+30, "RESET");
  TV.println(12,menuStart+40, "CHANGE TIME");
  TV.println(12,menuStart+50, "MAIN MENU");

  TV.delay(500);
  drawMenuSelection(selection);
  while(true) {
    joyval = checkJoystick();
    if (joyval == 0) {
      continue;
    }
    else if (joyval == 1) {
      return selection;
    }
    else if (joyval == 2) {
      selection = (selection - 1) % 5;
    }
    else if (joyval == 3) {
      selection = (selection + 1) % 5;
    }
    if (selection < 0) 
      selection = 4;
    if (selection > 4)
      selection = 0;
    drawMenuSelection(selection);
    TV.delay(20);
  }

  return selection;
}

inline void drawMenuSelection(int selection) {
  TV.draw_rect(3, menuStart+2, 8, 55, BLACK, BLACK);
  int y = menuStart + 10 + (selection * 10);
  TV.print(2, y, ">");
}

/**
 * returns 0, if nothing
 * returns 1, if fire was pressed,
 * returns 2, if up was pressed,
 * returns 3, if down was pressed.
 */
inline byte checkJoystick() {
  if (checkPin(FIREPIN) == true) {
    return 1;
  }
  else if (checkPin(UPPIN) == true) {
    return 2;
  }
  else if (checkPin(DOWNPIN) == true) {
    return 3;
  }
  return 0;
}

inline boolean checkPin(int pin) {
  int value = digitalRead(pin);

  if (value == LOW) {
    TV.delay(100);
    int value2 = digitalRead(pin);
    if (value2 == LOW) {
      return true;
    }
  }
  return false;  
}



// draw the two points between minutes and seconds
inline void drawPts() {
  TV.set_pixel(59, 50, WHITE);
  TV.set_pixel(59, 60, WHITE);
}

void countDown()
{
  byte delaycycles = 49;
  
  if (timerState == timerDefault)
  {
    TV.clear_screen();
    TV.print(20,50, "3");
    TV.delay(400);  
    TV.clear_screen();
    TV.print(28,50, "2");
    TV.delay(400);
    TV.clear_screen();
    TV.print(36,50, "1");
    TV.delay(400);
    TV.clear_screen();
    TV.print(44,50, "GO!");
    TV.delay(800);
  }
  TV.clear_screen();
  int seconds, minutes;
  while(timerState >= 0) {
    seconds = timerState % 60;
    minutes = timerState / 60;

    displayDigitsOf(seconds, true);
    displayDigitsOf(minutes, false);
    drawPts();
    // wait for 1 second (1000 ms)
    // unsigned long 
    now = millis();
    
    // time resolution is 16ms in PAL mode, therefor we do one second 
    // with 62 cycles (992 ms) and one second with 63 cycles (1008 ms)
    // to cancel out the error.
    for(int i = 0; i < delaycycles; i++) {
      
      if (checkJoystick() == 1) {
        return;
      } 
      TV.delay(16);
    }
    
    if (delaycycles == 49) {
      delaycycles = 50;
    }
    else {
      delaycycles = 49;
    }
    
    timerState--;  
  }
  timerState = timerDefault;
  showApplause();  
}

void showApplause() {
  TV.clear_screen();

  TV.bitmap(1, 45, applause_bmp);

  int currentLedState = 0;
  while (true) {
    if (currentLedState == 0) {
      currentLedState = 255;
      TV.draw_rect(0, 40, 118, 26, WHITE);
    }
    else {
      currentLedState = 0;
      TV.draw_rect(0, 40, 118, 26, BLACK);
    }
    analogWrite(LEDPIN, currentLedState);

    // wait for 1 second (1000 ms)
    //unsigned long 
    now = millis();
    while( millis() < (now + 300)) { 
      if (checkJoystick() >= 1) {
        analogWrite(LEDPIN, 0);
        return;
      }
    }
  }
}

void displayDigitsOf(int number, boolean seconds) {
  int ten = number / 10 % 10;
  int one = number      % 10;

  // seconds are always printed with leading zeros
  if(seconds) {
    drawNothing(60, 30);
    drawNumber(60, 30, ten);
    drawNothing(90, 30);
    drawNumber(90, 30, one);
  } 
  else {
    // do not print leading zeros
    if (ten < 1) {
      drawNothing(0, 30);
    }
    else {
      drawNothing(0, 30);
      drawNumber(0, 30, ten);
    }
    drawNothing(28, 30);
    drawNumber(28, 30, one);
  }
}

void drawNumber(int x, int y, int number)
{
  switch(number) {
  case 1:
    drawOne(x, y);
    break;
  case 2:
    drawTwo(x, y);
    break;
  case 3:
    drawThree(x, y);
    break;
  case 4:
    drawFour(x, y);
    break;
  case 5:
    drawFive(x, y);
    break;  
  case 6:
    drawSix(x, y);
    break;
  case 7:
    drawSeven(x, y);
    break;
  case 8:
    drawEight(x, y);
    break;
  case 9:
    drawNine(x, y);
    break;
  case 0:
    drawZero(x, y);
    break;
  }
}


void drawZero(int x, int y) {
  //TV.draw_rect(x, y, 30, 50, WHITE); 
  TV.draw_circle(x+15, y+13, 13, WHITE);
  TV.draw_circle(x+15, y+37, 13, WHITE);
  TV.draw_rect(x, y+16, 30, 20, BLACK, BLACK);
  TV.draw_line(x+2, y+16, x+2, y+36, WHITE);
  TV.draw_line(x+28, y+16, x+28, y+36, WHITE);
}

void drawOne(int x, int y) {
  //TV.draw_rect(x, y, 30, 50, WHITE); 
  TV.draw_line(x+10, y+15, x+25, y, WHITE);
  TV.draw_line(x+25, y, x+25, y+50, WHITE);
}

void drawTwo(int x, int y) {
  //TV.draw_rect(x, y, 30, 50, WHITE); 
  TV.draw_circle(x+15, y+13, 13, WHITE);
  TV.draw_rect(x, y+16, 30, 20, BLACK, BLACK);
  TV.draw_line(x+28, y+15, x+28, y+20, WHITE);
  TV.draw_line(x+28, y+20, x+2, y+50, WHITE);
  TV.draw_line(x+2, y+50, x+28, y+50, WHITE);
}

void drawThree(int x, int y) {
  //TV.draw_rect(x, y, 30, 50, WHITE); 
  TV.draw_circle(x+15, y+13, 13, WHITE);
  TV.draw_circle(x+15, y+37, 13, WHITE);
  TV.draw_rect(x, y+15, 20, 20, BLACK, BLACK);

}

void drawFour(int x, int y) {
  TV.draw_line(x+2, y, x+2, y+30, WHITE);
  TV.draw_line(x+2, y+30, x+28, y+30, WHITE);
  TV.draw_line(x+12, y+20, x+12, y+50, WHITE);
}

void drawFive(int x, int y) {
  TV.draw_circle(x+15, y+37, 13, WHITE);
  TV.draw_rect(x, y+15, 15, 28, BLACK, BLACK);
  TV.draw_line(x+5, y+23, x+16, y+23, WHITE);
  TV.draw_line(x+5, y+23, x+5, y, WHITE);
  TV.draw_line(x+5, y, x+28, y, WHITE);
}

void drawSix(int x, int y) {
  TV.draw_circle(x+15, y+13, 13, WHITE);
  TV.draw_rect(x, y+13, 30, 16, BLACK, BLACK);
  TV.draw_circle(x+15, y+37, 13, WHITE);
  TV.draw_line(x+2, y+13, x+2, y+37, WHITE);
}

void drawSeven(int x, int y) {
  //TV.draw_rect(x, y, 30, 50, WHITE); 
  TV.draw_line(x+2, y, x+28, y, WHITE);
  TV.draw_line(x+28, y, x+6, y+50, WHITE);
  TV.draw_line(x+10, y+25, x+24, y+25, WHITE);
}

void drawEight(int x, int y) {
  TV.draw_circle(x+15, y+13, 13, WHITE);
  TV.draw_circle(x+15, y+38, 13, WHITE);
}

void drawNine(int x, int y) {
  TV.draw_circle(x+15, y+37, 13, WHITE);
  TV.draw_rect(x, y+22, 30, 16, BLACK, BLACK);

  TV.draw_circle(x+15, y+13, 13, WHITE);
  TV.draw_line(x+28, y+13, x+28, y+38, WHITE);
}

void drawNothing(int x, int y) {
  TV.draw_rect(x, y, 30, 52, BLACK, BLACK);
}




