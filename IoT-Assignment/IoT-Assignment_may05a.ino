#include "arduino_secrets.h"

#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <OneWire.h>

WiFiClient wifi;
MqttClient mqttClient(wifi);
OneWire  ds(4);

int led_1 = 7;
int led_2 = 8;

char broker[] = "18.195.54.50";
int port = 1883;
char topic1[] = "terraTemps/1";
char topic2[] = "terraTemps/2";
char clientID[] = "terraTempClient";

int count = 0;

void setup() {
  Serial.begin(9600);

  delay(1500);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);

  // Making sure wifi is connected.
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(SECRET_SSID);
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  Serial.print("Connected. My IP address: ");
  Serial.println(WiFi.localIP());

  mqttClient.setId(clientID);
  mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);

  while (!connectToBroker()) {
    Serial.println("attempting to connect to broker");
    delay(1000);
  }
  Serial.println("connected to broker");
  
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(led_1, LOW);
  digitalWrite(led_2, LOW);
}

void loop() {
  if (!mqttClient.connected()) {
    Serial.println("reconnecting");
    connectToBroker();
  }

  // --------------------GET SENSOR "ID"-------------------
  // Heavily inspired code from: https://forum.arduino.cc/t/multiple-ds18b20-temperature-sensors-on-one-bus/139955/5
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  String romString = "";

  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
    romString += String(addr[i], HEX); // Coverting ROM to string for unique identification.
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end

  delay(5000);
  digitalWrite(LED_BUILTIN, HIGH);

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);

  // --------------------READ SENSOR DATA-------------------

  for ( i = 0; i < 9; i++) {
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  }

  // Printing sensor data
  celsius = (float)(raw / 16.0);
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.println(romString);

  // --------------------SEND DATA TO BROKER-------------------
  if (romString == "28c44c2d006c") {
    digitalWrite(led_1, HIGH);
    celsius = celsius - 1;
    mqttClient.beginMessage(topic1);
    mqttClient.print("temperature:");
    mqttClient.print(celsius);
    mqttClient.endMessage();
    count++;
  } else {
    digitalWrite(led_2, HIGH);
    celsius = celsius + 1;
    mqttClient.beginMessage(topic2);
    mqttClient.print("temperature:");
    mqttClient.print(celsius);
    mqttClient.endMessage();
    count++;
  }

  // --------------------"SHUTDOWN" FOR 1 MINUTE-------------------
  if (count == 2) {
    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(led_1, LOW);
    digitalWrite(led_2, LOW);
    delay(60000);
    digitalWrite(LED_BUILTIN, HIGH);
    count = 0;
  }
}

// CONNECTING TO BROKER
boolean connectToBroker() {
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MOTT connection failed. Error no: ");
    Serial.println(mqttClient.connectError());
    return false;
  }
  return true;
}