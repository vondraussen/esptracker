#include <Arduino.h>
#include <unity.h>
#include <lilygo-t-sim7000g.h>

Tracker tracker;

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_modem_info(void) {
  String modemNameExp = "SIM7000G R1529";
  TEST_ASSERT_EQUAL_STRING(modemNameExp.c_str(), tracker.modem.getModemInfo().c_str());
}

void test_modem_name(void) {
  String modemNameExp = "SIMCOM SIM7000G";
  TEST_ASSERT_EQUAL_STRING(modemNameExp.c_str(), tracker.modem.getModemName().c_str());
}

void test_modem_imei(void) {
  TEST_ASSERT_EQUAL_STRING(MODEM_IMEI, tracker.modem.getIMEI().c_str());
}

void test_modem_is_not_registered(void) {
  TEST_ASSERT_EQUAL_INT8(REG_UNREGISTERED, tracker.modem.getRegistrationStatus());
}

void test_modem_network_mode(void) {
  tracker.modem.setNetworkMode(2);
  TEST_ASSERT_EQUAL_INT16(2, tracker.modem.getNetworkMode());
}

void test_modem_preferred_network_modes(void) {
  TEST_ASSERT_EQUAL_STRING(" ((1-Cat-M),(2-NB-IoT),(3-Cat-M And NB-IoT))\r", tracker.modem.getPreferredModes().c_str());
  TEST_ASSERT_EQUAL_INT16(3, tracker.modem.getPreferredMode());
}

void test_gps_raw(void) {
  String modemGpsRaw = "1,0,,,,,,,0,,,,,,,,,,,,";
  tracker.gps_enable();

  TEST_ASSERT_EQUAL_STRING(modemGpsRaw.c_str(), tracker.modem.getGPSraw().c_str());
}

void test_at_command(void) {
  String modemFirmwareVersion = "1529B04SIM7000G";
  TEST_ASSERT_EQUAL_STRING(modemFirmwareVersion.c_str(), tracker.get_modem_firmware_version().c_str());
}

void test_gps(void) {
  float lat = 0.0, lon = 0.0;
  tracker.modem.getGPS(&lat, &lon);

  TEST_ASSERT_EQUAL_FLOAT(0.0, lat);
  TEST_ASSERT_EQUAL_FLOAT(0.0, lon);
}

void test_cell_connect(void) {
  TEST_ASSERT_TRUE(tracker.cell_connect());
  TEST_ASSERT_TRUE(tracker.modem.isNetworkConnected());
  TEST_ASSERT_INT16_WITHIN(20, 30, tracker.modem.getSignalQuality());
}

void test_battery_voltage(void) {
  // TODO
  TEST_ASSERT_EQUAL_UINT16(0, tracker.get_battery_mv());
}

void test_tcp_send(void) {
  // start a netcat instance listening for this test
  // nc -v -l 0.0.0.0 59999
  TEST_ASSERT_TRUE(tracker.modem.isGprsConnected());
  TEST_ASSERT_TRUE(tracker.client.connect(SERVER, SERVER_PORT));
  tracker.client.write("869951999999999,1,1,20210320104915.000,48.000001,9.000001,276.200,0.00,0.0,1,,1.7,1.9,0.9,,21,4,,,43,,");
}

void setup() {
  delay(2000);  // service delay
  UNITY_BEGIN();

  RUN_TEST(test_modem_info);
  RUN_TEST(test_modem_name);
  RUN_TEST(test_modem_imei);
  // RUN_TEST(test_modem_is_not_registered);
  RUN_TEST(test_modem_preferred_network_modes);
  RUN_TEST(test_modem_network_mode);
  RUN_TEST(test_gps_raw);
  RUN_TEST(test_gps);
  RUN_TEST(test_at_command);
  RUN_TEST(test_cell_connect);
  // RUN_TEST(test_battery_voltage);
  RUN_TEST(test_tcp_send);

  UNITY_END();  // stop unit testing
  tracker.client.stop();
}

void loop() {}