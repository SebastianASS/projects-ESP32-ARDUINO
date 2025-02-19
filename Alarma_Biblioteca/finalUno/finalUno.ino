//Este codigo permite integrar al dispositivo un sensor de sonido que detecta niveles de sonido, para definir en que momento se activa 
//la alarma. Fisicamente se realizó un sistema que simualara un reproductor de sonido para conectar una speakers y arrojar la alarma 
//al ambiente para dar aviso del ruido dentro de la Biblioteca.

#include "SD.h"
#define SD_ChipSelectPin 4
#include "TMRpcm.h"
#include "SPI.h"
TMRpcm tmrpcm;
int e = 6;//variable e = entrada - ENCENDIDO ESP32            
int s = 7;// variable s = salida                   ---
int ls = 3;//mostrar al usuario la sensibilidad
int r = 8;// variable r = rele
int MIC = 2;// variable MIC = microfono           ----
int a1 = 5;//variable a1(a uno) = // 
int a1e;//almacena el valor de a1
int valorE ;//variable valorE = valor entrada
int VALOR;//GUARDAR VALOR MIC
//const int relay = 0;

void setup() {
  Serial.begin(9600);
  pinMode(e, INPUT);
  pinMode(r, OUTPUT);
  //pinMode(s, OUTPUT);
  digitalWrite(r, HIGH);
  pinMode(ls, INPUT);
  pinMode(a1, INPUT);
}

void loop() {

  valorE = digitalRead(e);
  if (valorE == HIGH) {
    pinMode(MIC, INPUT);

    VALOR = digitalRead(MIC);
    Serial.println(VALOR);
    if (VALOR == HIGH) {
      a1e = digitalRead(a1);
      Serial.println(a1e);
      digitalWrite(ls, HIGH);
      digitalWrite(s, HIGH);
      tmrpcm.speakerPin = 9;
      if (!SD.begin(SD_ChipSelectPin)) {
        Serial.println("SD fail");
        return;
      }
      digitalWrite(r, LOW);
      tmrpcm.setVolume(6);

      if (a1e == HIGH) {
        tmrpcm.play("audio.wav");
      }
      else {
        tmrpcm.play("audio1.wav");
      }
      delay(7000);
      digitalWrite(s, LOW); // Apaga el LED
      digitalWrite(r, HIGH);
      delay(200);
    }
    digitalWrite(ls, LOW);
  }
}
