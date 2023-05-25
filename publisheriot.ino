//Kelompok A2
//Sensor Kebocoran Gas LPG
//PUBLISHER


#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Untuk Mengatur Jaringan
const char* ssid = "SB SCP";
const char* password = "henjoo123";
const char* mqtt_server = "broker.emqx.io";

// Sensor Gas berada pada A0
#define GAS_PIN A0

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

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

// menerima data
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan Diterima [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

// Berfungsi untuk menghubungkan ke broker
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
      client.subscribe("iot_a_5");
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
  pinMode(GAS_PIN, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);     // Menginisialisasi LED sebagai OUTPUT
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  int nilaiGas = analogRead(GAS_PIN);
  delay(2000);
  snprintf (msg, MSG_BUFFER_SIZE, "%s", itoa(nilaiGas, msg, 10)); // itoa (konversi integer ke string)
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish("iot_unmul/iot_a_5/kadargas", msg); // fungsi untuk publish ke broker
}
