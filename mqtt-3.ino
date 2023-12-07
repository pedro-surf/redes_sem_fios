#include <PubSubClient.h>
#include <esp_now.h>
#include <WiFi.h>

const char *ssid = "WIFI-NETWORK";
const char *password = "WIFI-PASSWORD";

//*************************************************
// MENSAGEM STRUCT
typedef struct struct_message {
  char a[32];
} struct_message;

struct_message myData;


//*************************************************
// MQTT Broker, acesso www.emqx.io/online-mqtt-client
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "ufsc";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);  // AP e Station
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the Wi-Fi network");

  //*************************************************
  // FUNÇÕES PARA INICIALIZAÇÃO DO ESPNOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW initialized");
  esp_now_register_recv_cb(OnDataRecv);


  //*************************************************
  // CONFIG DO MQTT
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(mqttCallback);
  String client_id = "Border-esp";

  while (!client.connected()) {
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public EMQX MQTT broker connected");
      client.publish(topic, "Esp conectado");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.subscribe(topic);
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.println();
  // PUBLICAR NO MQTT ASSIM QUE RECEBER
  client.publish(topic, myData.a);
}



void loop() {
}
