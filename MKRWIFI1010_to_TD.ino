/*
 This example connects MKR 1010 to an unencrypted Wifi network
 and send data to ARM Treasure Data.

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 modified 30 Oct 2018
 by Junko Nakajima
 */
#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
// ArduinoJson - Version: 5.13.3
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the Wifi radio's status

// ==========================================================================
// SECTION: Connect to WIFI
// ==========================================================================
void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

// ==========================================================================
// SECTION: Device Set up
// ==========================================================================
const char DEVICEID[] = "Arduino_MKR_WIFI_1010";
int tempPin = A1; //the analog pin the sensor's Vout (sense) pin is connected to

float get_temperature(){
  int reading = analogRead(tempPin);
  float voltage = reading * 3.3;
  voltage /= 1024.0;
  // Print tempeature in Celsius
  float temperatureC = (1.7 - voltage) * 100 ; //Change here to adjust to your sensor
  return temperatureC;
}


// ==========================================================================
// SECTION: API set up
// ==========================================================================

const char server[] = "in.treasuredata.com";
const char database[] = "iot_test/arduino"; // destination database
String path = "/postback/v3/event/"+String(database)+"?td_write_key="+String(SECRET_TD_API_KEY);

WiFiClient client;
HttpClient http = HttpClient(client, server);
int statusCode = 0;

// Json generator
StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
char jsonStr [200];

// ==========================================================================
// SECTION: Sense and send data loop
// ==========================================================================

void loop() {
  // Build JSON data string
  root["deviceid"] = DEVICEID;
  root["temp"] = get_temperature();
  
  root.printTo(jsonStr, 200);
  
  Serial.println(jsonStr);
  
  // Do the HTTP POST 
  http.post(path, "application/json", jsonStr);

  // Get status code
  statusCode = http.responseStatusCode();
  http.responseBody(); // flush response body

  Serial.print("Status code: ");
  Serial.println(statusCode);

  http.stop();

  delay(10000);
}

