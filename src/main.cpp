#include <Arduino.h>
#include <TaskScheduler.h>
#include <BluetoothSerial.h>
// #include <WiFi.h>
// #include <WiFiClientSecure.h>
#include "SPIFFS.h"

#include <lilygo-t-sim7000g.h>

#define GPS_MSG_INTV_SEC 15

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = WIFI_SSID;
const char wifiPass[] = WIFI_PASS;

BluetoothSerial SerialBT;

Tracker tracker;

Scheduler ts;
// Task tConfigure     (TASK_IMMEDIATE, TASK_ONCE, &cell_init, &ts, true);

// for bluetooth serial
void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t* param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    Serial.println("Client Connected");
  }
}

// cppcheck-suppress unusedFunction
void setup() {
  tracker.modem_pwr_on();
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
  Serial.println("going to loop");
  // ts.startNow();
}

enum StateMaschineState {
  INIT = 0,
  AGPS_SETUP,
  GPS_INIT,
  GPS_ENABLED,
  GPRS_CONNECT,
  GPRS_CONNECTED,
};

// cppcheck-suppress unusedFunction
void loop() {
  static uint8_t state = INIT;
  unsigned long timeout = 0;

  while (1) {
    switch (state) {
      case INIT:
        tracker.gps_enable();
        tracker.cell_connect();
        state = GPS_ENABLED;
        break;

      case GPS_ENABLED:
        DBG("GPS_ENABLED");
        tracker.led_off();
        state = GPRS_CONNECT;
        break;

      case GPRS_CONNECT:
        tracker.gprs_connect();
        break;

      case GPRS_CONNECTED:
        if(!tracker.is_data_connected()) {
            DBG("connection lost");
            // we need a delay here for the reconnect one which adds up till 30sec
            state = GPRS_CONNECT;
            break;
        }

        if (millis() > timeout) {
          tracker.get_gps_data();

          if(!tracker.is_data_connected()) {
            DBG("connection lost");
            // we need a delay here for the reconnect one which adds up till 30sec
            state = GPRS_CONNECT;
            break;
          }

          tracker.send_gps_raw();
          timeout = millis() + (GPS_MSG_INTV_SEC * 1000);
        }
        break;

      default:
        break;
    }
  }
  // ts.execute();
}
