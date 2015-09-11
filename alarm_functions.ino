void set_alarm_time() {
	int alarm_hours = alarm_time.hour();
	int alarm_minutes = alarm_time.minute();
	int digits[4];

	// show the current alarm time on the 7seg-display
	digits[2] = alarm_hours % 10;
	digits[3] = (alarm_hours / 10) % 10;
	digits[0] = alarm_minutes % 10;
	digits[1] = (alarm_minutes / 10) % 10;

	// wait until user lets go of button
	do {
		show_7seg(digits, 10);
	} while(digitalRead(BUTTON_MODE) == LOW);

	times_blinked = 0;
	// ######## SET ALARM HOURS ########
	do { // until mode button pressed again
		blink(alarm_hours, alarm_minutes, false, 0);
		if(digitalRead(BUTTON_INCR) == LOW) {
			times_blinked = 0; // reset times_blinked to avoid the updated numbern not being shown after pressing inc/decr button
			if(alarm_hours >= 23) { // avoid alarm_hours going over 23 (24 = 0)
				alarm_hours = 0;
			} else {
				alarm_hours++; // if between 0 and 22 -> increment by one
			}
			do { // wait for user to release button
				blink(alarm_hours, alarm_minutes, false, 0);
			} while(digitalRead(BUTTON_INCR) == LOW);
		} else if(digitalRead(BUTTON_DECR) == LOW) {
			times_blinked = 0; // reset times_blinked to avoid the updated numbern not being shown after pressing inc/decr button
			if(alarm_hours <= 0) { // avoid alarm_hours going below 0 (24 = 0)
				alarm_hours = 23;
			} else {
				alarm_hours--; // if between 23 and 1 -> decrement by one
			}
			do { // wait for user to release button
				blink(alarm_hours, alarm_minutes, false, 0);
			} while(digitalRead(BUTTON_DECR) == LOW);
		}
	} while(digitalRead(BUTTON_MODE) != LOW);

	do { // wait for user to release button
		blink(alarm_hours, alarm_minutes, true, 0); // blink the minutes now, not the hours
	} while(digitalRead(BUTTON_MODE) == LOW);

	times_blinked = 0;
	// ######## SET ALARM MINUTES ########
	do { // until mode button pressed again
		blink(alarm_hours, alarm_minutes, true, 0);
		if(digitalRead(BUTTON_INCR) == LOW) {
			times_blinked = 0; // reset times_blinked to avoid the updated number not being shown after pressing inc/decr button
			if(alarm_minutes >= 59) { // avoid alarm_minutes going over 59 (60 = 0)
				alarm_minutes = 0;
			} else {
				alarm_minutes++; // if between 0 and 58 -> increment by one
			}
			do { // wait for user to release button
				blink(alarm_hours, alarm_minutes, true, 0);
			} while(digitalRead(BUTTON_INCR) == LOW);
		} else if(digitalRead(BUTTON_DECR) == LOW) {
			times_blinked = 0; // reset times_blinked to avoid the updated number not being shown after pressing inc/decr button
			if(alarm_minutes <= 0) { // avoid alarm_hours going below 0 (59 = -1)
				alarm_minutes = 59;
			} else {
				alarm_minutes--; // if between 59 and 1 -> decrement by one
			}
			do { // wait for user to release button
				blink(alarm_hours, alarm_minutes, true, 0);
			} while(digitalRead(BUTTON_DECR) == LOW);
		}
		
	} while(digitalRead(BUTTON_MODE) != LOW);

	alarm_time = DateTime(1970, 1, 1, alarm_hours, alarm_minutes, 0);

	// wait for user to release button again
	do {
		show_time(10);
	} while(digitalRead(BUTTON_MODE) == LOW);
}

/* checks if alarm has been turned off TODO: snooze function */
void check_snooze() {
	if(digitalRead(SWITCH_ALARM) == LOW) { // check if alarm is still on
		alarm = false;
		alarm_stopped = true;
	} else if(digitalRead(BUTTON_SNOOZE) == LOW) { // snooze button pressed down
		
	}
}

/* checks if it is time to trigger the alarm */
void check_alarm() {
	if(digitalRead(SWITCH_ALARM) == LOW) { // if alarm turned off by switch
		return;
	}
	DateTime now = rtc.now();

	if(alarm_time.hour() == now.hour() && alarm_time.minute() == now.minute()) { // if time and alarm time are the same
		// trigger alarm if not triggered yet
		if(!alarm_stopped) {
			alarm = true;
		}
	} else {
		alarm_stopped = false;
	}
}
