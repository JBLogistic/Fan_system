/*============================================================================*/
/*                                 CAPGEMINI                                  */
/*============================================================================*/
/*                        OBJECT SPECIFICATION                                */
/*============================================================================*/
/*!
 * $Source: fanControl.h $
 * $Revision: 1 $
 * $Author: Bako Jozsef $
 * $Date: 2023.09.25 $
 */
/*============================================================================*/
/* DESCRIPTION :                                                              */
/** This is the main file of the project where 
*/
/* Includes */
/*============================================================================*/
#include "mqtt/mqtt.h"
#include "temperature/ds18b20.h"
#include "fanControl/fanControl.h"
/* Constants and types  */
/*============================================================================*/

const uint16_t interval = 60000;        // Interval at which to publish sensor readings

/* Variables */
/*============================================================================*/
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;


unsigned long previousMillis = 0;   // Stores last time temperature was published


/* Exported functions */
/*============================================================================*/
void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.setCredentials("mqtt-user","1234");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  //Serial.printf("[WiFi-event] event: %d\n", event);
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      //Serial.println(WiFi.localIP());
      connectToMqtt();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println(topic);
  Serial.println(payload);
  if(strcmp(topic , MQTT_PUB_SPEED) == 0){
    
    if(strncmp(payload,"0110",4)==0){
      changeFanSpeed(10,1);
    }
    if(strncmp(payload,"0120",4)==0){
      changeFanSpeed(30,1);
    }
    if(strncmp(payload,"0130",4)==0){
      changeFanSpeed(50,1);
    }
    if(strncmp(payload,"0140",4)==0){
      changeFanSpeed(80,1);
    }
    if(strncmp(payload,"0150",4)==0){
      changeFanSpeed(100,1);
    }
    if(strncmp(payload,"0160",4)==0){
      changeFanSpeed(130,1);
    }
    if(strncmp(payload,"0170",4)==0){
      changeFanSpeed(180,1);
    }
    if(strncmp(payload,"0180",4)==0){
      changeFanSpeed(200,1);
    }
    if(strncmp(payload,"0190",4)==0){
      changeFanSpeed(230,1);
    }
    if(strncmp(payload,"01100",5)==0){
      changeFanSpeed(255,1);
    }
    if(strncmp(payload,"010",3)==0){
      changeFanSpeed(0,1);
    }
    if(strncmp(payload,"1010",4)==0){
      changeFanSpeed(10,2);
    }
    if(strncmp(payload,"1020",4)==0){
      changeFanSpeed(30,2);
    }
    if(strncmp(payload,"1030",4)==0){
      changeFanSpeed(50,2);
    }
    if(strncmp(payload,"1040",4)==0){
      changeFanSpeed(80,2);
    }
    if(strncmp(payload,"1050",4)==0){
      changeFanSpeed(100,2);
    }
    if(strncmp(payload,"1060",4)==0){
      changeFanSpeed(130,2);
    }
    if(strncmp(payload,"1070",4)==0){
      changeFanSpeed(180,2);
    }
    if(strncmp(payload,"1080",4)==0){
      changeFanSpeed(200,2);
    }
    if(strncmp(payload,"1090",4)==0){
      changeFanSpeed(230,2);
    }
    if(strncmp(payload,"10100",4)==0){
      changeFanSpeed(255,2);
    }
    if(strncmp(payload,"100",3)==0){
      changeFanSpeed(0,2);
    }
  }
}
void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup() {
  // Start the DS18B20 sensor
  sensors.begin();
  
  Serial.begin(9600);
  Serial.println();
  Serial.println();

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  //mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  //mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");
  connectToWifi();
  //Init the Fans
  initPWMfans();
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Every X number of seconds (interval = 10 seconds) 
  // it publishes a new MQTT message
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    updateRPM();
    // New temperature readings
    sensors.requestTemperatures(); 
    // Temperature in Celsius degrees
    temp = sensors.getTempCByIndex(0);
    // Temperature in Fahrenheit degrees
    //temp = sensors.getTempFByIndex(0);
    // Publish an MQTT message on topic esp32/ds18b20/temperature
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP, 1, false, String(temp).c_str());                            
    /*Serial.printf("Publishing on topic %s at QoS 1, packetId: ", MQTT_PUB_TEMP);
    Serial.println(packetIdPub1);
    Serial.printf("Message: %.2f /n", sensors.getTempCByIndex(0));*/
    // Publish an MQTT message on topic fan/speed/out and fan/speed/in
    packetIdPub1 = mqttClient.publish(MQTT_PUB_FAN_OUT, 1, false, String(getRotation1()).c_str());    
    packetIdPub1 = mqttClient.publish(MQTT_PUB_FAN_IN, 1, false, String(getRotation2()).c_str());   
    //Publish an MQTT message so that the HA knows that the fans are online
    packetIdPub1 = mqttClient.publish(MQTT_PUB_ON, 1, false, String("ON").c_str());
    //Serial.printf("Message: %.2f \n", getRotation1());
    //Subscribing to the Fan/speed/set topic where the updated pwm values are published
    mqttClient.subscribe(MQTT_PUB_SPEED,1);
  }
}