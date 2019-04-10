#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

/*
 * Set up WiFi credentials below.
 * These credentials will be used to connect to the WiFi.
 */
#ifndef STASSID
#define STASSID "Zzzz"
#define STAPSK  "qazwsxedc111"
#endif

/*
 * The number of relays connected to nodemcu.
 * The number must be in the range 1 to 8
 */
#define RELAY_COUNT 8

/* 
 * Eight pins in this sketch are used for controlling relays.
 * Each pin can be used to switch a differnet relay.
 * 
 * The number of GPIO actually used will depend on the RELAY_COUNT value.
 * For example, if RELAY_COUNT is set to 4, only first four GPIO pins
 * will be used from the array defined below.
 * 
 * To change the GPIO pin numbers, follow format
 * const int relayPINS[8] = {PinOneNum, PinTwoNum, PinThreeNum, ..., PinEightNum};
 */
const int relayPINS[8] = {5,4,0,2,14,12,13,15};

/*
 * The name of the devices connected to each GPIO pin.
 * The name of device controlled by first element in relayPINS array
 * (defined above) will the first device name in the deviceName array
 * (defined below), second element in relayPINS array (defined above)
 *  will the second device name in the deviceName array, and so on.
 */
const String deviceName[8] = {
  "Device_1",
  "Device_2",
  "Device_3",
  "Device_4",
  "Device_5",
  "Device_6",
  "Device_7",
  "Device_8"
};

/*
 * Set the default behaviour of the relay GPIO pins.
 * By default every pin is set to LOW at boot.
 * To change this, set value either 0 or 1, 0 depicting LOW
 * and 1 depicting high in the below array.
 * The order of these values directly corresponds to the GPIO
 * pins in the relayPINS array
 */
int relayPINSstatus[8] = {0, 0, 0, 0, 0, 0, 0, 0};


/*
 * Sets up an HTTP server at the PORT number provided as argument.
 * If required, change PORT number here.
 * Default value: 80 (default HTTP PORT)
 */
ESP8266WebServer server(80);

const char* ssid = STASSID;
const char* password = STAPSK;

/*
 * Used to connect to WiFi.
 * Also, blinks on board LED which represents:
 *    - connecting: when blinking
 *    - connected: when glowing solid
 */
void connectWiFi(){
  Serial.print("\nConnecting to WiFi");
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.print(".");
  }
  // LED_BUILTIN is weird, it glows when the pin is LOW
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("\nConnected\nIP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {

  // begin serial output for debug
  Serial.begin(115200);

  // set required pins to output
  pinMode(LED_BUILTIN, OUTPUT);
  for (int x=0;x<RELAY_COUNT;x++) {
    pinMode(relayPINS[x], OUTPUT);
    // set the default PIN state to the relayPINSstatus declaration
    digitalWrite(relayPINS[x], relayPINSstatus[x]);
  }

  // set WiFi mode
  WiFi.mode(WIFI_STA);

  // connect to WiFi
  connectWiFi();

  if (MDNS.begin("esp8266"))
    Serial.println("MDNS responder started");
  
}

void loop() {
  
  // if WiFi is disconnected, try to reconnect
  if (WiFi.status() != WL_CONNECTED)  connectWiFi();

}
