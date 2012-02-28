#include <TVout.h>
#include <fontALL.h>
#include <avr/pgmspace.h>
#include "applause.h"
#include "hntlogo.h"
#include "pizza.h"

/**
 * A countdown timer for Berlin Hack-And-Tell.  
 */

TVout TV;
int ledPin = 3;

int firePin = 2;
int upPin = 5;
int downPin = 4;

int menuStart = 8; // y coordinate where the menu border starts

int timerDefault = 300;
int timerState = timerDefault;

char sponsorName[] = "SysEleven";
byte inByte;

void setup() {
  
  
  
  pinMode(firePin, INPUT);           // set pin to input
  digitalWrite(firePin, HIGH);       // turn on pullup resistors

  pinMode(upPin, INPUT);           // set pin to input
  digitalWrite(upPin, HIGH);       // turn on pullup resistors

  pinMode(downPin, INPUT);           // set pin to input
  digitalWrite(downPin, HIGH);       // turn on pullup resistors

  pinMode(ledPin, OUTPUT);

  TV.begin(PAL);
  
  TV.select_font(font8x8);
}


void loop() {
  
  TV.clear_screen();
  TV.bitmap(14,0, hntlogo_bmp);
  TV.delay(500);
 
  analogWrite(ledPin, 255);
  TV.delay(500);
  analogWrite(ledPin, 0);
  TV.delay(500);
  

  int selection = showMainMenu();

  if (selection == 0) {
    timerLoop();
  }
  else if (selection == 1) {
    serialLoop();  
  }
  else if (selection == 2) {
    pizzaLoop();
  }
}

void timerLoop() {
  while(true) {
    int selection = showTimerMenu();
    
    if (selection == 0) {
      countDown();
    }
    else if (selection == 1) {
      timerState = timerDefault;
      countDown();
    }
    else if (selection == 2) {
      return;
    }
  }
}

void serialLoop() {
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

void pizzaLoop() {
  boolean state = false;
  while (true) {
    if (!state) {
      TV.clear_screen();
      TV.print(0,0, "ENJOY THE PIZZA");
      TV.bitmap(30,20, pizza_bmp);
      TV.print(0,80, "SPONSORED BY:");
      TV.print(0,88, sponsorName);
    }
    else {
      TV.clear_screen();
      TV.bitmap(14,0, hntlogo_bmp);
    }
    
    // wait for 10 second (1000 ms)
    unsigned long now = millis();
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


int showMainMenu() {

  int selection = 0;
  TV.draw_rect(0, 0, 128, 7, WHITE, WHITE);  
  TV.print(2,0, "MAIN MENU");
  TV.draw_line(0, menuStart, 127, menuStart, BLACK);
  TV.draw_rect(0, menuStart+1, 102, 50, WHITE, BLACK);  
  TV.println(12,menuStart+10, "COUNT-DOWN");
  TV.println(12,menuStart+20, "SERIAL MON.");
  TV.println(12,menuStart+30, "PIZZA");

  TV.delay(500);
  drawMenuSelection(selection);
  while(true) {
    int joyval = checkJoystick();
    if (joyval == 0) {
      continue;
    }
    else if (joyval == 1) {
      return selection;
    }
    else if (joyval == 2) {
      selection = (selection - 1) % 3;
    }
    else if (joyval == 3) {
      selection = (selection + 1) % 3;
    }
    if (selection < 0) 
      selection = 2;
    if (selection > 2)
      selection = 0;
    drawMenuSelection(selection);
    TV.delay(20);
  }

  return selection;
}

int showTimerMenu() {

  int selection = 0;
  TV.draw_rect(0, 0, 128, 7, WHITE, WHITE);  
  TV.print(2,0, "TIMER");
  TV.draw_line(0, menuStart, 127, menuStart, BLACK);
  TV.draw_rect(0, menuStart+1, 102, 50, WHITE, BLACK);  
  if (timerState == timerDefault) {
    TV.println(12,menuStart+10, "BEGIN");
  }
  else {
    TV.println(12,menuStart+10, "CONTINUE");
  }
  TV.println(12,menuStart+20, "RESET");
  TV.println(12,menuStart+30, "MAIN MENU");

  TV.delay(500);
  drawMenuSelection(selection);
  while(true) {
    int joyval = checkJoystick();
    if (joyval == 0) {
      continue;
    }
    else if (joyval == 1) {
      return selection;
    }
    else if (joyval == 2) {
      selection = (selection - 1) % 3;
    }
    else if (joyval == 3) {
      selection = (selection + 1) % 3;
    }
    if (selection < 0) 
      selection = 2;
    if (selection > 2)
      selection = 0;
    drawMenuSelection(selection);
    TV.delay(20);
  }

  return selection;
}


void drawMenuSelection(int selection) {
  TV.draw_rect(3, menuStart+2, 8, 40, BLACK, BLACK);
  int y = menuStart + 10 + (selection * 10);
  TV.print(2, y, ">");
}

/**
 * returns 0, if nothing
 * returns 1, if fire was pressed,
 * returns 2, if up was pressed,
 * returns 3, if down was pressed.
 */
int checkJoystick() {
  if (checkPin(firePin) == true) {
    return 1;
  }
  else if (checkPin(upPin) == true) {
    return 2;
  }
  else if (checkPin(downPin) == true) {
    return 3;
  }
  return 0;
}

boolean checkPin(int pin) {
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
void drawPts() {
  TV.set_pixel(64, 50, WHITE);
  TV.set_pixel(64, 60, WHITE);
}

void countDown()
{
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
  drawPts();
  int seconds, minutes;
  while(timerState >= 0) {
    seconds = timerState % 60;
    minutes = timerState / 60;

    displayDigitsOf(seconds, true);
    displayDigitsOf(minutes, false);

    // wait for 1 second (1000 ms)
    unsigned long now = millis();
    for(int i = 0; i < 100; i++) {

      while( millis() < (now + 1000)) { 
        if (checkJoystick() == 1) {
          return;
        } 
      }
    }
    timerState--;  
  }
  timerState = timerDefault;
  TV.clear_screen();
  
  TV.bitmap(9, 45, applause_bmp);

  int currentLedState = 0;
  while (true) {
    if (currentLedState == 0) {
      currentLedState = 255;
      TV.draw_rect(5, 40, 122, 26, WHITE);
    }
    else {
      currentLedState = 0;
      TV.draw_rect(5, 40, 122, 26, BLACK);
    }
    analogWrite(ledPin, currentLedState);
    
    // wait for 1 second (1000 ms)
    unsigned long now = millis();
    while( millis() < (now + 300)) { 
      if (checkJoystick() >= 1) {
        analogWrite(ledPin, 0);
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
    drawNothing(66, 30);
    drawNumber(66, 30, ten);
    drawNothing(97, 30);
    drawNumber(97, 30, one);
  } 
  else {
    // do not print leading zeros
    if (ten < 1) {
      drawNothing(2, 30);
    }
    else {
      drawNothing(2, 30);
      drawNumber(2, 30, ten);
    }
    drawNothing(32, 30);
    drawNumber(32, 30, one);
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



