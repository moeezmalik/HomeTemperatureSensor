#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#include "index.h"

const String ssid = "ESPAccess";
const String password = "thereisnospoon";

ESP8266WebServer server(80);

void handleRoot() {
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page

  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
}

void handleBlink() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);

  Serial.println(server.arg("hh"));
  server.send(200, "text/html", "HelloWorld!");
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

void getFromEEPROM() {

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

}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  delay(10);
  Serial.println('\n');

  WiFi.softAP(ssid, password);
  Serial.print("Access Point: ");
  Serial.println(ssid);

  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/ledblink", handleBlink);

  server.begin();
  Serial.println("HTTP Server Started");

  Serial.print("Size of SSID: ");
  Serial.println(sizeof(ssid));

  Serial.print("Size of Password: ");
  Serial.println(sizeof(password));

  storeCredentialsInEEPROM("GS7", "hashtagteamcracy");
  getFromEEPROM();
}

void loop() {
  server.handleClient();
}
