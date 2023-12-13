/*
  **Automated Window Blinds**
  
  Smart window blind controlled by Alexa
  Uses ESP8266 Controller (ESP-12F NodeMcu D1), Stepper motor 28BYJ-48
  IOT Library (Sinric Pro): Control device via cloud
  Over The Air Update (ArduinoOTA): Update controller firmware over local Wi-Fi
*/

// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
  #define DEBUG_ESP_PORT Serial
  #define NODEBUG_WEBSOCKETS
  #define NDEBUG
#endif 

#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
#endif

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "SinricPro.h"
#include "SinricProBlinds.h"

#include "credentials.h" // WIFI_SSID, WIFI_PASS, APP_KEY, APP_SECRET

// uncomment to enable RIGHT
// #define LEFT_BLIND

#ifdef LEFT_BLIND
  #define BLINDS_ID         "64a30040743f9120704647fd"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
  #define PIN_MOTOR_A D1
  #define PIN_MOTOR_B D3
  #define PIN_MOTOR_C D2
  #define PIN_MOTOR_D D4
#else  // RIGHT_BLIND
  #define BLINDS_ID         "64c18a9a2ac6a1822a925a30"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
  #define PIN_MOTOR_A D4
  #define PIN_MOTOR_B D2
  #define PIN_MOTOR_C D3
  #define PIN_MOTOR_D D1
#endif


#define BAUD_RATE         9600                // Change baudrate to your need

#include <Stepper.h>


const int stepsPerRevolution = 2038;  // change this to fit the number of steps per revolution

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, PIN_MOTOR_A, PIN_MOTOR_B, PIN_MOTOR_C, PIN_MOTOR_D);

// toggle to high impedance (INPUT mode) to stop current draw
//  Did not work. Actually increased back EMF
void toggleMotor(bool on) 
{
  int mode = on ? OUTPUT : INPUT;
  pinMode(PIN_MOTOR_A, mode);
  pinMode(PIN_MOTOR_B, mode);
  pinMode(PIN_MOTOR_C, mode);
  pinMode(PIN_MOTOR_D, mode);
}

int blindsPosition = 0,
    blindsTarget   = 0;
bool powerState = false;
const int stepsPerResolution = 80;  // change this to fit the number of steps per revolution

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("\tDevice %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
  powerState = state;
  
  return true; // request handled properly
}

bool onRangeValue(const String &deviceId, int &position) {
  Serial.printf("\tDevice %s set position to %d\r\n", deviceId.c_str(), position);
  blindsTarget = position;

  toggleMotor(true);

  return true; // request handled properly
}

bool onAdjustRangeValue(const String &deviceId, int &positionDelta) {
  blindsTarget += positionDelta;
  Serial.printf("Device %s position changed about %i to %d\r\n", deviceId.c_str(), positionDelta, blindsTarget);
  positionDelta = blindsTarget; // calculate and return absolute position
  toggleMotor(true);
  return true; // request handled properly
}


// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();

  IPAddress localIP = WiFi.localIP();
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

void setupSinricPro() {
  // get a new Blinds device from SinricPro
  SinricProBlinds &myBlinds = SinricPro[BLINDS_ID];
  myBlinds.onPowerState(onPowerState);
  myBlinds.onRangeValue(onRangeValue);
  myBlinds.onAdjustRangeValue(onAdjustRangeValue);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setupMotor() {
  toggleMotor(true);

  // set the speed at 60 rpm:
  myStepper.setSpeed(5);
  // ok: 15 rpm 
}

void motorOff() {
  digitalWrite(PIN_MOTOR_A, LOW);
  digitalWrite(PIN_MOTOR_B, LOW);
  digitalWrite(PIN_MOTOR_C, LOW);
  digitalWrite(PIN_MOTOR_D, LOW);

  // toggleMotor(false);
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
  setupMotor();
}

void loop() {
  ArduinoOTA.handle();
  SinricPro.handle();

  if (blindsTarget != blindsPosition) {
    int dir = blindsTarget > blindsPosition ? 1 : -1;
    myStepper.step(stepsPerResolution * dir);
    blindsPosition += dir;
  }
  else {
    motorOff();

    // heartbeat
    digitalWrite(LED_BUILTIN, (millis() % 1000) <= 1000);
  }
}
