#include <lilygo-t-sim7000g.h>

Tracker::Tracker() {
  pinMode(BAT_ADC_PIN, INPUT);
  pinMode(SOLAR_ADC_PIN, INPUT);
  DBG("Initializing modem...");
  modem_pwr_on();
  return;
}

void Tracker::gps_enable() {
  modem.sendAT(ACTIVATE_GPS_ANT_POWER);
  modem.waitResponse();
  modem.enableGPS();
  modem.sendAT("+CGNSHOT");
  modem.waitResponse();
  delay(1000);
}

void Tracker::gps_disable() {
    this->modem.disableGPS();
    this->modem.sendAT(DEACTIVATE_GPS_ANT_POWER);
    this->modem.waitResponse();
}

bool Tracker::cell_connect() {
  DBG("Modem Name:", this->modemName);
  DBG("Modem Info:", this->modemInfo);

  DBG("Waiting for network...");
  if (!this->modem.waitForNetwork()) {
    DBG("Failed to connect network, will try again");
    return false;
  }

  if (this->modem.isNetworkConnected()) {
    DBG("Network connected");
  }

  DBG("Connecting to", APN);
  if (!this->modem.gprsConnect(APN, "", "")) {
    return false;
  }

  bool res = this->modem.isGprsConnected();
  this->csq = this->modem.getSignalQuality();

  DBG("GPRS status:", res ? "connected" : "not connected");
  DBG("IMEI:", this->imei);
  DBG("Signal quality:", this->csq);
  return res;
}

void Tracker::gprs_connect() {
    DBG("GPRS_CONNECT");
    DBG("Connecting to ", SERVER, SERVER_PORT);
    if (!this->client.connect(SERVER, SERVER_PORT)) {
        DBG("... failed");
        // delay and retry
        // break;
    }
    // timeout = millis();
}

bool Tracker::is_data_connected() {
    return this->client.connected();
}

void Tracker::get_gps_data() {
    char buf[200];
    float fspeed, falt, facc = 0;

    bool gotPos = this->modem.getGPS(
        &this->gps.lat,
        &this->gps.lon,
        &fspeed,
        &falt,
        &this->gps.vsat,
        &this->gps.usat,
        &facc);

    if (!gotPos) {
        DBG("no fix");
        return;
    }

    this->gps.speed = (uint16_t)fspeed;
    this->gps.alt = (uint16_t)falt;
    this->gps.acc = (uint8_t)(facc * 10);

    this->gpsRaw = this->modem.getGPSraw();
    sprintf(buf, "%.7f,%.7f acc:%d bat:%f - %s", this->gps.lat,
            this->gps.lon, this->gps.acc, this->get_battery_mv(),
            this->gpsRaw.c_str());
    // SerialBT.println(buf);
    message = buf;
    DBG(buf);
}

void Tracker::send_gps_raw() {
  this->client.print(this->imei +','+ this->gpsRaw + "\r\n");
}

uint16_t Tracker::get_battery_mv() {
  // return (analogRead(BAT_ADC_PIN) / 4095) * 2 * 3300 * (ADC_VREF);
  return (4095 / 4095) * 2 * 3300 * (ADC_VREF);
}

String Tracker::get_modem_firmware_version() {
  String res;
  modem.sendAT(GF("+CGMR"));
  modem.waitResponse(5000L, res);
  res.replace(GSM_NL "OK" GSM_NL, "");
  res.replace("Revision:", "");
  res.trim();
  return res;
}

void Tracker::led_on() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void Tracker::led_off() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

void Tracker::modem_pwr_on() {
  pinMode(MODEM_POWER_PIN, OUTPUT);
  digitalWrite(MODEM_POWER_PIN, HIGH);
  delay(10);
  digitalWrite(MODEM_POWER_PIN, LOW);
  delay(1500);
  digitalWrite(MODEM_POWER_PIN, HIGH);
  delay(4600);

  serialAt.begin(MODEM_BAUD_RATE, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

  // be sure, that a SIM is inserted or this will block forever!
  while(!modem.init()) {
    Serial.println("Failed to init modem, retrying...");
  }
}

void Tracker::modem_pwr_off() {
  pinMode(MODEM_POWER_PIN, OUTPUT);
  digitalWrite(MODEM_POWER_PIN, LOW);
  serialAt.end();
}
