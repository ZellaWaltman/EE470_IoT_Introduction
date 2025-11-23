//-----------------------------
// Title: MQTT
//-----------------------------
// Program Details:
//-----------------------------
// Purpose: The purpose of this program is to connect to broker.mqtt-dashboard.com,
// and publish and subscribe to three topics: a button input/LED control topic, a buttonpress topic,
// and a topic which contains the voltage readings of the potentiometer. When a user sends a 1 to
// the inTopic, either by sending a 1 directly or pressing a switch on the MQTT app, a LED will turn on.
// When a user presses a switch on the board, a 1 will be sent to the switch topic, with a 0 following
// after 5 seconds. The potentiometer value is read every 2 seconds and is automatically sent to the MQTT
// broker.
//
// Dependencies: PubSubClient.h, ESP8266WiFi.h, ESP8266HTTPClient.h
// Date: 11/20/2025 8:43 PM PT
// Compiler: PIO Version 3.3.4
// Atuhor: Zella Waltman
// OUTPUT: publishes 1 and 0 to outTopic/zellas_mqtt, potentiometer values to outTopic/zellas_mqtt/pot
// INPUT: Received value from the broker on inTopic  
// SETUP: To see the published values go to http://www.hivemq.com/demos/websocket-client/ 
//        subscribe to inTopic and outTopic, or Zella's MQTT app.
// Versions: 
//  v1: Nov-24-2022 - Cleaned up version 
//  v2: Nov-20-2025 - Zella's version
//-----------------------------

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

// WiFi variables
//-----------------------------
const char* ssid     = "BigPapa";      // Wifi Name
const char* password = "wificonnect";  // WiFi Password

// MQTT variables
//-----------------------------
const char* mqtt_server = "broker.mqtt-dashboard.com";

// Topic to get LED control (App -> ESP)
const char* subscribeTopic = "testtopic/temp/inTopic/zellas_mqtt";

// Topic to send switch events (ESP -> App)
const char* switchTopic = "testtopic/temp/outTopic/zellas_mqtt";

// Topic to send potentiometer values (ESP -> App)
const char* potTopic = "testtopic/temp/outTopic/zellas_mqtt/pot";

#define publishTimeInterval 2000  // ms between potentiometer publishes

// Pins
//-----------------------------
#define BTN    D1
#define ledPin D2
#define potPin A0   // potentiometer wiper

// ADC Constant
//----------------------------------
#define VIN 3.3 // 3.3 V Voltage

// Global Variables
//-----------------------------
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int  ledStatus = 0;

WiFiClient espClient;
PubSubClient client(espClient);

// Button / timer state
//-----------------------------
volatile bool g_btnEvent = false; // Button Press ISR Flag
bool switchPendingZero = false; // Sent 0 flag
unsigned long switchOneSentTime = 0; // Time when 1 was sent

// -----------------------------------------------------
// Button Interrupt
// -----------------------------------------------------
IRAM_ATTR void onButtonISR() {
  g_btnEvent = true;  // just set a flag, keep ISR short
}

// -----------------------------------------------------
// Initialize Button
// -----------------------------------------------------
void buttonInit() {
  pinMode(BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN), onButtonISR, FALLING);
}

// -----------------------------------------------------
// Potentometer Voltage Read
// -----------------------------------------------------
void potentiometer() {
  unsigned long now = millis();
  if (now - lastMsg > publishTimeInterval) { // Read value every 2000 ms (2s)
    lastMsg = now;

    int potVal = analogRead(potPin);  // 0–1023 ADC value

    // Convert ADC Value -> Voltage (value * 3.3V/2^10)
    float Vout = float(potVal) * (VIN / float(1023));

    // Publish Voltage as text
    snprintf(msg, MSG_BUFFER_SIZE, "%.3f", Vout);

    // Display to user
    Serial.print("Publishing pot value: ");
    Serial.print(msg);
    Serial.print(" to ");
    Serial.println(potTopic);

    // Publish Voltage
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
// MQTT Callback
// -----------------------------------------------------
// Called when a subscribed message arrives
void callback(char* topic, byte* payload, int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Print full payload for debugging
  String incoming = "";
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    Serial.print(c);
    incoming += c;
  }
  Serial.println();

  // Only handle messages for subscribeTopic
  if (strcmp(topic, subscribeTopic) == 0 && length > 0) {
    char first = payload[0];

    // Control LED from 1 or 0 input
    if (first == '1') {
      digitalWrite(ledPin, HIGH);  // LED ON
      ledStatus = 1;
      Serial.println("LED turned ON (from MQTT)");
    } 
    else if (first == '0') {
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

    // Create random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect to MQTT
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // resubscribe to MQTT
      client.subscribe(subscribeTopic);
      Serial.print("Subscribed to: ");
      Serial.println(subscribeTopic);
    } 
    else { // Fail Case
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
    if (current - lastBtn > 50) { // debounce ~50 ms
      lastBtn = current;

      // Acknowledge press for user
      Serial.println("Switch press detected");

      // Send 1 to switch topic on MQTT
      client.publish(switchTopic, "1");
      // Display for user
      Serial.print("Published 1 to ");
      Serial.println(switchTopic);

      // Start 5s timer to send 0
      switchPendingZero  = true;
      switchOneSentTime  = millis();
    }
  }

  // If 1 was sent, send 0 5s later
  if (switchPendingZero && (millis() - switchOneSentTime >= 5000)) {
    // Send 0 to switch topic on MQTT
    client.publish(switchTopic, "0");
    // Display for user
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
  pinMode(potPin, INPUT);

  Serial.begin(9600);
  setup_wifi(); // Wifi Connection Details

  digitalWrite(ledPin, LOW); // LED OFF at start

  buttonInit();  // Initialize button interrupt

  client.setServer(mqtt_server, 1883); // Connect to MQTT
  client.setCallback(callback);
}

// -----------------------------------------------------
// Loop
// -----------------------------------------------------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // handle MQTT traffic & trigger callback()

  // Check for button press (press = 1, then 0 after 5s)
  buttonPress();

  // Periodically publish potentiometer value
  potentiometer();
}