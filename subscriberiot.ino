//Kelompok A2
//Sensor Kebocoran Gas LPG
//SUBSCRIBER

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Untuk Mengatur Jaringan
const char* ssid = "SB SCP";
const char* password = "henjoo123"; 
const char* mqtt_server = "broker.emqx.io"; // broker gratisan

//Untuk Mengatur PIN LED(D5) dan PIN BUZZER (D6)
#define BUZZER_PIN D6
#define LED_PIN D5

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
int maxgas = 300;
int kadarGas;

void setup_wifi() {
  delay(10);
  //Memulai Koneksi ke WIFI
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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Fungsi untuk menerima data
void callback(char* topic, byte* payload, unsigned int length) {

  if (strcmp(topic, "iot_unmul/iot_a_5/kadargas")== 0)  {
    Serial.print("Kadar Gas ");
    String gas = ""; // variabel untuk menyimpan data yang berbentuk array char
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      gas += (char)payload[i]; // menyimpan kumpulan char kedalam string
    }
    Serial.println(" PPM");
    kadarGas = gas.toInt();
    
  }
  else if (strcmp(topic, "iot_unmul/iot_a_5/maxgas") == 0) {
    Serial.print("Maksimal Gas ");
    String maks = ""; // variabel untuk menyimpan data yang berbentuk array char
    for (int i = 0; i < length; i++) {
      maks += (char)payload[i];// menyimpan kumpulan char kedalam string
    }
    maxgas = maks.toInt();
    Serial.print(maxgas);
    Serial.println(" PPM");
    
  }}
  

// Berfungsi untuk mengubungkan ke broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("iot_unmul/iot_a_5/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);  // Inisialisasi pin buzzer
  pinMode(LED_PIN, OUTPUT);     // Inisialisasi pin LED
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883); // setup awal ke server mqtt
  client.setCallback(callback); 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if(kadarGas > maxgas){
      tone(BUZZER_PIN,1000,2000);
      digitalWrite(LED_PIN, HIGH);  // Ketika berada di HIGH maka LED menyala (Kadar Gas >600)
      
    }
    else if(kadarGas <= maxgas){
      noTone(BUZZER_PIN);
      digitalWrite(LED_PIN, LOW);  // Ketika di berada di LOW maka LED mati (Kadar Gas <600)
      
    }
}
