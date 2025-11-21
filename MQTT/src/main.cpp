//-----------------------------
// Title: MQTT
//-----------------------------
// Program Details:
//-----------------------------
// Purpose: Conenct to broker.mqtt-dashboard.com, Publish and subscribe
// Dependencies: Make sure you have installed PubSubClient.h
// Date: 11/20/2025 8:43 PM PT
// Compiler: PIO Version 1.72.0
// Atuhor: Originally an example called ESP8266-mqtt / slightly modified and cleaned up by Farid Farahmand
// OUTPUT: publishes 1,2,3,.... on outTopic every publishTimeInterval
// INPUT: Received value from the broker on inTopic  
// SETUP: To see the published values go to http://www.hivemq.com/demos/websocket-client/ 
//        subscribe to inTopic and outTopic. You can also create an APP using MQTT Dash
// Versions: 
//  v1: Nov-24-2022 - Cleaned up version 
//  v2: Nov-20-2025 - Zella's version
//-----------------------------

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

// ---------- WiFi variables ----------
const char* ssid     = "BigPapa";      // Enter your WiFi name
const char* password = "wificonnect";  // Enter WiFi password

// ---------- MQTT variables ----------
const char* mqtt_server = "broker.mqtt-dashboard.com";

// Topic to RECEIVE LED control (App → ESP)
const char* subscribeTopic = "testtopic/temp/inTopic/zellas_mqtt";

// Topic to SEND switch events (ESP → App)
const char* switchTopic    = "testtopic/temp/outTopic/zellas_mqtt";

// Topic to SEND potentiometer values (ESP → App)
const char* potTopic       = "testtopic/temp/outTopic/zellas_mqtt/pot";

#define publishTimeInterval 2000  // ms between pot publishes

// ---------- Pins ----------
#define BTN    D1
#define ledPin D2
#define potPin A0   // potentiometer wiper

// ADC & Voltage Divider Constants
//----------------------------------
#define VIN 3.3 // 3.3 V Power Voltage

// ---------- Globals ----------
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int  ledStatus = 0;

WiFiClient espClient;
PubSubClient client(espClient);

// --------- Button / timer state ----------
volatile bool g_btnEvent = false;       // Set by ISR when button pressed
bool switchPendingZero   = false;       // If true, we still need to send "0"
unsigned long switchOneSentTime = 0;    // When we sent "1"

// -----------------------------------------------------
// Interrupt Service Routine for button
// -----------------------------------------------------
IRAM_ATTR void onButtonISR() {
  g_btnEvent = true;  // just set a flag, keep ISR short
}

// -----------------------------------------------------
// Initialize Button (with pull-up)
// -----------------------------------------------------
void buttonInit() {
  // Button wired from D1 to GND
  pinMode(BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN), onButtonISR, FALLING);
}

void potentiometer() {
  unsigned long now = millis();
  if (now - lastMsg > publishTimeInterval) {
    lastMsg = now;

    int potVal = analogRead(potPin);  // 0–1023

    // Convert from analog to voltage (LDR Analog Output * Resolution)
    float Vout = float(potVal) * (VIN / float(1023));

    // Publish Vout as text
    snprintf(msg, MSG_BUFFER_SIZE, "%.3f", Vout);

    Serial.print("Publishing pot value: ");
    Serial.print(msg);
    Serial.print(" to ");
    Serial.println(potTopic);

    client.publish(potTopic, msg);
  }
}

// -----------------------------------------------------
// WiFi setup
// -----------------------------------------------------
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// -----------------------------------------------------
// MQTT callback - called when a subscribed message arrives
// -----------------------------------------------------
void callback(char* topic, byte* payload, int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Print the full payload for debugging
  String incoming = "";
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    Serial.print(c);
    incoming += c;
  }
  Serial.println();

  // Only handle messages for our subscribeTopic
  if (strcmp(topic, subscribeTopic) == 0 && length > 0) {
    char first = payload[0];

    // We expect '1' or '0'
    // ASSUMPTION: external LED on D2 -> HIGH = ON, LOW = OFF
    if (first == '1') {
      digitalWrite(ledPin, HIGH);  // LED ON
      ledStatus = 1;
      Serial.println("LED turned ON (from MQTT)");
    } else if (first == '0') {
      digitalWrite(ledPin, LOW);   // LED OFF
      ledStatus = 0;
      Serial.println("LED turned OFF (from MQTT)");
    }
  }
}

// -----------------------------------------------------
// Reconnect to MQTT broker if needed
// -----------------------------------------------------
void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // resubscribe
      client.subscribe(subscribeTopic);
      Serial.print("Subscribed to: ");
      Serial.println(subscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// -----------------------------------------------------
// Handle button press + publish 1 then 0 after 5s
// -----------------------------------------------------
void buttonPress() {
  static uint32_t lastBtn = 0;
  uint32_t current = millis();

  // If the button has been pressed (ISR set flag)
  if (g_btnEvent) {
    g_btnEvent = false; // Reset Interrupt flag
    if (current - lastBtn > 50) { // debounce
      lastBtn = current;

      Serial.println("Switch press detected");

      // Immediately send "1" on switchTopic
      client.publish(switchTopic, "1");
      Serial.print("Published 1 to ");
      Serial.println(switchTopic);

      // Start 5-second timer to send "0"
      switchPendingZero  = true;
      switchOneSentTime  = millis();
    }
  }

  // If we previously sent 1, send 0 five seconds later
  if (switchPendingZero && (millis() - switchOneSentTime >= 5000)) {
    client.publish(switchTopic, "0");
    Serial.print("Published 0 to ");
    Serial.println(switchTopic);
    switchPendingZero = false;
  }
}

// -----------------------------------------------------
// Setup
// -----------------------------------------------------
void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // LED OFF at start (for external LED: LOW = off, HIGH = on)
  pinMode(potPin, INPUT);

  Serial.begin(9600);
  setup_wifi();

  buttonInit();  // <<< IMPORTANT: enable button + interrupt

  client.setServer(mqtt_server, 1883);  // plain TCP
  client.setCallback(callback);
}

// -----------------------------------------------------
// Loop
// -----------------------------------------------------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // handles MQTT traffic & triggers callback()

  // 1) Handle switch behaviour (press → publish 1, then 0 after 5s)
  buttonPress();

  // 2) Periodically publish potentiometer value
  potentiometer();
}