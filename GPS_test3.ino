/*Marina Zanotta Rocha - GPS guide controlled by ESP32 connected through WiFi*/

#define Cassino_lat -32.2849370
#define Cassino_lon -52.2615010
#define Australia_lat -32.040556
#define Australia_lon -52.089722
#define Casa_lat -32.057691
#define Casa_lon -52.857451

#include <stdlib.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define TmpAtualiza 5000
#define TmpDesvio 2000
#define TmpDobrar 3500
#define TmpVolta  5000
#define MotorD    18
#define MotorE    19

double latitude = Casa_lat;     //latitude do destino
double longitude = Casa_lon;    //longitude do destino
unsigned long intervalo = 0;
double distance = 0.0;
double alinhamento = 0.0;
const char *direcao = 0;
int desvio = 0;
float velocidade = 0.0;

const char* ssid = "wifi_name";
const char* password = "wifi_password";

int ok;
String msg;
char buf[20];
TinyGPSPlus gps;


WebServer server(80);

String gpsLocal(){
  return String(gps.location.lat(), 6) + String(",") +
         String (gps.location.lng(), 6) + String ("\n");
  
}

String gpsDataHora(){
  return String (gps.date.day())    + String("/") +
         String (gps.date.month())  + String ("/") +
         String (gps.date.year())   + String ("\n") +
         String (gps.time.hour())   + String (":") +
         String (gps.time.minute()) + String (":") +
         String (gps.time.second()) + String ("\n"); 
}
String destino(){
  return String (latitude, 6) + String(",") +
         String (longitude, 6) + String ("\n");
  
}
String paraOnde (){         //De acordo com o desvio necessário envia mensagem para web da ação a ser tomada
  int angulo = (int)(360 + TinyGPSPlus :: courseTo (gps.location.lat(), gps.location.lng(),latitude, longitude) - gps.course.deg())% 360;
  if (TinyGPSPlus :: distanceBetween (gps.location.lat(), gps.location.lng(),latitude, longitude) > 10){
    if (angulo > 345 || angulo < 15){
      return String ("Seguir em frente");
    }
    if (angulo >= 15 && angulo <= 45){
      return String ("Desviar para esquerda");
    }
    if (angulo > 45 && angulo < 105){
      return String ("Dobrar para a esquerda");
    }
    if (angulo >= 105 && angulo <= 180){
      return String ("Virar para a esquerda");
    }
    if (angulo >= 315 && angulo <= 345){
      return String ("Desviar para a direita");
    }
    if (angulo > 255 && angulo < 315){
      return String ("Dobrar para a direita");
    }
    if (angulo > 180 && angulo < 255){
      return String ("Virar para a direita");
    } 
  }
   else if (TinyGPSPlus :: distanceBetween (gps.location.lat(), gps.location.lng(),latitude, longitude) <= 20){
      return String ("Sucesso!!! Vc chegou ao seu destino!"); 
   }
}

void montaMsg(){
  msg = "Agora: ";
  msg += gpsDataHora();
  msg += "Local: ";
  msg += gpsLocal();
  msg += "Destino: ";
  msg += destino();
  msg += "Distancia: ";
  msg += TinyGPSPlus :: distanceBetween (gps.location.lat(), gps.location.lng(),latitude, longitude); //Distância em metros
  msg += "m"+ String ("\n");
  msg += "Velocidade: ";
  msg += gps.speed.kmph();
  msg += "Km/h" + String ("\n");
  msg += "Direction: ";
  msg += TinyGPSPlus :: cardinal (TinyGPSPlus :: courseTo (gps.location.lat(), gps.location.lng(),latitude, longitude)) + String ("\n"); 
  msg += "Desvio: ";
  msg += (int)(360 + TinyGPSPlus :: courseTo (gps.location.lat(), gps.location.lng(),latitude, longitude) - gps.course.deg())% 360 + String ("\n");
  msg += paraOnde() + String ("\n");
  msg += "Satelites: ";
  msg += String(gps.satellites.value()) + String("\n");
  ok = 1;
}

void handleRoot() {
  if (ok){
     server.send(200, "text/plain", msg);
     msg = "";
     ok = 0;
  } else {
     server.send(200, "text/plain", "Atualizando local");
  }
}

void handleNotFound() {
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
void Parada (){
  Serial.print ("Siga para: ");
  Serial.println (TinyGPSPlus :: cardinal (TinyGPSPlus :: courseTo (gps.location.lat(), gps.location.lng(),latitude, longitude)));
}
void chegou(){                                                //Se chegar no destino
  if (TinyGPSPlus :: distanceBetween (gps.location.lat(), gps.location.lng(),latitude, longitude) < 10){
    Serial.println ("Sucesso!!! Vc chegou ao seu destino!");  //Printa mensagem
    digitalWrite(MotorD, LOW);                                //Desliga motores
    digitalWrite(MotorE, LOW);
  }
}
void desviarDireita (){                       //Realizar desvio para direita (se entre 15° a 45° à esquerda do alvo)
  if (millis() <= intervalo + TmpDesvio){     //Tempo de 2s
    digitalWrite(MotorD, HIGH);               //Ligar motor da direita
    Serial.println ("Desviar para direita");  //Printa ação a ser tomada
    intervalo = millis();                     //Atualiza tempo
    digitalWrite(MotorE, LOW);
  }  
}
void desviarEsquerda (){                      //Realizar desvio para esquerda (se entre 15° a 45° à direita do alvo)
  if (millis() <= intervalo + TmpDesvio){     //Tempo de 2s
    digitalWrite(MotorE, HIGH);               //Ligar motor da esquerda
    Serial.println ("Desviar para esquerda"); //Printa ação a ser tomada
    intervalo = millis();                     //Atualiza tempo
    digitalWrite(MotorD, LOW);
  }  
}
void dobrarDireita (){                      //Realizar retorno maior para direita (se entre 45° a 105° à esquerda do alvo)
  if (millis() <= intervalo + TmpDobrar){   //Tempo de 3,5s
    digitalWrite(MotorD, HIGH);             //Ligar motor da direita
    Serial.println ("Dobrar para direita"); //Printa ação a ser tomada
    intervalo = millis();                   //Atualiza tempo
    digitalWrite(MotorE, LOW);
  }
}
void dobrarEsquerda (){                       //Realizar retorno maior para esquerda (se entre 45° a 105° à direita do alvo)
  if (millis() <= intervalo + TmpDobrar){     //Tempo de 3,5s
    digitalWrite(MotorE, HIGH);               //Ligar motor da esquerda
    Serial.println ("Dobrar para esquerda");  //Printa ação a ser tomada
    intervalo = millis();                     //Atualiza tempo
    digitalWrite(MotorD, LOW);
  }  
}
void virarDireita (){                       //Realizar volta para direita (se entre 105° e 180° à esquerda do alvo)
  if (millis() <= intervalo + TmpVolta){    //Tempo de 5s
    digitalWrite(MotorD, HIGH);             //Ligar motor da direita
    Serial.println ("Virar para direita");  //Printa ação a ser tomada
    intervalo = millis();                   //Atualiza tempo
    digitalWrite(MotorE, LOW);
  }
}
void virarEsquerda (){                       //Realizar volta para esquerda (se entre 105° e 180° à direita do alvo)
  if (millis() <= intervalo + TmpVolta){    //Tempo de 5s
    digitalWrite(MotorE, HIGH);             //Ligar motor da esquerda
    Serial.println ("Virar para esquerda"); //Printa ação a ser tomada
    intervalo = millis();                   //Atualiza tempo
    digitalWrite(MotorD, LOW);
  }
}
void setup(void) {
  Serial.begin(115200);

  Serial2.begin(9600, SERIAL_8N1, 16, 17); // GPS
  intervalo = millis();
  pinMode (MotorD, OUTPUT);
  pinMode (MotorE, OUTPUT);


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  distance = TinyGPSPlus :: distanceBetween (gps.location.lat(), gps.location.lng(),latitude, longitude); //Distância em metros
  alinhamento = TinyGPSPlus :: courseTo (gps.location.lat(), gps.location.lng(),latitude, longitude); //
  direcao = TinyGPSPlus :: cardinal (alinhamento);          // direção cardeal
  desvio = (int)(360 + alinhamento - gps.course.deg())%360; // desvio
  velocidade = gps.speed.kmph();            // Velocidade em Km/h
  if (millis() > intervalo + TmpAtualiza){  // Atualizar a cada 5 segundos
      if (distance <= 10){                  // Se a distância for menor que 10 m considerar como destino atingido
        chegou();
      }
      else {
        if (velocidade < 1.0){              //Velocidade menor que 1,0km/h é considerado como parada
          Parada();
        }
        if (desvio > 345 || desvio < 15){
          Serial.println ("Seguir em frente");
        }
        if (desvio >= 15 && desvio <= 45){
          desviarEsquerda();
        }
        if (desvio > 45 && desvio < 105){
          dobrarEsquerda();
        }
        if (desvio >= 105 && desvio <= 180){
          virarEsquerda();
        }
        if (desvio >= 315 && desvio <= 345){
          desviarDireita();
        }
        if (desvio > 255 && desvio < 315){
          dobrarDireita();
        }
        if (desvio > 180 && desvio < 255){
          virarDireita();
        }
     }
    intervalo = millis();
  }
  if (Serial2.available() > 0)
     if (gps.encode(Serial2.read()))
        montaMsg();
  
  server.handleClient();
}
