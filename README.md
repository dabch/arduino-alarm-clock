# Arduino Alarm Clock
This is the code for an Arduino Nano alarm clock. It uses a 7 segment display to display the time, an RTC to keep time and several buttons and switches to set the alarm time and activate / deactivate the alarm function

**The following features have already been implemented:**
* time is stored in RTC (DS3231 or DS1307)
* current time is shown on 4-digit 7 segment display (connected via shift registers, only using 3 output pins)
* piezo buzzer for alarm
* alarm time can be set using three buttons
* alarm time is stored in EEPROM, so the last alarm will be restored after the Arduino is reset

**Planned features:**
* 60 single 3mm LEDs to display the seconds, connected using 8 shift registers (each second one LED will light up, after a minute is over they will go out and begin anew)
