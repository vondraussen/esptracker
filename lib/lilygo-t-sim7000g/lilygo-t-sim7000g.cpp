#include <lilygo-t-sim7000g.h>

Tracker::Tracker() {
    this->modem = new TinyGsm(Serial2);
    this->client = new TinyGsmClient(*this->modem, 0);

    DBG("Initializing modem...");
    while (!this->modem->init()) { // TODO(vondraussen) add timeout here and do a reset
        DBG("Failed to restart modem, delaying 10s and retrying");
        delay(10000);
    }
    return;
}

Tracker::~Tracker() {
    delete this->client;
    delete this->modem;
    return;
}

void Tracker::gps_enable() {
    this->modem->sendAT(ACTIVATE_GPS_ANT_POWER);
    this->modem->waitResponse();
    this->modem->enableGPS();
    this->modem->sendAT("+CGNSHOT");
    this->modem->waitResponse();
}

void Tracker::gps_disable() {
    this->modem->disableGPS();
    this->modem->sendAT(DEACTIVATE_GPS_ANT_POWER);
    this->modem->waitResponse();
}

void Tracker::cell_connect() {
  this->name = this->modem->getModemName();
  this->imei = this->modem->getIMEI();
  this->modemInfo = this->modem->getModemInfo();

  DBG("Modem Name:", this->name);
  DBG("Modem Info:", this->modemInfo);

  DBG("Waiting for network...");
  if (!this->modem->waitForNetwork()) {
    DBG("Failed to connect network, will try again");
    delay(10000);
    return;
  }

  if (this->modem->isNetworkConnected()) {
    DBG("Network connected");
  }

  DBG("Connecting to", APN);
  if (!this->modem->gprsConnect(APN, "", "")) {
    delay(10000);
    return;
  }

  bool res = this->modem->isGprsConnected();
  this->csq = this->modem->getSignalQuality();

  DBG("GPRS status:", res ? "connected" : "not connected");
  DBG("IMEI:", this->imei);
  DBG("Signal quality:", this->csq);
}

void Tracker::gprs_connect() {
    DBG("GPRS_CONNECT");
    DBG("Connecting to ", SERVER, SERVER_PORT);
    if (!this->client->connect(SERVER, SERVER_PORT)) {
        DBG("... failed");
        // delay and retry
        // break;
    }
    // timeout = millis();
}

bool Tracker::is_data_connected() {
    return this->client->connected();
}

void Tracker::get_gps_data() {
    char buf[200];
    float fspeed, falt, facc = 0;

    bool gotPos = this->modem->getGPS(
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

    this->gpsRaw = this->modem->getGPSraw();
    sprintf(buf, "%.7f,%.7f acc:%d bat:%f - %s", this->gps.lat,
            this->gps.lon, this->gps.acc, this->get_battery_mv(),
            this->gpsRaw.c_str());
    // SerialBT.println(buf);
    DBG(buf);
}

void Tracker::send_gps_raw() {
  this->client->print(this->imei +','+ this->gpsRaw + "\r\n");
}

float Tracker::get_battery_mv() {
  return ((float)analogRead(BAT_ADC_PIN) / 4095.0) * 2.0 * 3.3 * (ADC_VREF);
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
  Serial2.begin(MODEM_BAUD_RATE, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
}

void Tracker::modem_pwr_off() {
  pinMode(MODEM_POWER_PIN, OUTPUT);
  digitalWrite(MODEM_POWER_PIN, LOW);
  Serial2.end();
}
