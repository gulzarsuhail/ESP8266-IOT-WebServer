# ESP8266-IOT-WebServer
An arduino IDE based firmware for nodemcu (ESP8266) to control one to eight relay modules simultaneously through a responsive web interface.

# Setup
To get the webserver running, you need to configure the lines marked  with #CONFIG

Set up your wifi SSID and password in the lines 

```#define STASSID "your_wifi_ssid"
#define STAPSK  "your_wifi_password"```

Set up the number of relays connected to your nodemcu. The minimum number is 1 and the maximum is 8. Set the number in line

```#define RELAY_COUNT 8```

You can set up the device name in the array named deviceName. Even though the array contains eight strings, you can change only the number of strings you defined as RELAY_COUNT.
For example, if four devices are connected, then RELAY_COUNT should be set up as 4 and deviceName array should be configured as

```const String deviceName[8] = {
  "Kitchen Lamp",
  "Toaster",
  "Microwave",
  "Dryer",
  "Device_5",
  "Device_6",
  "Device_7",
  "Device_8"
};```

By default, the above devices must be connected in the following order

| Device number |  GPIO pin number | Board pin number |
| ------- | --- | --- |
| Device1 | 5   | D1 |
| Device2 | 4   | D2 |
| Device3 | 0   | D3 |
| Device4 | 14  | D5 |
| Device5 | 12  | D6 |
| Device6 | 13  | D7 |
| Device7 | 15  | D8 |
| Device8 | 3   | RX |

After the config is complete, upload the code to your nodemcu and connect the wires respective to your relay module and connet mcu to power.

# Usage

You can get to the webinterface via the nodemcu local address http://esp8266.local/status or the local IP address e.g, http://192.168.1.4

# API

The current status of relay connected nodemcu pins an be accessed by sending a GET request to /status and to change the state of a pin, send a POST request to `/status` with `Content-Type` set to `application/x-www-form-urlencoded` and sending two arguments `pinNumber` and  `state` reffering to the pin number whose status you want to change and the state which can be a 1 or 0.
