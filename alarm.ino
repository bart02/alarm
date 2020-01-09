#define PHONE   "9069595030" // номер телефона для звонков, они будут приниматься только с этого номера
#define SIMRX   7 // пин RX SIM800L
#define SIMTX   6 // пин TX SIM800L
#define SPEAKER 9 // пин соединенный с передачей звука на SIM800L
#define SDPIN   4 // пин SD карты
#define MGPIN   3 // пин SD карты

#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>

SoftwareSerial gsm(SIMRX, SIMTX); // RX, TX
TMRpcm tmrpcm;

boolean state;
volatile boolean al = 0;
volatile boolean vskr = 0;

void(* resetFunc) (void) = 0; // объявляем функцию reset

void alarm() {
  if (digitalRead(MGPIN)) {
    al = 1;
    vskr = 1;
  }
}

void StateTo(boolean st) {
  if (st == 1) {
    state = 1;
    attachInterrupt(1, alarm, RISING);
  } else {
    state = 0;
    detachInterrupt(1);
  }
}

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

    if (vskr) {
      plays("vskr.wav");
      while (1) { // в цикле
        String temp;
        temp = ReadGSM();
        delay(500);

        if (temp == "\r\n+DTMF: 1\r\n") {
          Serial.println("1"); // выполняем команду 1
          resetFunc(); //вызываем reset
          break;
        } else if (temp == "\r\nNO CARRIER\r\n") { // если пришел отбой -выходим из цикла
          break;
        }
      }
    } else {
      if (state == 1) plays("vkl.wav");
      else plays("vikl.wav");

      plays("per.wav");

      while (1) { // в цикле
        String temp;
        temp = ReadGSM();
        delay(500);

        if (temp == "\r\n+DTMF: 1\r\n") {
          Serial.println("1"); // выполняем команду 1
          if (state == 1) {
            if (vvpar() == 1) {
              StateTo(0);
              plays("verno.wav");
            }
            else plays("neverno.wav");
          } else {
            StateTo(1);
            plays("vkl.wav");
          }
          break;
        } else if (temp == "\r\nNO CARRIER\r\n") { // если пришел отбой -выходим из цикла
          break;
        }
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
  plays("parol.wav");
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

void AlarmCall() {
  Serial.println("alarm");
  delay(1000);
  gsm.println("AT+DDET=1");
  delay(10);
  gsm.print("ATD+7");
  gsm.print(PHONE);
  gsm.println(";");

  while (1) { // в цикле

    String temp;
    temp = ReadGSM();
    delay(500);

    if (temp == "\r\n+DTMF: 1\r\n") {
      Serial.println("1"); // выполняем команду 1
      plays("vskr.wav");
      al = 0;
      break;
    } else if (temp == "\r\nNO CARRIER\r\n") { // если пришел отбой -выходим из цикла
      al = 0;
      break;
    } else if (temp == "\r\nBUSY\r\n") { // если пришел отбой -выходим из цикла
      break;
    } else if (temp == "\r\nNO ANSWER\r\n") { // если пришел отбой -выходим из цикла
      break;
    } else if (temp == "\r\nNO DIALTONE\r\n") { // если пришел отбой -выходим из цикла
      break;
    }
  }
  gsm.println("ATH0");         // на всякий случай сбросим вызов
  Serial.println("OK!");
}

void setup() {
  pinMode(MGPIN, INPUT_PULLUP);
  StateTo(1);
  tmrpcm.speakerPin = SPEAKER; // Динамик подключен к 9 - pin.
  if (!SD.begin(SDPIN)); // Здесь можно изменить № pin-CS - pin 4.

  Serial.begin(9600);
  gsm.begin(9600);

  gsm.println("ATH0");
  gsm.println("AT+CLIP=1");
  gsm.println("ATE0");
}

void loop() {
  gsm.println("AT");            // иначе модем засыпает
  if (al) {
    AlarmCall();
  }
  if (gsm.find("RING")) {       // если нашли RING
    ReadDtmf();
  }
}

