#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>

SoftwareSerial gsm(7, 6); // RX, TX
TMRpcm tmrpcm;
#define PHONE "9069595030"

boolean state = 1;


String ReadGSM() {
  int c;
  String v;
  while (gsm.available()) { //сохраняем входную строку в переменную v
    c = gsm.read();
    v += char(c);
    delay(10);
  }
  return v;
}

void ReadDtmf() {
  Serial.println("RING!");
  if (gsm.find(PHONE)) {
    gsm.println("AT+DDET=1");   // включаем DTMF
    delay(10);
    gsm.println("ATA");         // поднимаем трубку
    delay(500);

    if (state == 1) tmrpcm.play("vkl.wav");
    else tmrpcm.play("vikl.wav");

    while (tmrpcm.isPlaying() == 1);
    tmrpcm.disable();

    tmrpcm.play("per.wav");

    while (tmrpcm.isPlaying() == 1);
    tmrpcm.disable();

    while (1) { // в цикле
      String temp;
      temp = ReadGSM();
      delay(500);

      if (temp == "\r\n+DTMF: 1\r\n") {
        Serial.println("1"); // выполняем команду 1
        if (state == 1) {
          if (vvpar() == 1) {
            state = 0;
            tmrpcm.play("verno.wav");
          }
          else tmrpcm.play("neverno.wav");

          while (tmrpcm.isPlaying() == 1);
          tmrpcm.disable();
        } else {
          state = 1;
          tmrpcm.play("vkl.wav");
          while (tmrpcm.isPlaying() == 1);
          tmrpcm.disable();
        }
        break;
      } else if (temp == "\r\nNO CARRIER\r\n") { // если пришел отбой -выходим из цикла
        break;
      }
    }
    gsm.println("ATH0");         // на всякий случай сбросим вызов
    Serial.println("OK!");
  } else {
    gsm.println("ATH0");
    Serial.println("NUM.ERROR. OK!");
  }
}

boolean vvpar() {
  Serial.println("parol");
  tmrpcm.play("parol.wav");
  while (tmrpcm.isPlaying() == 1);
  tmrpcm.disable();
  int i = 0;

  String vvpas;

  while (1) { // в цикле
    String temp;
    temp = ReadGSM();
    delay(500);
    Serial.print("i = ");
    Serial.println(i);
    if (temp == "\r\n+DTMF: 1\r\n") {
      Serial.println("1"); // выполняем команду 1
      vvpas += '1';
      i++;

    } else if (temp == "\r\n+DTMF: 2\r\n") {
      Serial.println("2"); // выполняем команду 2
      vvpas += '2';
      i++;

    } else if (temp == "\r\n+DTMF: 3\r\n") {
      Serial.println("3"); // выполняем команду 3
      vvpas += '3';
      i++;

    } else if (temp == "\r\n+DTMF: 4\r\n") {
      Serial.println("4"); // выполняем команду 4
      vvpas += '4';
      i++;

    } else if (temp == "\r\n+DTMF: 5\r\n") {
      Serial.println("5"); // выполняем команду 5
      vvpas += '5';
      i++;

    } else if (temp == "\r\n+DTMF: 6\r\n") {
      Serial.println("6"); // выполняем команду 6
      vvpas += '6';
      i++;

    } else if (temp == "\r\n+DTMF: 7\r\n") {
      Serial.println("7"); // выполняем команду 7
      vvpas += '7';
      i++;

    } else if (temp == "\r\n+DTMF: 8\r\n") {
      Serial.println("8"); // выполняем команду 8
      vvpas += '8';
      i++;

    } else if (temp == "\r\n+DTMF: 9\r\n") {
      Serial.println("9"); // выполняем команду 9
      vvpas += '9';
      i++;

    } else if (temp == "\r\n+DTMF: 0\r\n") {
      Serial.println("0"); // выполняем команду 0
      vvpas += '0';
      i++;
    }
    else if (temp == "\r\nNO CARRIER\r\n") { // если пришел отбой -выходим из цикла
      break;
    }
    if (i >= 5) {
      Serial.println(vvpas);
      if (vvpas == "12345") return 1;
      else return 0;
    }
  }
}

void plays(String fn) {
  tmrpcm.play(fn.c_str());
  while (tmrpcm.isPlaying() == 1);
  tmrpcm.disable();
}

void setup() {
  tmrpcm.speakerPin = 9; // Динамик подключен к 9 - pin.
  if (!SD.begin(4)); // Здесь можно изменить № pin-CS - pin 4.
  
  Serial.begin(9600);
  gsm.begin(9600);
  
  gsm.println("AT+CLIP=1");
  gsm.println("ATE0");          
}

void loop() {
  gsm.println("AT");            // иначе модем засыпает

  if (gsm.find("RING")) {       // если нашли RING
    ReadDtmf();
  }
}

