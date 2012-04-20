![The Hack and Tell Logo on a Green Screen](http://distilleryimage9.instagram.com/46d1c8f2624611e1a87612313804ec91_7.jpg)

This project was done for Berlin Hack-And-Tell (http://www.meetup.com/berlin-hack-and-tell). This is monthly event where
hackers have 5 minutes to show off their projects. This count-down timer keeps track of the time.

This is the C++ code for an Arduino-based count-down timer. It displays a five minute countdown 
with big numbers on a TV screen. It uses Arduino TVOut library (see: http://code.google.com/p/arduino-tvout/ )
for PAL or NTSC composite video output. The time duration of the count-down can be changed in the source code.
When the count-down is over it presents a flashing message ("APPLAUSE") on the screen. In addition, a MOSFET can
be attached to a PWM-capable pin of the arduino. We use this to light a big "APPLAUSE" sign with several LEDs.

The user controls the device with a classic joystick like this:
![Competition Pro](http://upload.wikimedia.org/wikipedia/commons/thumb/0/0e/Competition_pro_first_version_45deg.png/220px-Competition_pro_first_version_45deg.png)

Apart from the count-down function the code implements a simple serial monitor and a message display.

The program has a graphical menu which is controlled by a classic digital joystick oder gamepad like those found on the Atari 2600 or those
for the Commodore 64. We use an ancient Competition Pro joystick.

