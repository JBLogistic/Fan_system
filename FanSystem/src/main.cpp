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
#include <TaskScheduler.h>
/* Constants and types  */
/*============================================================================*/
#define PERIOD1 5000
#define DURATION 100000
#define PERIOD2 10000
const uint16_t interval = 60000;        // Interval at which to publish sensor readings

/* Variables */
/*============================================================================*/
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

Scheduler ts;

unsigned long previousMillis = 0;   // Stores last time temperature was published

/* Exported functions */
/*============================================================================*/

void task1();
Task Task1( PERIOD1 * TASK_MILLISECOND, DURATION / PERIOD1, &task1, &ts, true );

void task2();
Task Task2( PERIOD2 * TASK_MILLISECOND, DURATION / PERIOD2, &task2, &ts, true );
// Function to connect to Wi-Fi using predefined credentials.
void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}
// Function to connect to MQTT broker with specific credentials.
void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.setCredentials("mqtt-user","1234");
  mqttClient.connect();
}
// Callback function for Wi-Fi events, such as connection and disconnection.
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
// Callback function when MQTT client successfully connects to the broker.
void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}
// Callback function when MQTT client disconnects from the broker.
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}
// Callback function when MQTT client receives a message.
// The message is being checked if it contains a fan control functionality
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println(topic);
  Serial.println(payload);
  // Here the topic is checked if it matches the fan controlling topic
  if(strcmp(topic , MQTT_PUB_SPEED) == 0){
    // Here the message is checked if the first 3 or 4 characters are as follows
    // ther first two indicate the desired fan, the second 2 or 3 are the speed percentage
    if(strncmp(payload,"0110",4)==0){
      changeFanSpeed(10,fan1);
    }
    if(strncmp(payload,"0120",4)==0){
      changeFanSpeed(30,fan1);
    }
    if(strncmp(payload,"0130",4)==0){
      changeFanSpeed(50,fan1);
    }
    if(strncmp(payload,"0140",4)==0){
      changeFanSpeed(80,fan1);
    }
    if(strncmp(payload,"0150",4)==0){
      changeFanSpeed(100,fan1);
    }
    if(strncmp(payload,"0160",4)==0){
      changeFanSpeed(130,fan1);
    }
    if(strncmp(payload,"0170",4)==0){
      changeFanSpeed(180,fan1);
    }
    if(strncmp(payload,"0180",4)==0){
      changeFanSpeed(200,fan1);
    }
    if(strncmp(payload,"0190",4)==0){
      changeFanSpeed(230,fan1);
    }
    if(strncmp(payload,"01100",5)==0){
      changeFanSpeed(255,fan1);
    }
    if(strncmp(payload,"010",3)==0){
      changeFanSpeed(0,fan1);
    }
    if(strncmp(payload,"1010",4)==0){
      changeFanSpeed(10,fan2);
    }
    if(strncmp(payload,"1020",4)==0){
      changeFanSpeed(30,fan2);
    }
    if(strncmp(payload,"1030",4)==0){
      changeFanSpeed(50,fan2);
    }
    if(strncmp(payload,"1040",4)==0){
      changeFanSpeed(80,fan2);
    }
    if(strncmp(payload,"1050",4)==0){
      changeFanSpeed(100,fan2);
    }
    if(strncmp(payload,"1060",4)==0){
      changeFanSpeed(130,fan2);
    }
    if(strncmp(payload,"1070",4)==0){
      changeFanSpeed(180,fan2);
    }
    if(strncmp(payload,"1080",4)==0){
      changeFanSpeed(200,fan2);
    }
    if(strncmp(payload,"1090",4)==0){
      changeFanSpeed(230,fan2);
    }
    if(strncmp(payload,"10100",4)==0){
      changeFanSpeed(255,fan2);
    }
    if(strncmp(payload,"100",3)==0){
      changeFanSpeed(0,fan2);
    }
  }
}
// Callback function when MQTT client unsubscribes from a topic.
void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

// Callback function when MQTT client publishes a message.
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
  //mqttClient.subscribe(MQTT_PUB_SPEED,1);
  //Init the Fans
  initPWMfans();
  //ts.init();
  ts.addTask(Task1);
  ts.addTask(Task2);
  ts.enableAll();
}

void loop() {
  mqttClient.subscribe(MQTT_PUB_SPEED,1);
  ts.execute();
}
void task1(){
  updateRPM();
  Serial.println("asd1");
}
void task2(){
  Serial.println("asd2");
  sensors.requestTemperatures(); 
    // Temperature in Celsius degrees
  temp = sensors.getTempCByIndex(0);
  uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP, 1, false, String(temp).c_str());                            
    /*Serial.printf("Publishing on topic %s at QoS 1, packetId: ", MQTT_PUB_TEMP);
    Serial.println(packetIdPub1);
    Serial.printf("Message: %.2f /n", sensors.getTempCByIndex(0));*/
    // Publish an MQTT message on topic fan/speed/out and fan/speed/in
    packetIdPub1 = mqttClient.publish(MQTT_PUB_FAN_OUT, 1, false, String(getRotation1()).c_str());    
    packetIdPub1 = mqttClient.publish(MQTT_PUB_FAN_IN, 1, false, String(getRotation2()).c_str());   
    //Publish an MQTT message so that the HA knows that the fans are online
    packetIdPub1 = mqttClient.publish(MQTT_PUB_ON, 1, false, String("ON").c_str());
}