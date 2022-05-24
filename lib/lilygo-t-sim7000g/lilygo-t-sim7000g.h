#ifndef LILYGO_T_SIM7000G
#define LILYGO_T_SIM7000G

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#include <TinyGsmClient.h>

class Tracker {

#define MODEM_DTR_PIN   25
#define MODEM_RX_PIN    26
#define MODEM_TX_PIN    27
#define MODEM_POWER_PIN 4
#define MODEM_GPS_PIN   4 // GPIO4 (SIM7000 internal GPIO)
#define MODEM_BAUD_RATE 115200

#define BAT_ADC_PIN     35
#define SOLAR_ADC_PIN   35
#define ADC_VREF        1100

#define SD_MISO_PIN     2
#define SD_MOSI_PIN     15
#define SD_CLK_PIN      14
#define SD_CS_PIN       13

#define LED_PIN         12

#define ACTIVATE_GPS_ANT_POWER "+SGPIO=0,4,1,1"
#define DEACTIVATE_GPS_ANT_POWER "+SGPIO=0,4,1,0"

typedef struct {
    float     lat;
    float     lon;
    uint16_t  speed;
    uint16_t  alt;
    int       vsat;
    int       usat;
    uint8_t   acc;
    int       year;
    int       month;
    int       day;
    int       hour;
    int       min;
    int       sec;
} gpsInfo;

private:
    HardwareSerial serialAt = HardwareSerial(1);

    String modemName;
    String imei;
    String modemInfo;
    int csq;
    gpsInfo gps;
    String gpsRaw;
    String message;

public:
    Tracker();
    TinyGsm modem = TinyGsm(serialAt);
    TinyGsmClient client = TinyGsmClient(modem);

    bool cell_connect();
    void gprs_connect();
    void gps_enable();
    void gps_disable();
    bool is_data_connected();
    void get_gps_data();
    void send_gps_raw();
    uint16_t get_battery_mv();
    void led_on();
    void led_off();
    void modem_pwr_on();
    void modem_pwr_off();

    // https://cdn.geekfactory.mx/sim7000g/SIM7000%20Series_AT%20Command%20Manual_V1.06.pdf
    // additional AT commands
    String get_modem_firmware_version();
};

#endif // LILYGO_T_SIM7000G