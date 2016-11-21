
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define WLAN_SSID       ""
#define WLAN_PASS       ""

#define ARDUINO_ID "123456789"
#define BASE_TOPIC "WateringOfPlants/microController/" ARDUINO_ID 
#define WATER_PLANTS BASE_TOPIC "/water"
#define MEASURE_MOISTURE BASE_TOPIC "/measure"
#define MOISTURE_VALUES BASE_TOPIC "/measuredValues/"

WiFiClient client;
PubSubClient mqttclient(client);

void waterPlant(int position, int time) {
  Serial.println("Water plant");
  Serial.println(position);
  Serial.println(time);
  delay(time);
  Serial.println("Watering end");  
}

void getMoistureValues(JsonArray& pins, int nrOfPins) {
  char concatenated[sizeof(MOISTURE_VALUES) + 1];
  for (int i = 0; i < nrOfPins; ++i) {
    sprintf(concatenated,"%s%i", MOISTURE_VALUES, pins[i].as<int>());
    Serial.println(concatenated);
    mqttclient.publish(concatenated, "hallo");
  }
}

void callback (char* topic, byte* payload, unsigned int length) {
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject((char*) payload);
  String parsedTopic(topic);
  Serial.println(topic);
  //free(topic);
  if (parsedTopic == WATER_PLANTS) {
    waterPlant(root["position"], root["time"]);
  } else if (parsedTopic == MEASURE_MOISTURE) {
    getMoistureValues(root["pins"], root["nrOfPins"]);
  } else {
    Serial.print("Unknown topic: ");
    Serial.println(topic);
  }
}

void setup(void) {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while(WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  mqttclient.setServer("test.mosca.io", 1883);
  mqttclient.setCallback(callback);
  
 // did that last thing work? sweet, let's do something
  if (mqttclient.connect("clientId-nr78ZXWMF5")) {
    Serial.println("Connected to mqtt broker");
    mqttclient.publish(BASE_TOPIC, "Initial");
    mqttclient.subscribe(WATER_PLANTS);
    mqttclient.subscribe(MEASURE_MOISTURE);
  }
}

void loop(void) {
  mqttclient.loop();
}
