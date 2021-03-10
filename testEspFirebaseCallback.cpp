#include "testEspFirebaseCallback.h"

//Define FirebaseESP8266 data object
FirebaseData fbdo1;
FirebaseData fbdo2;

unsigned long sendDataPrevMillis = 0;

//String path = "/Test/Stream";
String path = "/count";

uint16_t count = 0;

void printResult(FirebaseData &data);
void printResult(StreamData &data);

void streamCallback(StreamData data) {

	Serial.printf("\n-------------------------------------\n         Atualiza��o Firebase            \n-------------------------------------\n");

	Serial.println("Stream Data1 available...");
	Serial.println("STREAM PATH: " + data.streamPath());
	Serial.println("EVENT PATH: " + data.dataPath());
	Serial.println("DATA TYPE: " + data.dataType());
	Serial.println("EVENT TYPE: " + data.eventType());
	Serial.print("VALUE: ");
	printResult(data);
	Serial.printf("\n-------------------------------------\n\n");
}

void streamTimeoutCallback(bool timeout) {
	if (timeout) {
		Serial.printf("\n\n-------------------------------------\n\n");
		Serial.println("Stream timeout, resume streaming...");
		Serial.printf("\n-------------------------------------\n\n");
	}
}

void setup() {

	Serial.begin(9600);

	pinMode(LED_8, OUTPUT);

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.print("Connecting to Wi-Fi");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(300);
	}
	Serial.println();
	Serial.print("Connected with IP: ");
	Serial.println(WiFi.localIP());
	Serial.println();

	Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
	Firebase.reconnectWiFi(true);

	//Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
	fbdo1.setBSSLBufferSize(1024, 1024);

	//Set the size of HTTP response buffers in the case where we want to work with large data.
	fbdo1.setResponseSize(1024);

	//Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
	fbdo2.setBSSLBufferSize(1024, 1024);

	//Set the size of HTTP response buffers in the case where we want to work with large data.
	fbdo2.setResponseSize(1024);

	if (!Firebase.beginStream(fbdo1, path)) {
		Serial.println("------------------------------------");
		Serial.println("Can't begin stream connection...");
		Serial.println("REASON: " + fbdo1.errorReason());
		Serial.println("------------------------------------");
		Serial.println();
	}

	Firebase.setStreamCallback(fbdo1, streamCallback, streamTimeoutCallback);
}

void loop() {

	if (millis() - sendDataPrevMillis > 15000) {

		digitalWrite(LED_8, !digitalRead(LED_8));

		sendDataPrevMillis = millis();
		count++;

		Serial.println("------------------------------------");
		Serial.println("Set JSON...");

//		json.add("data", "hello").add("num", count);
//		if (Firebase.setJSON(fbdo2, path + "/Json", json)) {
//			Serial.println("PASSED");
//			Serial.println("PATH: " + fbdo2.dataPath());
//			Serial.println("TYPE: " + fbdo2.dataType());
//			Serial.print("VALUE: ");
//			printResult(fbdo2);
//			Serial.println("------------------------------------");
//			Serial.println();
//		} else {
//			Serial.println("FAILED");
//			Serial.println("REASON: " + fbdo2.errorReason());
//			Serial.println("------------------------------------");
//			Serial.println();
//		}

//		json.add("value", count);

//		FirebaseJson updateData;
		FirebaseJson json;

		json.set("value", count);

		if (Firebase.updateNode(fbdo2, path, json)) {
			Serial.println("PASSED");
			Serial.println("PATH: " + fbdo2.dataPath());
			Serial.println("TYPE: " + fbdo2.dataType());
			Serial.print("VALUE: ");
			printResult(fbdo2);
			Serial.println("------------------------------------");
			Serial.println();
		} else {
			Serial.println("FAILED");
			Serial.println("REASON: " + fbdo2.errorReason());
			Serial.println("------------------------------------");
			Serial.println();
		}
	}
}

void printResult(FirebaseData &data) {

	if (data.dataType() == "int")
		Serial.println(data.intData());
	else if (data.dataType() == "float")
		Serial.println(data.floatData(), 5);
	else if (data.dataType() == "double")
		printf("%.9lf\n", data.doubleData());
	else if (data.dataType() == "boolean")
		Serial.println(data.boolData() == 1 ? "true" : "false");
	else if (data.dataType() == "string")
		Serial.println(data.stringData());
	else if (data.dataType() == "json") {
		Serial.println();
		FirebaseJson &json = data.jsonObject();
		//Print all object data
		Serial.println("Pretty printed JSON data:");
		String jsonStr;
		json.toString(jsonStr, true);
		Serial.println(jsonStr);
		Serial.println();
		Serial.println("Iterate JSON data:");
		Serial.println();
		size_t len = json.iteratorBegin();
		String key, value = "";
		int type = 0;
		for (size_t i = 0; i < len; i++) {
			json.iteratorGet(i, type, key, value);
			Serial.print(i);
			Serial.print(", ");
			Serial.print("Type: ");
			Serial.print(
					type == FirebaseJson::JSON_OBJECT ? "object" : "array");
			if (type == FirebaseJson::JSON_OBJECT) {
				Serial.print(", Key: ");
				Serial.print(key);
			}
			Serial.print(", Value: ");
			Serial.println(value);
		}
		json.iteratorEnd();
	} else if (data.dataType() == "array") {
		Serial.println();
		//get array data from FirebaseData using FirebaseJsonArray object
		FirebaseJsonArray &arr = data.jsonArray();
		//Print all array values
		Serial.println("Pretty printed Array:");
		String arrStr;
		arr.toString(arrStr, true);
		Serial.println(arrStr);
		Serial.println();
		Serial.println("Iterate array values:");
		Serial.println();
		for (size_t i = 0; i < arr.size(); i++) {
			Serial.print(i);
			Serial.print(", Value: ");

			FirebaseJsonData &jsonData = data.jsonData();
			//Get the result data from FirebaseJsonArray object
			arr.get(jsonData, i);
			if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
				Serial.println(jsonData.boolValue ? "true" : "false");
			else if (jsonData.typeNum == FirebaseJson::JSON_INT)
				Serial.println(jsonData.intValue);
			else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
				Serial.println(jsonData.floatValue);
			else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
				printf("%.9lf\n", jsonData.doubleValue);
			else if (jsonData.typeNum == FirebaseJson::JSON_STRING
					|| jsonData.typeNum == FirebaseJson::JSON_NULL
					|| jsonData.typeNum == FirebaseJson::JSON_OBJECT
					|| jsonData.typeNum == FirebaseJson::JSON_ARRAY)
				Serial.println(jsonData.stringValue);
		}
	} else if (data.dataType() == "blob") {

		Serial.println();

		for (int i = 0; i < data.blobData().size(); i++) {
			if (i > 0 && i % 16 == 0)
				Serial.println();

			if (i < 16)
				Serial.print("0");

			Serial.print(data.blobData()[i], HEX);
			Serial.print(" ");
		}
		Serial.println();
	} else if (data.dataType() == "file") {

		Serial.println();

		File file = data.fileStream();
		int i = 0;

		while (file.available()) {
			if (i > 0 && i % 16 == 0)
				Serial.println();

			int v = file.read();

			if (v < 16)
				Serial.print("0");

			Serial.print(v, HEX);
			Serial.print(" ");
			i++;
		}
		Serial.println();
		file.close();
	} else {
		Serial.println(data.payload());
	}
}

void printResult(StreamData &data) {

	if (data.dataType() == "int")
		Serial.println(data.intData());
	else if (data.dataType() == "float")
		Serial.println(data.floatData(), 5);
	else if (data.dataType() == "double")
		printf("%.9lf\n", data.doubleData());
	else if (data.dataType() == "boolean")
		Serial.println(data.boolData() == 1 ? "true" : "false");
	else if (data.dataType() == "string" || data.dataType() == "null")
		Serial.println(data.stringData());
	else if (data.dataType() == "json") {
		Serial.println();
		FirebaseJson *json = data.jsonObjectPtr();
		//Print all object data
		Serial.println("Pretty printed JSON data:");
		String jsonStr;
		json->toString(jsonStr, true);
		Serial.println(jsonStr);
		Serial.println();
		Serial.println("Iterate JSON data:");
		Serial.println();
		size_t len = json->iteratorBegin();
		String key, value = "";
		int type = 0;
		for (size_t i = 0; i < len; i++) {
			json->iteratorGet(i, type, key, value);
			Serial.print(i);
			Serial.print(", ");
			Serial.print("Type: ");
			Serial.print(
					type == FirebaseJson::JSON_OBJECT ? "object" : "array");
			if (type == FirebaseJson::JSON_OBJECT) {
				Serial.print(", Key: ");
				Serial.print(key);
			}
			Serial.print(", Value: ");
			Serial.println(value);
		}
		json->iteratorEnd();
	} else if (data.dataType() == "array") {
		Serial.println();
		//get array data from FirebaseData using FirebaseJsonArray object
		FirebaseJsonArray *arr = data.jsonArrayPtr();
		//Print all array values
		Serial.println("Pretty printed Array:");
		String arrStr;
		arr->toString(arrStr, true);
		Serial.println(arrStr);
		Serial.println();
		Serial.println("Iterate array values:");
		Serial.println();

		for (size_t i = 0; i < arr->size(); i++) {
			Serial.print(i);
			Serial.print(", Value: ");

			FirebaseJsonData *jsonData = data.jsonDataPtr();
			//Get the result data from FirebaseJsonArray object
			arr->get(*jsonData, i);
			if (jsonData->typeNum == FirebaseJson::JSON_BOOL)
				Serial.println(jsonData->boolValue ? "true" : "false");
			else if (jsonData->typeNum == FirebaseJson::JSON_INT)
				Serial.println(jsonData->intValue);
			else if (jsonData->typeNum == FirebaseJson::JSON_FLOAT)
				Serial.println(jsonData->floatValue);
			else if (jsonData->typeNum == FirebaseJson::JSON_DOUBLE)
				printf("%.9lf\n", jsonData->doubleValue);
			else if (jsonData->typeNum == FirebaseJson::JSON_STRING
					|| jsonData->typeNum == FirebaseJson::JSON_NULL
					|| jsonData->typeNum == FirebaseJson::JSON_OBJECT
					|| jsonData->typeNum == FirebaseJson::JSON_ARRAY)
				Serial.println(jsonData->stringValue);
		}
	} else if (data.dataType() == "blob") {

		Serial.println();

		for (int i = 0; i < data.blobData().size(); i++) {
			if (i > 0 && i % 16 == 0)
				Serial.println();

			if (i < 16)
				Serial.print("0");

			Serial.print(data.blobData()[i], HEX);
			Serial.print(" ");
		}
		Serial.println();
	} else if (data.dataType() == "file") {

		Serial.println();

		File file = data.fileStream();
		int i = 0;

		while (file.available()) {
			if (i > 0 && i % 16 == 0)
				Serial.println();

			int v = file.read();

			if (v < 16)
				Serial.print("0");

			Serial.print(v, HEX);
			Serial.print(" ");
			i++;
		}
		Serial.println();
		file.close();
	}
}
