void pulse(int pin, boolean high_low = true); // function prototype needed to allow optional argument

/* generates a HIGH-LOW or LOW-HIGH pulse at the pin given */
void pulse(int pin, boolean high_low) {
	if(high_low) {
		digitalWrite(pin, HIGH);
		digitalWrite(pin, LOW);
	} else {
		digitalWrite(pin, LOW);
		digitalWrite(pin, HIGH);
	}
}

/* shows the 4 digits given in the argument int array on the 7-segment display */
void show_7seg(int digits[4], int ms) {
	unsigned long start_millis = millis();
	/*if(digits[3] == 0) { // if the 10-digit of the hours is 0, show nothing at all (6:40 instead of 06:40)
		digits[3] = 10;
	}*/
	do {
		for(int digit = 0; digit < 4; digit++) { // for each digit
				for(int i = 0; i < 4; i++) { // turn the right digit on
					digitalWrite(DATA_7SEG, i == digit);
					pulse(CLOCK_7SEG);
				}
				shiftOut(DATA_7SEG, CLOCK_7SEG, LSBFIRST, digits_pattern[digits[digit]]); // shiftOut the digit pattern
				pulse(LATCH_7SEG);
		}
	} while(millis() < start_millis + ms);
	
	for(int i = 0; i < 4; i++) { // turn all digits off to prevent differences in visible brightness
		digitalWrite(DATA_7SEG, LOW);
		pulse(CLOCK_7SEG);
		shiftOut(DATA_7SEG, CLOCK_7SEG, LSBFIRST, digits_pattern[10]);
		pulse(LATCH_7SEG);
	}
}

/* shows time on 7SEG for at least ms milliseconds (not accurate down to a single ms as multiplexing takes about 6ms, offset usually not more than 2 ms) */
void show_time(unsigned long ms) {
	unsigned long start_millis = millis();
	do {
		int digits[4];
	
		DateTime now = rtc.now();
		int hour = now.hour();
		digits[2] = hour % 10; // 
		digits[3] = (hour / 10) % 10;
		int min = now.minute();
		digits[0] = min % 10; // 
		digits[1] = (min / 10) % 10;
		
		show_7seg(digits, 0);
	} while(millis() < start_millis + ms);
	//int offset_ms = millis() - (start_millis + ms);
	//return offset_ms;
}

/* blinks for the given number of ms. if blink_minutes is false, the hours will blink and vice-versa */
void blink(int hours, int minutes, boolean blink_minutes, long ms) {
	int digits[4];
	unsigned long start_millis = millis();
	
	int non_blink = blink_minutes ? hours : minutes;
	int blink = blink_minutes ? minutes : hours;
	
	int digits_noblink = blink_minutes ? 2 : 0;
	int digits_blink = blink_minutes ? 0 : 2;
	
	digits[digits_noblink] = non_blink % 10; // non-blinking digit is easy
	digits[digits_noblink + 1] = (non_blink / 10) % 10;
	
	do {
		if(times_blinked <= BLINK_TIME / 10) {
			digits[digits_blink] = blink % 10;
			digits[digits_blink + 1] = (blink / 10) % 10;
			times_blinked++;
		} else if(times_blinked <= BLINK_TIME / 10 * 2) {
			digits[digits_blink] = 10; // all off
			digits[digits_blink + 1] = 10; // all off
			times_blinked++;
		} else {
			times_blinked = 0; // reset
		}
		show_7seg(digits, 10);
	} while (millis() < start_millis + ms);
}
