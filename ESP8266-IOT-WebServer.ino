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
const char* ssid = STASSID;
const char* password = STAPSK;

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
int relayPINSstatus[8] = {0, 0, 0, 0, 1, 0, 1, 1};


/*
 * Initialize HTTP server at the PORT number provided as argument.
 * If required, change PORT number here.
 * Default value: 80 (default HTTP PORT)
 */
ESP8266WebServer server(80);

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


/* 
 * Updates the current status of GPIO pins in the relayPINS
 * into relayPINSstatus
 */
void readPINstatus(){
  for (int x=0; x<RELAY_COUNT; x++)
    relayPINSstatus[x] = digitalRead(relayPINS[x]);
}

/*
 * Writes pin output based on values in relayPINSstatus
 */
void updatePINoputput(){
  for (int x=0; x<RELAY_COUNT; x++)
    digitalWrite(relayPINS[x], relayPINSstatus[x]);
}

/*
 * Changes the state of the PIN
 */
int changePinState(int pinNo, int state){
  int x;
  // check if pin exists in relayPINS and also get the index
  for (x=0; x<RELAY_COUNT; x++){
    if (relayPINS[x] == pinNo){
      break;
    }
  }
  if (x == RELAY_COUNT) return -1;
  else if (state == 1 || state == 0){
    relayPINSstatus[x] = state;
    updatePINoputput();
    return 1;
  }
  else {
    return -2;
  }
}

/*
 * Converts the status of the PINS into JSON which is later
 * sent to the browser via AJAX
 */
String buildJSON(){
  String res = "[";
  for (int x=0; x<RELAY_COUNT; x++){
    res += "{\"deviceName\":\"";
    res +=  deviceName[x];
    res += "\",\"pinNumber\":";
    res += relayPINS[x];
    res += ",\"pinStatus\":";
    res += relayPINSstatus[x];
    res += "}";
    if (x+1 < RELAY_COUNT) res+= ",";
  }
  res += "]";
  return res;
}

/*
 * Send back HTTP page to the browser on GET request to "/".
 * e.g, http://esp8266.local/
 */
void handleRoot(){
  Serial.print("Recieved root GET request... ");
  String page = "<!DOCTYPE html><html lang='en'><head> <meta charset='UTF-8'> \
  <meta name='viewport' content='width=device-width, initial-scale=1.0'> \
  <meta http-equiv='X-UA-Compatible' content='ie=edge'> <title>ESP8266 Control Panel\
  </title> <style>*{padding: 0; margin: 0; font-family: Helvetica, Arial, \
  sans-serif;}html, body{min-height: 100% !important; height: 100%;}body{background: \
  #252525; display: flex; flex-direction: column;}#loader{z-index: 100; position: \
  absolute; width: 100%; height: 100%; background: #252525; display: flex; color: \
  white; align-items: center; transition: all 0.5s linear;}#loader span{margin: 0 \
  auto;}.container{max-width: 900px; margin: 0 auto;}header{background: rgb(20, 20, \
  20); padding: 10px; color: white; align-content: center; box-shadow: 0 8px 6px \
  -6px black; flex-grow: 0;}header div{display: flex; justify-content: space-around;}.\
  logo{font-weight: 200; display: inline-block; cursor: pointer; font-style: italic; \
  font-size: 1.5em;}.switchboard{flex-grow: 1; display: flex; flex-direction: row; \
  align-items: center;}.summary{display: flex; flex-direction: row; padding: 20px; \
  flex-wrap: wrap; align-items: center; justify-content: center;}.device{flex-basis: \
  250px; background: rgba(0, 0, 0, 0.7); padding: 20px; margin: 20px; transition: \
  background 0.25s linear; position: relative; box-shadow: 0 8px 6px -6px black; \
  cursor: pointer; color: white;}.device:hover{top: 1px; background: rgba(0, 0, 0, \
  0.8); box-shadow: 0 4px 6px -6px black;}.device:active{top: 4px; box-shadow: none;}\
  .device .name{font-size: 1.1em; pointer-events: none;}.device .pin{font-style: italic; \
  font-size: 0.8em; margin: 10px 0px; pointer-events: none;}.on{background-color: \
  rgba(255, 255, 0, 1); color: black;}.on:hover{background-color: rgba(255, 255, 0, \
  0.9);}h1, h2{text-align: center;}@media only screen and (max-width: 800px){.device{margin: \
  5px;}.summary{padding: 0px;}}@media only screen and (max-width: 650px){.device{margin: \
  10px 5px; flex-basis: 300px;}.summary{padding: 0px;}}</style></head><body> <div id='loader'> \
  <span> <span class='logo'>nodemcu |</span> <span>loading...</span> </span> </div><header> \
  <div class='container'> <span> <span class='logo'>nodemcu |</span> <span>control panel</span> \
  </span> </div></header> <section class='container switchboard'> <div class='summary' id='pins'>\
  </div></section></body><script type='text/javascript'> getPinState=new Promise( \
  ((resolve, reject)=>{fetch('/status').then(res=>{if (res.status !==200) throw new Error \
  (res.status); else resolve(res.json())}).catch(err=> reject('An error occured while \
  fetching data'))}) ); function addDevices(pinState){let str=''; pinState.forEach(pin=>\
  {str +='<span class=\"device '; str +=(pin.pinStatus==1)?\"on\":\"\"; str +='\" pinno='\
  + pin.pinNumber +'><div class=\"name\">' + pin.deviceName + '</div><div class=\"pin\">\
  GPIO<span class=\"no\">' + pin.pinNumber + '</span></div></span>';}); document.getElementById\
  ('pins').innerHTML=str;}function changePinState(pinEle){let sendArgs='pinNumber='+Number\
  (pinEle.getAttribute('pinno'))+\"&state=\"; sendArgs +=pinEle.classList.contains('on')?0:1; \
  fetch('/status',{method: 'POST', body: sendArgs, headers:{'Content-Type' : \
  'application/x-www-form-urlencoded'}}).then((res)=>{if (res.status !==200) throw new Error \
  (res.status);}).catch((err)=>{error(err);}); pinEle.classList.toggle('on');}function error(err)\
  {document.getElementById('loader').innerHTML='<span><h2>Error occured while communicating to nodemcu.\
  <h3><br><h1>RELOADING</h3></span>'; console.log(err); document.getElementById('loader').style.display=\
  'flex';window.setInterval(()=>{location.reload();}, 1000);}window.onload=()=>{getPinState.then\
  (pinState=>{addDevices(pinState); document.getElementById('loader').style.display='none';})\
  .catch(err=>{error();}); document.addEventListener('click',function(e){if(e.target && e.target.\
  classList.contains('device')){changePinState(e.target);}});}</script></html>";
  server.send(200, "text/html", page);
  Serial.println("Response Sent");
}

/*
 * Handels HTTP GET request to /status e.g, http://esp8266.local/status
 * Send back JSON response with the current status of the relay modules
 */
void handleGetStatusRequest(){
  Serial.print("\nRecieved /status GET request... ");
  // update the status of the PINS
  readPINstatus();
  // build an JSON response
  String resJSON = buildJSON();
  server.send(200, "application/json", resJSON);
  Serial.println("Response Sent");
}

/*
 * Handels HTTP POST request to /status
 * Changes the state of GPIO pin.
 * The arguments of the post request are the pin number to change
 * the state of and the state to change the pin to.
 */
void handleChangePinStateRequest(){
  Serial.print("\nRecieved /status change POST request... ");
  // read the arguments
  int pinNo = server.arg("pinNumber").toInt();
  int pinState = server.arg("state").toInt();
  if (changePinState (pinNo, pinState) > 0){
    server.send(200, "text/html", "Success"); 
  } else {
    server.send(400, "text/html", "Error Switching PIN State"); 
  }
}

void handleNotFound(){
  Serial.println("Recieved GET request to undefined path... ");
  // the html webpage for when path not found
  String errorPageHTML="<!DOCTYPE html><html lang='en'><head> <meta charset='UTF-8'>\
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
  <meta http-equiv='X-UA-Compatible' content='ie=edge'> <title>ESP8266 IOT ERROR 404</title>\
  <style>html, body, #fullheight{min-height: 100% !important; height: 100%;\
  padding: 0; margin: 0;}body{background: #000428; background: -webkit-linear-gradient\
  (to right, #004e92, #000428); background: linear-gradient(to right, #004e92, #000428);\
  color: white; display: flex; justify-content: center; align-items: center; flex-direction:\
  column; font-family: Helvetica, Arial, sans-serif;}button{background-color: #4CAF50;\
  border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none;\
  display: inline-block; font-size: 16px; cursor: pointer;}</style></head><body>\
  <h1 style='font-size:3.5em;'>4 0 4</h1> <h2 style='font-weight: 100; margin: 10px;'>\
  The requested page cound not be found.</h2> <br><a href='/'><button>Go Home</button></a></body></html>";
  // send response
  server.send(404, "text/html", errorPageHTML);
  Serial.println("Response Sent");
}

void setup() {

  // begin serial output for debug
  Serial.begin(115200);

  // set required pins to output
  pinMode(LED_BUILTIN, OUTPUT);
  for (int x=0;x<RELAY_COUNT;x++) pinMode(relayPINS[x], OUTPUT);

  // update the default output of pins based on default values of relayPINSstatus
  updatePINoputput();

  // set WiFi mode
  WiFi.mode(WIFI_STA);

  // connect to WiFi
  connectWiFi();

  // mDNS, will make webpage available locally at http://esp8266.local
  if (MDNS.begin("esp8266")) Serial.println("MDNS responder started");
  
  // create routes for handeling HTTP requests
  server.on("/", handleRoot);
  server.on("/status", HTTP_GET, handleGetStatusRequest);
  server.on("/status", HTTP_POST, handleChangePinStateRequest);
  server.onNotFound(handleNotFound);
  
  // start the HTTP server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  
  // if WiFi is disconnected, try to reconnect
  if (WiFi.status() != WL_CONNECTED)  connectWiFi();

  server.handleClient();
  MDNS.update();
}
