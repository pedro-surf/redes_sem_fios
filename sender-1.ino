#include <esp_now.h>
#include <WiFi.h>


//********************************************************
// SETANDO MAC PARA ESPNOW
uint8_t broadcastAddress[] = { 0x30, 0xAE, 0xA4, 0x9D, 0x20, 0x28 };

const char *ssid = "WIFI-NETWORK";
const char *password = "WIFI-PASSWORD";

//*************************************************
// MENSAGEM STRUCT

typedef struct struct_message {
  char a[32];
} struct_message;

struct_message myData;

//*************************************************
// FUNÇÕES DE RECEBER E ENVIAR ESP NOW

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Serial.println("\n\n");
}
//*************************************************

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  //*************************************************
  // WIFI MODE CONFIGS

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  Serial.println(WiFi.macAddress());

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

  // SETTING SENDER
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
// LOOPING ENVIANDO DADOS FIXOS PARA ESP - MIDDLER
void loop() {

  strcpy(myData.a, "VALOR DO 1");

  // SENDING BY ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
