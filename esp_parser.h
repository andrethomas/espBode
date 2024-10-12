#ifndef _ESP_PARSER_H_
#define _ESP_PARSER_H_

#include <ESP8266WiFi.h>
#include "esp_config.h"
#ifdef USE_JY8600
#include "esp_fy6800.h"
#endif USE_JY8600
#ifdef USE_JDS8060
#include "esp_jds8060.h"
#endif USE_JDS8060

extern volatile char *gReadBuffer;

void handleWriteMsg(char *msg, uint8_t len);

#endif
