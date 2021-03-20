#ifndef TRACKER_TYPES_H
#define TRACKER_TYPES_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct {
  String name;
  String imei;
  String modemInfo;
  int csq;
  gpsInfo gps;
} espTracker;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // TRACKER_TYPES_H