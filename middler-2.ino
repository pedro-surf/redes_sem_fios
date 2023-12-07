#include <esp_now.h>
#include <WiFi.h>

// ESP ATUAL: { 0x30, 0xAE, 0xA4, 0x9D, 0x20, 0x28 };
// SETANDO MAC PARA ESPNOW
uint8_t broadcastAddress[] = { 0x30, 0xAE, 0xA4, 0x9C, 0xCD, 0xE8 };

const char *ssid = "WIFI-NETWORK";
const char *password = "WIFI-PASSWORD";

//*************************************************
// MENSAGEM STRUCT

typedef struct struct_message {
  char a[32];
} struct_message;

struct_message myData;
struct_message myData1;

//*************************************************
// FUNÇÕES DE RECEBER E ENVIAR ESP NOW

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

}

//*************************************************

void setup() {
  
  Serial.begin(115200);
  //*************************************************
  // WIFI MODE CONFIGS

  WiFi.mode(WIFI_AP_STA);
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
  // SETTING RECEIVER E SENDER
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  // SETTING PEER
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // ADD PEER
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}
//*************************************************
// LOOPING ENVIANDO DADOS FIXOS PARA ESP - RE:MQTT

void loop() {
  //ENVIANDO VALOR FIXO DO SEGUNDO ESP
  strcpy(myData1.a, "char DOIS");

  // SENDING BY ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData1, sizeof(myData1));

  if (result == ESP_OK) {
    Serial.println("Retransmited with success");
  } else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
