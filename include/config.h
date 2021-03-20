#ifndef PINCONFIG_H
#define PINCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define RXD2 26
#define TXD2 27
#define BATT_ADC 35
#define LED 12
#define MODEM_PWR 4
#define GSM_PIN ""
#define GPS_MSG_INTV_SEC 15

#define ADC_VREF 1100

#define led_on() \
        pinMode(LED, OUTPUT); \
        digitalWrite(LED, LOW);

#define led_off() \
        pinMode(LED, OUTPUT); \
        digitalWrite(LED, HIGH);

#define modem_pwr_on() \
        pinMode(MODEM_PWR, OUTPUT); \
        digitalWrite(MODEM_PWR, HIGH);

#define modem_pwr_off() \
        pinMode(MODEM_PWR, OUTPUT); \
        digitalWrite(MODEM_PWR, LOW);

#define modem_gps_on() \
        modem.sendAT("+SGPIO=0,4,1,1"); \
        modem.waitResponse(); \
        modem.enableGPS();

#define modem_gps_off() \
        modem.disableGPS(); \
        modem.sendAT("+SGPIO=0,4,1,0"); \
        modem.waitResponse();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // PINCONFIG_H