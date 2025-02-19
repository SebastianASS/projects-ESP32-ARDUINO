#include <WiFi.h>
#include "time.h"
#include <FirebaseESP32.h>
#include <HTTPClient.h>
#include <UrlEncode.h> 
const char* ntpServer = "co.pool.ntp.org";//
const long  gmtOffset_sec = -6 * 3600;//
const int   daylightOffset_sec = 3600;//
const char* callmebot_host = "api.callmebot.com";
const char* ssid     = "CELADORES INSCARMEN";
const char* password = "Mono2021*";
#define FIREBASE_HOST "pas-timbre-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "V7vflmrQCj3umSyl7vmgtdVZWLgj5IEmltbnSW9I"
FirebaseData firebaseData;
String fechaActual;
String horaActual;
int relay = 4;
int led = 2;


void setup() {
  pinMode(relay,OUTPUT);
  pinMode(led,OUTPUT);
  digitalWrite(relay,LOW);
  Serial.begin(9600);
  delay(1000);
  WiFi.begin(ssid, password);
  delay(1000);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  delay(1000);
}

void loop() {


   if (WiFi.status() == WL_CONNECTED) {
     Firebase.getInt(firebaseData, "/apagado");
     int apagado = firebaseData.intData();
     if(apagado==1){
      digitalWrite(led,HIGH);
    Firebase.getInt(firebaseData, "/activar");
    int activar = firebaseData.intData();
    if(activar==1){
    Firebase.getInt(firebaseData, "/tiempo");
    int tiempoAlarma = firebaseData.intData();
    digitalWrite(relay,HIGH);
    delay(tiempoAlarma);
    digitalWrite(relay,LOW);
    delay(1000);
    }
   }else{
    Serial.println("Apagado");
    digitalWrite(led,LOW);
    delay(1000);
   }
  }
}

 void tiempo() {
 struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char fechaBuffer[30];
  char horaBuffer[30];  
  //Formatear la fecha y la hora en los búferes
  strftime(fechaBuffer, sizeof(fechaBuffer), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  fechaActual = fechaBuffer;
  strftime(horaBuffer, sizeof(fechaBuffer), "%H:%M", &timeinfo);
  horaActual = fechaBuffer;
 }


