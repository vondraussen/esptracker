#include <Arduino.h>
#include <BluetoothSerial.h>
// #include <WiFi.h>
// #include <WiFiClientSecure.h>
#include "SPIFFS.h"
#include "config.h"
#include "trackerTypes.h"

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_DEBUG Serial
#include <TinyGsmClient.h>

// Your GPRS credentials, if any
const char gprsUser[] = "";
const char gprsPass[] = "";

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = WIFI_SSID;
const char wifiPass[] = WIFI_PASS;

BluetoothSerial SerialBT;

void cell_init(espTracker* tracker, TinyGsm modem) {
  tracker->name = modem.getModemName();
  tracker->imei = modem.getIMEI();
  tracker->modemInfo = modem.getModemInfo();

  DBG("Modem Name:", tracker->name);
  DBG("Modem Info:", tracker->modemInfo);

  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) {
    modem.simUnlock(GSM_PIN);
  }

  DBG("Waiting for network...");
  if (!modem.waitForNetwork()) {
    DBG("Failed to connect network, will try again");
    delay(10000);
    return;
  }

  if (modem.isNetworkConnected()) {
    DBG("Network connected");
  }

  DBG("Connecting to", APN);
  if (!modem.gprsConnect(APN, gprsUser, gprsPass)) {
    delay(10000);
    return;
  }

  bool res = modem.isGprsConnected();
  tracker->csq = modem.getSignalQuality();

  DBG("GPRS status:", res ? "connected" : "not connected");
  DBG("IMEI:", tracker->imei);
  DBG("Signal quality:", tracker->csq);
}

// for bluetooth serial
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t* param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    Serial.println("Client Connected");
  }
}

// TODO no float needed here! convert to fixed point
float readBattery(uint8_t pin) {
  return ((float)analogRead(pin) / 4095.0) * 2.0 * 3.3 * (ADC_VREF);
}

void setup() {
  modem_pwr_on();
  Serial.begin(230400);

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }

  SerialBT.register_callback(callback);
  if (!SerialBT.begin("ESP32")) {
    Serial.println("An error occurred initializing Bluetooth");
  } else {
    Serial.println("Bluetooth initialized");
  }

  Serial.println("Waiting for connections...");
  delay(6000);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println("going to loop");
}

enum StateMaschineState {
  INIT = 0,
  AGPS_SETUP,
  GPS_INIT,
  GPS_ENABLED,
  GPRS_CONNECT,
  GPRS_CONNECTED,
};

// typedef struct {
//   espTracker* tracker;
//   cppQueue* gpsQueue;
//   TinyGsmClient* cell;
// } context_t;

void loop() {
  static uint8_t state = INIT;
  char buf[200];
  unsigned long timeout = 0;
  espTracker tracker;
  TinyGsm modem(Serial2);
  TinyGsmClient client(modem, 0);

  // context_t ctx;
  // ctx.tracker = &tracker;
  // ctx.cell = &client;

  DBG("Initializing modem...");
  if (!modem.init()) {
    DBG("Failed to restart modem, delaying 10s and retrying");
    delay(10000);
    return;
  }

  while (1) {
    switch (state) {
      case INIT:
        cell_init(&tracker, modem);
        modem_gps_on();
        state = GPS_INIT;
        break;

      case GPS_INIT:
        DBG("GPS_INIT");
        modem.sendAT("+CGNSHOT");
        modem.waitResponse();
        state = GPS_ENABLED;
        break;

      case GPS_ENABLED:
        DBG("GPS_ENABLED");
        led_off();
        state = GPRS_CONNECT;
        break;

      case GPRS_CONNECT:
        DBG("GPRS_CONNECT");
        DBG("Connecting to ", SERVER, SERVER_PORT);
        if (!client.connect(SERVER, SERVER_PORT)) {
          DBG("... failed");
          // delay and retry
          // break;
        }
        timeout = millis();
        state = GPRS_CONNECTED;
        break;

      case GPRS_CONNECTED:
        if(!client.connected()) {
            DBG("connection lost");
            // we need a delay here for the reconnect one which adds up till 30sec
            state = GPRS_CONNECT;
            break;
        }
        if (millis() > timeout) {
          float fspeed, falt, facc = 0;
          bool gotPos =
          modem.getGPS(&tracker.gps.lat, &tracker.gps.lon, &fspeed, &falt,
                       &tracker.gps.vsat, &tracker.gps.usat, &facc);

          if (!gotPos) {
            DBG("no fix");
            break;
          }

          tracker.gps.speed = (uint16_t)fspeed;
          tracker.gps.alt = (uint16_t)falt;
          tracker.gps.acc = (uint8_t)(facc * 10);

          String gpsRaw = modem.getGPSraw();
          sprintf(buf, "%.7f,%.7f acc:%d bat:%f - %s", tracker.gps.lat,
                  tracker.gps.lon, tracker.gps.acc, readBattery(BATT_ADC),
                  gpsRaw.c_str());
          SerialBT.println(buf);
          DBG(buf);

          if(!client.connected()) {
            DBG("connection lost");
            // we need a delay here for the reconnect one which adds up till 30sec
            state = GPRS_CONNECT;
            break;
          }

          // send to server
          client.print(tracker.imei +','+ gpsRaw + "\r\n");

          timeout = millis() + (GPS_MSG_INTV_SEC * 1000);
        }
        break;

      default:
        break;
    }
  }
}
