#include <ESP8266WiFi.h>
#include "esp_network.h"
#include "esp_config.h"
#ifdef USE_JDS8060
#include "esp_jds8060.h"
#endif USE_JDS8060
#ifdef USE_FY6800
#include "esp_fy6800.h"
#endif USE_FY6800

WiFiServer rpc_server(RPC_PORT);
WiFiServer lxi_server(LXI_PORT);

void setup() {
  Serial.begin(115200);
  // We start by connecting to a WiFi network
  DEBUG("Connecting to ");
  DEBUG(WIFI_SSID);
#if defined(STATIC_IP)
  IPAddress ip(ESP_IP);
  IPAddress mask(ESP_MASK);
  IPAddress gateway(ESP_GW);
  WiFi.config(ip, gateway, mask);
#endif
#if defined(WIFI_MODE_CLIENT)
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSK);
#elif defined(WIFI_MODE_AP)
  WiFi.softAP(WIFI_SSID, WIFI_PSK);
#else
  #error PLEASE SELECT WIFI_MODE_AP OR WIFI_MODE_CLIENT!
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUG(".");
  }
  DEBUG("WiFi connected");
  DEBUG("IP address: ");
  DEBUG(WiFi.localIP());
  rpc_server.begin();
  lxi_server.begin();
}

void loop() {
#ifdef SLEEP_DYNAMIC
  uint32_t loop_start = millis();
#endif SLEEP_DYNAMIC  
   WiFiClient rpc_client;
   WiFiClient lxi_client;
   lxi_client.setTimeout(1000);
   while (!rpc_client) {
    rpc_client = rpc_server.available();
   }
   DEBUG("RPC CONNECTION.");
   handlePacket(rpc_client);
   rpc_client.stop();
   while(!lxi_client) {
     lxi_client = lxi_server.available();
   }

   DEBUG("LXI CONNECTION.");
   while (1) {
    if(0 != handlePacket(lxi_client)) {
      lxi_client.stop();
      DEBUG("RESTARTING");
      return;
    }
   }
#ifdef SLEEP_DYNAMIC
  uint32_t loop_duration = millis() - loop_start;
  if (loop_duration < 20) {
    delay(19-loop_duration);
  } else {
    delay(1);
  }
#endif SLEEP_DYNAMIC    
}

