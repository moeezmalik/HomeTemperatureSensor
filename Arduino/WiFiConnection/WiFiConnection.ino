#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include <PubSubClient.h>
#include <ESP8266Ping.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

//////////////////////////////////
////////////WIFI SETUP////////////
//////////////////////////////////

#ifndef STASSID
#define STASSID "GS7"
#define STAPSK  "hashtagteamcracy"
#endif

String ssid     = STASSID;
String password = STAPSK;

ESP8266WebServer server(80);

//////////////////////////////////
//////////PIN DEFINITONS//////////
//////////////////////////////////

const int statusLED = 2;

//////////////////////////////////
////////////JSON SETUP////////////
//////////////////////////////////

StaticJsonDocument<200> doc;
JsonObject root = doc.to<JsonObject>();


//////////////////////////////////
/////////TEMP SENSOR SETUP////////
//////////////////////////////////

#define DHTPIN D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//////////////////////////////////
/////////////MQTT SETUP///////////
//////////////////////////////////

WiFiClient espClient;
PubSubClient MQTTClient(espClient);

const char* mqttServer = "35.192.49.64";

void setup() {

  loadCredentialsFromEEPROM();

	//Setup MQTT Broker Destination
	MQTTClient.setServer(mqttServer, 1883);

	//Starting DHT Sensor
	dht.begin();

	//Setting up Status LED
	pinMode(statusLED, OUTPUT);
	digitalWrite(statusLED, HIGH);

	//Starting Serial
	Serial.begin(9600);

 //setupMode();

	//Try to connect to WiFi
	if(tryToConnectWiFi()){
		Serial.println("Connected To Wifi");
		digitalWrite(statusLED, LOW);
	}
  
}

void loop() {

	if(WiFi.status() != WL_CONNECTED){
		if(tryToConnectWiFi()){
		  digitalWrite(statusLED, LOW);
		}
	} else{
		Serial.println(WiFi.status());
		
		if(checkInternetConnection()){

			Serial.println("Internet Connected");

			if(!tryToConnectMQTTBroker()){
				Serial.println("Unable to connect to MQTT Broker after 5 failed attempts");
			} else{
				Serial.println("Connected to MQTT Broker");
				publishSensorValues();
				delay(10000);
			}
		} else{
			Serial.println("Internet Not Connected");

			digitalWrite(statusLED, LOW);
			delay(250);
			digitalWrite(statusLED, HIGH);
			delay(250);
			digitalWrite(statusLED, LOW);
			delay(250);
			digitalWrite(statusLED, HIGH);
			delay(250);
			
		}

		
	}

	delay(250);
}

bool tryToConnectWiFi(){
	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

 int count = 0;

	/* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
	 would try to act as both a client and an access-point and could cause
	 network-issues with your other WiFi-devices on your WiFi-network. */
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	Serial.println(WiFi.status());

	while (WiFi.status() != WL_CONNECTED) {

    if(count == 20){
      setupMode();
      break;
    }
  
		digitalWrite(statusLED, LOW);
		delay(1000);
		Serial.print(".");
		digitalWrite(statusLED, HIGH);
		delay(1000);
    count = count + 1;

	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.println(WiFi.status());

	return true;
}

bool publishSensorValues(){

	char jsonMessgeBuffer[200];

	float incomingTemperature = dht.readTemperature();
	delay(100);
	float incomingHumidity = dht.readHumidity();

	Serial.print("Temperature: ");
	Serial.println(incomingTemperature);
	Serial.print("Humidity: ");
	Serial.println(incomingHumidity);

	root["id"] = "bbf1c9";
	root["temperature"] = incomingTemperature;
	root["humidity"] = incomingHumidity;

	serializeJson(root, jsonMessgeBuffer);

	Serial.print(jsonMessgeBuffer);
	Serial.println();

	if(MQTTClient.publish("home/things/sensors/temperature", jsonMessgeBuffer)){
		Serial.println("Successfully Published");
		digitalWrite(statusLED, HIGH);
		delay(50);
		digitalWrite(statusLED, LOW);
		delay(50);
		digitalWrite(statusLED, HIGH);
		delay(50);
		digitalWrite(statusLED, LOW);
		delay(50);
		digitalWrite(statusLED, HIGH);
		delay(50);
		digitalWrite(statusLED, LOW);
		delay(50);
		digitalWrite(statusLED, HIGH);
		delay(50);
		digitalWrite(statusLED, LOW);
		delay(50);
	} else{
		Serial.println("Publish Attempt Failed");
	}

	return true;
}

bool tryToConnectMQTTBroker() {
	// Loop until we're reconnected

	int failureCount = 0;

	while (!MQTTClient.connected()) {

		Serial.print("Attempting MQTT connection...");

		//Create a random client ID
		String clientId = "ESP8266Client-";
		clientId += String(random(0xffff), HEX);

		// Attempt to connect
		if (MQTTClient.connect(clientId.c_str())) {
			Serial.println("Connected to MQTT Broker");
			digitalWrite(statusLED, LOW);
			return true;

		} else {
			Serial.print("failed, rc=");
			Serial.print(MQTTClient.state());
			Serial.println("Trying again in 5 Seconds");

			failureCount = failureCount + 1;
			Serial.print("Failure Count: ");
			Serial.println(failureCount);
		}

		if(failureCount > 4){
			return false;
		}

		for(int i = 0; i < 5; i++){
			digitalWrite(statusLED, LOW);
			delay(250);
			digitalWrite(statusLED, HIGH);
			delay(250);
			digitalWrite(statusLED, LOW);
			delay(250);
			digitalWrite(statusLED, HIGH);
			delay(250);
		}

		

	}

	return true;
}

bool checkInternetConnection(){
	if(Ping.ping("www.google.com"))
		return true;
	else
		return false;
}

bool getOutOfSetupMode = false;

void setupMode(){

  getOutOfSetupMode = false;
  
  Serial.println("Entering Setup Mode");
  String setupSSID = "ESPAccessPoint";
  String setupPassword = "thereisnospoon";

  Serial.print("Setup SSID: ");
  Serial.println(setupSSID);

  Serial.print("Setup Password: ");
  Serial.println(setupPassword);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(setupSSID, setupPassword);

  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/ledblink", handleBlink);
  server.begin();

  while(1){
    server.handleClient();
    if(getOutOfSetupMode){
      break;
    }

    digitalWrite(statusLED, LOW);
    delay(50);
    digitalWrite(statusLED, HIGH);
    delay(50);
    digitalWrite(statusLED, LOW);
    delay(50);
    digitalWrite(statusLED, HIGH);
    delay(50);
    digitalWrite(statusLED, LOW);
    delay(50);
    digitalWrite(statusLED, HIGH);
    delay(50);

    delay(250);
  }
  
}

void handleBlink() {
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);

  Serial.println(server.arg("hh"));
  server.send(200, "text/html", "HelloWorld!");

  storeCredentialsInEEPROM(server.arg("ssid"), server.arg("pwd"));
  loadCredentialsFromEEPROM();

  getOutOfSetupMode = true;
}

void storeCredentialsInEEPROM(String ssid, String password) {

  EEPROM.begin(100);

  int addr = 0;

  int ssidLength = ssid.length();
  int passwordLength = password.length();
  
  //Put sizes of SSID and Password
  EEPROM.put(addr, ssidLength);
  EEPROM.put(addr + sizeof(int), passwordLength);

  //Put SSID
  for(int i = 0; i < ssidLength; i++){
    EEPROM.put(addr + sizeof(int) + sizeof(int) + i, ssid[i]);
  }

  for(int i = 0; i < passwordLength; i++){
    EEPROM.put(addr + sizeof(int) + sizeof(int) + ssidLength + i, password[i]);
  }

  EEPROM.commit();
  
  Serial.println("Put in EEPROM Successful");

  
}

void loadCredentialsFromEEPROM() {

  EEPROM.begin(100);

  int addr = 0;
  
  int ssidLength = 0;
  int passwordLength = 0;

  EEPROM.get(addr, ssidLength);
  EEPROM.get(addr + sizeof(int), passwordLength);

  //Read SSID from memory
  char ssidFromEEPROM[ssidLength + 1];

  for(int i = 0; i < ssidLength; i++){
    EEPROM.get(addr + sizeof(int) + sizeof(int) + i, ssidFromEEPROM[i]);
  }
  ssidFromEEPROM[ssidLength] = '\0';

  //Read password from memory
  char passwordFromEEPROM[passwordLength + 1];

  for(int i = 0; i < passwordLength; i++){
    EEPROM.get(addr + sizeof(int) + sizeof(int) + ssidLength + i, passwordFromEEPROM[i]);
  }
  passwordFromEEPROM[passwordLength] = '\0';

  Serial.println("");
  Serial.print("SSID Length: ");
  Serial.println(ssidLength);
  Serial.print("Password Length: ");
  Serial.println(passwordLength);

  Serial.print("SSID: ");
  Serial.println(String(ssidFromEEPROM));

  Serial.print("Password: ");
  Serial.println(String(passwordFromEEPROM));

  ssid = String(ssidFromEEPROM);
  password = String(passwordFromEEPROM);
}
