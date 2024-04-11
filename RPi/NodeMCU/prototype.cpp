#include <ESP8266WiFi.h> 
#include "Adafruit_MQTT.h" 
#include "Adafruit_MQTT_Client.h" 

#define WLAN_SSID "SSID"
#define WLAN_PASSWORD "PASSWORD"
#define MQTT_SERVER "STATIC_IP"
#define MQTT_PORT 2000
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

#define LED_PIN 4
#define BUTTON_PIN 5


WiFiClient client; // 8266 WiFiClient
// MQTT server
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);

// Publishing feed
Adafruit_MQTT_Publish pi_led = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME, "/leds/pi");
// Subscribing feed
Adafruit_MQTT_Subscribe esp8266_led = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME, "/leds/esp8266");


void mqtt_connect();

void setup() {
    Serial.begin(115200);
    
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    Serial.println("\nConnecting to ");
    Serial.print(WLAN_SSID);
    Serial.println();
    WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");
    Serial.println("IP Address: ");
    Serial.print(WiFi.localIP());

    // Subscribe to esp8266 feed
    mqtt.subscribe(&esp8266_led);
}

bool prev = 0;

void loop() {

    mqtt_connect();
    Adafruit_MQTT_Subscribe *subscription; 
    while (subscription = mqtt.readSubscription()) {
        if (subscription == &esp8266_led) {
            Serial.print(F("Got: ")); 
            Serial.println(message); 
                // Check if the message was ON, OFF, or TOGGLE. 
            if (strncmp(message, "ON", 2) == 0) { 
                // Turn the LED on. 
                digitalWrite(LED_PIN, HIGH); 
            } 
            else if (strncmp(message, "OFF", 3) == 0) { 
                // Turn the LED off. 
                digitalWrite(LED_PIN, LOW); 
            } 
            else if (strncmp(message, "TOGGLE", 6) == 0) { 
                // Toggle the LED. 
                digitalWrite(LED_PIN, !digitalRead(LED_PIN)); 
            } 
        }
    }


    bool curr = digitalRead(BUTTON_PIN);
    if (curr && !prev) {
        Serial.println("Pi LED Pressed");
        pi_led.publish("TOGGLE");
    }
}


// Function to connect and reconnect as necessary to the MQTT server. 
void MQTT_connect() { 
 int8_t ret; 
 // Stop if already connected. 
 if (mqtt.connected()) { 
   return; 
 } 
 Serial.print("Connecting to MQTT... "); 
 uint8_t retries = 3; 
 while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected 
      Serial.println(mqtt.connectErrorString(ret)); 
      Serial.println("Retrying MQTT connection in 5 seconds..."); 
      mqtt.disconnect(); 
      delay(5000);  // wait 5 seconds 
      retries--; 
      if (retries == 0) { 
        System.out.println("Connection unsuccessful");
        while (1);
      } 
 } 
 Serial.println("MQTT Connected"); 
}