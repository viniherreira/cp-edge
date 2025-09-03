/*
  CP 4 - Edge Computing
  Autores: Gabriel Santos, Vinicius Herreira
*/

#include <WiFi.h> // Importa a biblioteca Wifi
#include "DHT.h" // Importa a biblioteca DHT
#include "ThingSpeak.h" // Importa a biblioteca ThingSpeak

const char* ssid = "Wokwi-GUEST"; // Nome da rede WiFi
const char* password = ""; // Senha da rede WiFi

unsigned long channelID = 3060776; // ID do canal do ThingSpeak
const char* writeAPIKey = "LHX0ATUB923LLJDF"; // Chave de escrita do canal

#define DHTPIN 15 // Pino do ESP32 para leitura do sensor DHT
#define DHTTYPE DHT22 // Tipo do sensor DHT (no Wokwi só existe o DHT22, mas usamos o DHT11 na prática)
#define LDR_PIN 34 // Pino do ESP32 para leitura do LDR

// Criação do objeto DHT com o pino e tipo definidos
DHT dht(DHTPIN, DHTTYPE);

// Cliente WiFi utilizado pela biblioteca ThingSpeak
WiFiClient client;

void setup() {
  Serial.begin(115200);  // Inicia a comunicação serial com a taxa de 115200 bps
  WiFi.begin(ssid, password); // Inicia a conexão com a rede WiFi

  // Enquanto não conectar, exibe mensagem a cada 1 segundo
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("WiFi conectado!"); // Exibe mensagem de sucesso quando a conexão for estabelecida
  
  ThingSpeak.begin(client); // Inicializa a comunicação com o ThingSpeak
  dht.begin(); // Inicializa o sensor DHT
}

void loop() {
  // Realiza a leitura dos sensores
  float temperatura = dht.readTemperature(); // Lê a temperatura (°C)
  float umidade = dht.readHumidity(); // Lê a umidade relativa (%)
  int luminosidade = analogRead(LDR_PIN); // Lê a luminosidade (de 0 a 4095 no ESP32)

  // Se a leitura do DHT falhar, exibe erro e interrompe a execução da função
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Erro na leitura do sensor DHT11");
    return;
  }

  // Exibe os valores lidos no monitor serial
  Serial.print("Temp: ");
  Serial.print(temperatura);
  Serial.print(" °C | Umidade: ");
  Serial.print(umidade);
  Serial.print(" % | Luminosidade: ");
  Serial.println(luminosidade);

  // Atribui os valores aos campos correspondentes no ThingSpeak
  ThingSpeak.setField(1, temperatura); // Campo 1 = Temperatura
  ThingSpeak.setField(2, umidade); // Campo 2 = Umidade
  ThingSpeak.setField(3, luminosidade); // Campo 3 = Luminosidade

  // Envia os dados para o canal do ThingSpeak
  int statusCode = ThingSpeak.writeFields(channelID, writeAPIKey);

  // Se o envio for bem-sucedido (código 200), exibe mensagem de sucesso
  if (statusCode == 200) {
    Serial.println("Dados enviados com sucesso!");
  } else {
    // Caso contrário, exibe o código do erro
    Serial.print("Erro no envio: ");
    Serial.println(statusCode);
  }

  delay(20000); // Aguarda 20 segundos antes da próxima leitura
}