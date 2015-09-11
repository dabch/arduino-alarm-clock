#include <Wire.h>
#include "RTClib.h"

#include <EEPROM.h>

#define DATA_7SEG	2
#define CLOCK_7SEG	3
#define LATCH_7SEG	4

#define PIEZO_BUZZER	5

#define BUTTON_MODE	6
#define BUTTON_INCR	7
#define BUTTON_DECR	8

#define BUTTON_SNOOZE	9
#define SWITCH_ALARM	10 // HIGH (NC switch): alarm, LOW (switch grounded): no alarm


#define SNOOZE_TIME	5 // snooze time in minutes
#define BUZZER_DELAY	100 // time of buzz <-> silence during alarm
#define BLINK_TIME	500 // time needed for one blink in alarm set mode (in ms)
#define CHECK_TIME_ALARM	100 // check_alarm etc. will run every X loop runs
#define CHECK_TIME_BUTTONS	25 // check for pressed button will run every X loop runs

RTC_DS1307 rtc;

const int digits_pattern[11] = { // bytes to be sent to shift register in order to show digits (index is digit shown)
	B00000011, // zero
	B10011111, // one
	B00100101, // ...
	B00001101,
	B10011001,
	B01001001,
	B01000001,
	B00011111,
	B00000001,
	B00001001,
	B11111111}; // index 10: all off

DateTime alarm_time;
boolean alarm;
boolean alarm_stopped;
int snooze = 0;

int times_blinked = 0;

unsigned long loop_counter = 0;

void setup() {
	Serial.begin(9600);
	
	pinMode(DATA_7SEG, OUTPUT);
	pinMode(CLOCK_7SEG, OUTPUT);
	pinMode(LATCH_7SEG, OUTPUT);
	
	pinMode(PIEZO_BUZZER, OUTPUT);
	
	pinMode(BUTTON_MODE, INPUT_PULLUP);
	pinMode(BUTTON_INCR, INPUT_PULLUP);
	pinMode(BUTTON_DECR, INPUT_PULLUP);
	
	pinMode(BUTTON_SNOOZE, INPUT_PULLUP);
	pinMode(SWITCH_ALARM, INPUT_PULLUP);
	
	Wire.begin();
	
	rtc.begin(); // connect to RTC
	
	/*
	// Timer0 is already used for millis() - we'll just interrupt somewhere
	// in the middle and call the "Compare A" function below
	OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);
	*/
	
	// read hours and minutes of alarm time from eeprom, index 0x00 is hours, index 0x01 is minutes
	int hours = EEPROM.read(0);
	int minutes = EEPROM.read(1);
	alarm_time = DateTime(1970, 1, 1, hours, minutes, 0);
	Serial.println("initialized");
}

void loop() {
	if(loop_counter % CHECK_TIME_ALARM == 0) {
		check_alarm();
		if(alarm) {
			show_time(BUZZER_DELAY);
			analogWrite(PIEZO_BUZZER, 256/2); // buzzer full volume
			show_time(BUZZER_DELAY);
			check_snooze();
			analogWrite(PIEZO_BUZZER, 0);
		}
		show_time(0);
	}
	show_time(0);
	if(loop_counter % CHECK_TIME_BUTTONS == 0) {
		if(digitalRead(BUTTON_MODE) == LOW) { // if button pressed, enter input mode
			set_alarm_time();
			// write alarm time to EEPROM
			int hours = alarm_time.hour();
			int minutes = alarm_time.minute();
			EEPROM.write(0, hours);
			EEPROM.write(1, minutes);
		}
	}
	loop_counter++;
}


/*
// Interrupt is called once a millisecond, 
SIGNAL(TIMER0_COMPA_vect) {
	int val = millis();
	int digits[4];
	digits[0] = val % 10;
	digits[1] = (val / 10) % 10;
	digits[2] = (val / 100) % 10;
	digits[3] = (val / 1000) % 10;
	for(int i = 0; i < 4; i++) {
		shiftOut(DATA_7SEG, CLOCK_7SEG, LSBFIRST, digits_selector[i]);

		shiftOut(DATA_7SEG, CLOCK_7SEG, LSBFIRST, digits_pattern[digits[i]]);
		pulse(LATCH_7SEG);
		delay(1);
	}
}
*/
