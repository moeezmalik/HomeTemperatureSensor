#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ESP8266Ping.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>

//////////////////////////////////
////// CONSTANTS DEFINITONS //////
//////////////////////////////////

#define SERVER_IP "35.192.49.64"
#define STATUS_LED 2
#define DEVICE_ID "bbf1c9"

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
//////// TEMP SENSOR SETUP ///////
//////////////////////////////////

#define DHTPIN D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


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

bool getOutOfSetupMode = false;

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

    digitalWrite(STATUS_LED, LOW);
    delay(50);
    digitalWrite(STATUS_LED, HIGH);
    delay(50);
    digitalWrite(STATUS_LED, LOW);
    delay(50);
    digitalWrite(STATUS_LED, HIGH);
    delay(50);
    digitalWrite(STATUS_LED, LOW);
    delay(50);
    digitalWrite(STATUS_LED, HIGH);
    delay(50);

    delay(250);
  }
  
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

	while (WiFi.status() != WL_CONNECTED) {

    if(count == 20){
      setupMode();
      break;
    }
  
		digitalWrite(STATUS_LED, LOW);
		delay(1000);
		Serial.print(".");
		digitalWrite(STATUS_LED, HIGH);
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

bool checkInternetConnection(){
	if(Ping.ping("www.google.com"))
		return true;
	else
		return false;
}

HTTPClient http;

void performGETRequest(){

  Serial.println("Getting values from sensor");
  float incomingTemperature = dht.readTemperature();
	delay(100);
	float incomingHumidity = dht.readHumidity();

  Serial.println("Attempting to perform GET request");

  http.begin("http://" + String(SERVER_IP) + "/add?id=" + String(DEVICE_ID) + "&temperature=" + String(incomingTemperature)+"&humidity=" + String(incomingHumidity));

  int httpCode = http.GET();
  
  Serial.print("HTTP GET Code: ");
  Serial.println(httpCode);
  
  if(httpCode>0){
    if(httpCode == 200){
      Serial.println("Data sent");
      digitalWrite(STATUS_LED, HIGH);
      delay(250);
      digitalWrite(STATUS_LED, LOW);
      delay(250);
      digitalWrite(STATUS_LED, HIGH);
      delay(250);
      digitalWrite(STATUS_LED, LOW);
      delay(250);
      digitalWrite(STATUS_LED, HIGH);
      delay(250);
      digitalWrite(STATUS_LED, LOW);
      delay(250);
    } else{
      Serial.println("Server side error");
      digitalWrite(STATUS_LED, HIGH);
      delay(250);
      digitalWrite(STATUS_LED, LOW);
      delay(250);
    }
  } else{
    Serial.println("HTTP Client Error");
    digitalWrite(STATUS_LED, HIGH);
    delay(250);
    digitalWrite(STATUS_LED, LOW);
    delay(250);
  }
  http.end();
}


void setup() {

  //storeCredentialsInEEPROM("NIAQ", "indoorairquality");
  loadCredentialsFromEEPROM();

	//Starting DHT Sensor
	dht.begin();

	//Setting up Status LED
	pinMode(STATUS_LED, OUTPUT);
	digitalWrite(STATUS_LED, HIGH);

	//Starting Serial
	Serial.begin(9600);

 //setupMode();

	//Try to connect to WiFi
	if(tryToConnectWiFi()){
		Serial.println("Connected To Wifi");
		digitalWrite(STATUS_LED, LOW);
	}
  
}

void loop() {

	if(WiFi.status() != WL_CONNECTED){
		if(tryToConnectWiFi()){
		  digitalWrite(STATUS_LED, LOW);
		}
	} else{
		
		if(checkInternetConnection()){

			Serial.println("Internet Connected");
			performGETRequest();

		} else{
			Serial.println("Internet Not Connected");

			digitalWrite(STATUS_LED, LOW);
			delay(250);
			digitalWrite(STATUS_LED, HIGH);
			delay(250);
			digitalWrite(STATUS_LED, LOW);
			delay(250);
			digitalWrite(STATUS_LED, HIGH);
			delay(250);
			
		}

		
	}

	delay(20000);
  Serial.println("");

}