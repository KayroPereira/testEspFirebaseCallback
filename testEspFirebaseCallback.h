
#ifndef _testEspFirebaseCallback_H_
#define _testEspFirebaseCallback_H_
	#include "Arduino.h"

	#include <ESP8266WiFi.h>
	#include <FirebaseESP8266.h>
	#include "security.h"

	#define LED_2			2			//GPIO2	-	D4

	#define OUT_8			12			//GPIO12 -	D6
	#define OUT_11			15			//GPIO15 -	D8

	#define BUTTON_9		13			//GPIO13 -	D7
	#define BUTTON_10		14			//GPIO14 -	D5

	#define LENGTH_PATH_FIREBASE		2
	const String PATH_FIREBASE[] = {"btn1", "btn2"};
	const uint8_t PATH_LEDS[] = {OUT_8, OUT_11};
	const uint8_t PATH_BUTTONS[] = {BUTTON_9, BUTTON_10};



	void printResult(FirebaseData &data);
	void printResult(StreamData &data);
	void streamCallback(StreamData data);
	void streamTimeoutCallback(bool timeout);
	void refreshFirebase();

#endif /* _testEspFirebaseCallback_H_ */
