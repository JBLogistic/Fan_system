#include <WiFi.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#define WIFI_SSID "CoPlace"
#define WIFI_PASSWORD "coplace1"

// Raspberry Pi Mosquitto MQTT Broker
#define MQTT_HOST IPAddress(192, 168, 1, 41)
// For a cloud MQTT broker, type the domain name
//#define MQTT_HOST "example.com"
#define MQTT_PORT 1883

// Temperature MQTT Topic
#define MQTT_PUB_TEMP "esp32/ds18b20/temperature"
// Fan MQTT Topic
#define MQTT_PUB_FAN_OUT "Fan/speed/out"
#define MQTT_PUB_FAN_IN "Fan/speed/in"
#define MQTT_PUB_SPEED "Fan/speed/set"
#define MQTT_PUB_ON "Fan/state"

