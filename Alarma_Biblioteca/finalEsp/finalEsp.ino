#include <WiFi.h>//
#include "time.h"//
#include <FirebaseESP32.h>
#include <WiFiManager.h>
const int wifiDesconectado = 5;//asignacion pin wifi
const int wifiConectado = 26;//asignacion pin wifi
const int encendido = 19;//asignacion pin wifi
const int apagado = 21;//asignacion pin wifi
const int espera = 22;//asignacion pin wifi
int e = 34;
int salida = 23;
int valor_salida;
int valor;
int power;
int a1 = 33; //PIN PARA CAMBIAR DE AUDIO 
int sonido;

const char* ntpServer = "co.pool.ntp.org";//
const long  gmtOffset_sec = -5 * 3600;//
const int   daylightOffset_sec = 3600;//


#define FIREBASE_HOST "app-pas-9d3be-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "BmDuHO4fwlCmyipb42QXHmHaA8TjQE69Cdrv4Z6q"
FirebaseData firebaseData;


void setup() {

  Serial.begin(9600);
  pinMode(encendido, OUTPUT);
  pinMode(apagado, OUTPUT);
  pinMode(espera, OUTPUT);
  pinMode(salida, OUTPUT);
  pinMode(a1, OUTPUT);
  pinMode(e, INPUT);

  inicializacionWifi();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //printLocalTime();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  delay(2000);
}

void loop() {
  ///
  digitalWrite(salida, LOW);
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(wifiDesconectado, LOW); // Apaga el LED
    digitalWrite(wifiConectado, HIGH); // Apaga el LED
    Firebase.getInt(firebaseData, "/power"); // Lee el valor de 'power' en cada iteración
    power = firebaseData.intData();
    Firebase.getInt(firebaseData, "/audio"); // Lee el valor de 'que audio se reproduce' en cada iteración
    sonido = firebaseData.intData();

    if (power == 1) {
      digitalWrite(apagado, LOW); // Utiliza el número de pin definido
      digitalWrite(encendido, HIGH); // Utiliza el número de pin definido
      digitalWrite(espera, LOW); // Enciende el LED
      Firebase.getInt(firebaseData, "/audio"); // Lee el valor de 'power' en cada iteración
      sonido = firebaseData.intData();
      if (sonido == 1) {
        digitalWrite(a1, HIGH);
      } else {
        digitalWrite(a1, LOW);
      }
      
      valor = digitalRead(e);
      Serial.println(valor);

      if (valor == HIGH) {
        manejoFecha();
        digitalWrite(salida, HIGH); // Apaga el LED
        delay(7100);
      }
      digitalWrite(salida, LOW);
    }
    
    else {
      Serial.println("!Apagado!");
      digitalWrite(encendido, LOW); // Utiliza el número de pin definido

      if (WiFi.status() == WL_CONNECTED) {
        digitalWrite(wifiDesconectado, LOW); // Utiliza el número de pin definido
        digitalWrite(wifiConectado, HIGH); // Utiliza el número de pin definido

      } else {

        digitalWrite(wifiDesconectado, HIGH); // Utiliza el número de pin definido
        digitalWrite(wifiConectado, LOW); // Utiliza el número de pin definido
      }

      digitalWrite(apagado, HIGH); // Utiliza el número de pin definido
      digitalWrite(espera, LOW); // Utiliza el número de pin definido
      Serial.print("powerl: ");
      Serial.println(power);
      delay(200);//cambio
    }
  } else {
    Serial.println("Desconectado");
    digitalWrite(salida, LOW);
    //digitalWrite(LED_PIN, LOW); // Enciende el LED
    digitalWrite(apagado, LOW); // Utiliza el número de pin definido
    digitalWrite(encendido, LOW); // Utiliza el número de pin definido
    digitalWrite(wifiConectado, LOW); // Apaga el LED
    digitalWrite(wifiDesconectado, HIGH); // Enciende el LED
    digitalWrite(espera, HIGH); // Enciende el LED
    inicializacionWifi();
    Serial.println("");
    Serial.println("");
    digitalWrite(a1, LOW); // Enciende el LED

  }
}

void inicializacionWifi() {
  WiFi.mode(WIFI_STA); 
  // pinMode(LED_PIN, OUTPUT); // Utiliza el número de pin definido
  pinMode(wifiDesconectado, OUTPUT); // Utiliza el número de pin definido
  pinMode(wifiConectado, OUTPUT); // Utiliza el número de pin definido
  digitalWrite(wifiDesconectado, HIGH); // Utiliza el número de pin definido
  digitalWrite(espera, HIGH); // Utiliza el número de pin definido

  WiFiManager wiFiManager;
  wiFiManager.resetSettings();

  bool res;
  res = wiFiManager.autoConnect("Silence", "12345678");

  if (!res) {
    Serial.println("Failed to connect!");
  }
  else {
    Serial.println("Connected :)");
  }
}

void manejoFecha() {
  //Tiempo
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char fechaBuffer[20]; // Tamaño suficiente para almacenar la fecha formateada
  char horaBuffer[10];  // Tamaño suficiente para almacenar la hora formateada

  // Formatear la fecha y la hora en los búferes
  strftime(fechaBuffer, sizeof(fechaBuffer), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  //strftime(horaBuffer, sizeof(horaBuffer), "%H:%M:%S", &timeinfo);

  // Almacenar la fecha y la hora en variables String
  String fechaActual = fechaBuffer;
  //String horaActual = horaBuffer;
 // Serial.println("Fecha: ");
  //Serial.println(fechaActual);

  //Tiempo
  FirebaseJson json;
  json.set("level", valor);
  json.set("date", fechaActual);
  if (Firebase.pushJSON(firebaseData, "/records", json)) {
  //  Serial.println("Datos enviados a Firebase!");
  } else {
    Serial.println("Error al enviar datos a Firebase.");
    Serial.println(firebaseData.errorReason());
  }
}
