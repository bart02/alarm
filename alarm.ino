#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>

SoftwareSerial gsm(7, 6); // RX, TX
TMRpcm tmrpcm;

String input_string;

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);
  tmrpcm.speakerPin = 9; // Динамик подключен к 9 - pin.
  if (!SD.begin(4)); // Здесь можно изменить № pin-CS - pin 4.
  gsm.println("AT+CLIP=1");
  gsm.println("ATE0");          // выключаем эхо
  //gsm.println("AT+CFUN=1,1");

}

void loop() {
  gsm.println("AT");            // иначе модем засыпает

  if (gsm.find("RING")) {       // если нашли RING
    Serial.println("RING!");
    if (gsm.find("9069595030")) {
      gsm.println("AT+DDET=1");   // включаем DTMF
      delay(10);
      gsm.println("ATA");         // поднимаем трубку
      delay(500);
      tmrpcm.play("parol.wav");
      while (gsm.available()) gsm.read();
      input_string = "";
      while (!gsm.find("NO ")) {
        while (gsm.available()) {
          char c = gsm.read();
          if (c == '\n') {
            if (StrInt(input_string) != 0) {
              Serial.println(StrInt(input_string));
            }
            input_string = "";
          } else {
            input_string += c;
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
  //gsm.println("ATH0");
}

int StrInt(String str) {
  String outstr;
  for (int i = 0; i < str.length(); i++) {
    outstr = str.substring(i);
    if (outstr.toInt() != 0) break;
  }
  return outstr.toInt();
}

