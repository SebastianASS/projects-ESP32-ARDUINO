//Este codigo permite la lectura de sensores de movimiento, cuando uno de los sensores detecta cambios de temperatura en el ambiente,
//automaticamente realizaba una llamada a un usuario de telegram seleccionado desde la base de datos, esto con el fin de dar aviso 
//a la comunidad o las autoridades de que existe presencia inusual. Ademas, envia una señal a la base de datos para que sea leida por 
//el otro dispositivo y encienda la alarma del colegio.

#include <WiFi.h>
#include "time.h"
#include <FirebaseESP32.h>
#include <UrlEncode.h>
#include <HTTPClient.h>

const char* ntpServer = "co.pool.ntp.org";//
const long  gmtOffset_sec = -6 * 3600;//
const int   daylightOffset_sec = 3600;//
const char* callmebot_host = "api.callmebot.com";
const char* ssid     = "INFORMATICA";
const char* password = "informatica24#";

#define FIREBASE_HOST "pas-timbre-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "V7vflmrQCj3umSyl7vmgtdVZWLgj5IEmltbnSW9I"
FirebaseData firebaseData;

int sensor1 = 14;
int sensor2 = 12;
int led = 2;
String fechaHora;
String inicio;
String finalizacion;

void setup() {
  // put your setup code here, to run once:
  pinMode(sensor1,INPUT);
  pinMode(sensor1,INPUT);
  pinMode(led,OUTPUT);
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
        int valorSensor1 = digitalRead(sensor1);
        int valorSensor2 = digitalRead(sensor2);
        Serial.print("Lectura sensor 1: ");
        Serial.println(valorSensor1);
        Serial.print("Lectura sensor 2: ");
        Serial.println(valorSensor2);
        delay(350);
       if(valorSensor1==1 || valorSensor2==1){
          fechaActual();
          fecha_activacion_db();
          if (comprobarFecha(fechaHora, inicio, finalizacion)){
             Firebase.setInt(firebaseData, "/activar",1);
             llamadoTelegram();
             delay(100);
             enviarActivacion();
             Firebase.setInt(firebaseData, "/activar",0);
             delay(800);
           }
          }
        }else{
          Serial.println("Apagado");
          digitalWrite(led,LOW);
          Firebase.setInt(firebaseData, "/activar",0);
     }
  }
}

void fecha_activacion_db(){
        Firebase.getInt(firebaseData, "/inicio"); // Lee el valor de 'power' en cada iteración
        inicio = firebaseData.stringData();
        Firebase.getInt(firebaseData, "/finalizacion"); // Lee el valor de 'power' en cada iteración
        finalizacion = firebaseData.stringData();
}


void llamadoTelegram(){
   Firebase.getInt(firebaseData, "/llamadas");
    String usuario = firebaseData.stringData();
    delay(400);
    String mensaje = "Se ha detectado presencia inusual dentro de las instalaciones de la inscarmen, por favor acudir inmediatamente";
    delay(400);
    String url = "http://api.callmebot.com/start.php?user=" + usuario + "&text=" + urlEncode(mensaje) + "&lang=es-US-Neural2-C&rpt=2";
    Serial.println("Llama hecha a: "+usuario);
    HTTPClient http;
    delay(300);
    http.begin(url);
    delay(300);
    int httpResponseCode = http.POST(url);
    delay(300);
    if (httpResponseCode == 200) {
      Serial.println("Error en la llamada");
      Serial.print("Código de respuesta HTTP: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.println("Se realizó la llamada");
  }
}


void fechaActual() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char fechaBuffer[20];
  char horaBuffer[10];  
  strftime(fechaBuffer, sizeof(fechaBuffer),"%Y-%m-%dT%H:%M:%S",&timeinfo);
  fechaHora = fechaBuffer;
}
bool comprobarFecha(const String& fechaHora, const String& inicio, const String& cierre) {
  return (fechaHora >= inicio) && (fechaHora <= cierre);
}

void enviarActivacion(){
  FirebaseJson json;
  json.set("date", fechaHora);
  if (Firebase.pushJSON(firebaseData, "/historial", json)) {
  //  Serial.println("Datos enviados a Firebase!");
  } else {
    Serial.println("Error al enviar datos a Firebase.");
    Serial.println(firebaseData.errorReason());
  }
}
