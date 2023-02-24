/* Comunicação WebServer e MQTT através do ESP-32 para verificar um botão, sensor LDR e envio de mensagens
   pelo próprio servidor web e/ou pelo aplicativo MQTT Dash.
   Obs1: O circuito para o sensor LDR consiste em um sensor LDR alimentado com 3.3V (do prórpio ESP-32) em 
   um dos terminais e no outro (onde sai o sinal a ser lido - pino 4) ligado a um resistor de 10k, cujo outro terminal 
   está ligado ao GND (também do ESP-32).
   Obs2: O server utilizado para o protocolo mqtt é: http://www.mqtt-dashboard.com/ ao ser executado pela rede
   ReinodeRocha
   Programação realizada pelo grupo: Marina, Cassio e Luan - 4E 
 */
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#define BOTAO 5
#define LED 2
#define LDR_PIN 4
#define DEBUG

//informações da rede WIFI
const char* ssid = "wifi_name";       //SSID da rede WIFI
const char* password = "wifi_password";    //senha da rede wifi

//informações do broker MQTT - Verifique as informações geradas pelo CloudMQTT
//const char* mqttServer = "192.168.0.171";   //server funciona com a rede IFRSautoIoT
//const char* mqttServer = "iot.eclipse.org";  // server
const char* mqttServer = "broker.mqtt-dashboard.com";  // server funciona 
const char* mqttUser = "sensorult4E";      //user
const int   mqttPort = 1883;                  //port
const char* mqttTopicPubUlt ="sensorult4E/Distancia"; //tópico que sera assinado
const char* mqttTopicPubBotao ="sensorult4E/Botao"; //tópico que sera assinado
const char* mqttTopicSubBotLed ="sensorult4E/BotLed"; //tópico que será postado

 
WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80); 
String dist;
String ldr;
String botao;
int ordem = 1;
float valorult; 
int valorbotOld = 0;
int sensorldr = 0;
int valorbot = digitalRead(BOTAO);
void msgbot(){
  if (valorbot == 0){   //se botão for pressionado
      server.send(200,"text/plain","ON"); // mensagem em web_sever "ON"
  }
  else {              // se não for pressionado
      server.send(200, "text/plain", "OFF"); // mensagem em web_server "OFF"
  }
}
void msgldr(){              // função do sensor ldr
  int a = analogRead(LDR_PIN);  //lê valor do sensor
  String ldr = String(a);       // transforma em string
  server.send(200,"text/plain", ldr); //envia ao web_server
}
void BotVir() {           // função do botão pelo web_server
  server.send(200, "text/plain", "Oie");  // web_server mostra mensagem "oie"
  Serial.println("Oie");                  // printa a mesma mensagem na serial p/ usuário verificar se está de acordo
  digitalWrite(LED, HIGH);                // liga LED tbm para confirmação visual do usuário           
}
void error() {                            // se a função chamada pelo web_server não existir
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, "text/plain", message);
}
void setup() {
  Serial.begin(9600);
  // abre a conexao de rede
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  pinMode(BOTAO, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef DEBUG
      Serial.println("Conectando ao WiFi..");
    #endif
  }
  #ifdef DEBUG
    Serial.println("Conectado na rede WiFi");
  #endif
 
  client.setServer(mqttServer, mqttPort);
   
  client.setCallback(callback);
 
  while (!client.connected()) {
    #ifdef DEBUG
    Serial.println("Conectando ao Broker MQTT...");
    #endif
 
    if (client.connect(mqttUser)) {
      #ifdef DEBUG
      Serial.println("Conectado");  
      #endif
 
    } else {
      #ifdef DEBUG 
      Serial.print("falha estado  ");
      Serial.print(client.state());
      #endif
      delay(2000);
 
    }
  }

  //chama as funções definidas acima conforme web_server requisitar
  server.on("/botao", msgbot);
  server.on("/sensor", msgldr);
  server.on("/virtual", BotVir);
  server.onNotFound(error);
  server.begin();   
}

 
void callback(char* topic, byte* payload, unsigned int length) {

  //armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);

  #ifdef DEBUG
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");
  #endif

  //aciona saída conforme msg recebida 
  if (strMSG == "1"){         //se msg "1"
     digitalWrite(LED, LOW);  //estado do LED 0
  }else if (strMSG == "0"){   //se msg "0"
     digitalWrite(LED, HIGH);   //acende LED e 
     Serial.println("Oie");     // printa mensagem
  }
 
}


//função pra reconectar ao servidor MQTT
void reconect() {
  //Enquanto estiver desconectado
  while (!client.connected()) {
    #ifdef DEBUG
    Serial.print("Tentando conectar ao servidor MQTT");
    #endif
     
    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("sensorult4E", mqttUser,"") :
                     client.connect("sensorult4E");

    if(conectado) {
      #ifdef DEBUG
      Serial.println("Conectado!");
      #endif
      //subscreve no tópico
      //client.subscribe(mqttTopicSub, 1); //nivel de qualidade: QoS 1
    } else {
      #ifdef DEBUG
      Serial.println("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10 s");
      #endif
      //Aguarda 10 segundos 
      delay(10000);
    }
  }
}
 

void loop() {
  if (!client.connected()) {
    reconect();
  }
  valorbot = digitalRead(BOTAO);   // lê estado do botão
  if (valorbot != valorbotOld){   // verifica se houve alteração no estado do botão
    botao = String(valorbot);     // transforma o valor do botão para String
    client.publish (mqttTopicPubBotao, botao.c_str());  // publica no tópico do mqtt o estado do botão
    valorbotOld = valorbot;       // atualiza o estado do botão
  }
  sensorldr = analogRead(LDR_PIN); // lê sensor LDR
  ldr = String(sensorldr);        // transforma o valor em String
  Serial.println (sensorldr);     // printa valor do sensor
  Serial.println (botao);         // printa estado do botão (1 ou 0)
  client.publish (mqttTopicPubUlt, ldr.c_str());  // publica no tópico mqtt o valor do sensor
  client.subscribe(mqttTopicSubBotLed);           // se inscreve no tópico do botão virtual no mqtt 
  server.handleClient();           // web_server

//  ordem++;
  delay (1000);
  client.loop();
}
