#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define PH_PIN A2 // ph seviyesi analog port
#define SUSEVIYE_PIN A1 // su seviye analog port

float voltage,phValue,temperature = 25; // oda sıcaklığına göre ph ölçümü yapsın 
DFRobot_PH ph; // ph tanımlaması

int samandira = 7; // şamandıra bağlantısı

int IN1 = 5; // valf bağlantı
int IN2 = 6; 

int IN3 = 4; // motor - 1 - ( ph + )
//int IN4 = 9; // led ekran bağlantısı için 1 pin eksiği vardı
               // motorları tek yönde çevireceğimiz için o bacak low bacak olacağı için 
               // ground dan bağladım. biraz güçsüz oluyor ama iş görüyor.
               // ekran bağlantısını başka bir arduino ile yaparak daha komplike haline getirmek istemedim.

int IN5 = 2; // motor - 2 -
int IN6 = 3; 

// LCD ekran için bağlantı pinleri
const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

String phString  = "";

void setup() {
  Serial.begin(9600);
  pinMode(samandira, INPUT_PULLUP);
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT); 
  ph.begin();
  lcd.begin(16, 2);  // 16 sütun, 2 satır
}

void loop() {
  int suSeviyesi = analogRead(SUSEVIYE_PIN); // su seviyesini ölçüyorum
  Serial.println(suSeviyesi);
  int samandiraol = digitalRead(samandira); // şamandıra durumunu ölçüyorum
  if(suSeviyesi < 200 && samandiraol == 0) { // eğer su seviyesi 200 ün altında ve şamandıra su yok diyorsa
    Serial.print("Valf Aciliyor ..."); // valfi açıyorum ve su dolduruyorum
    lcd.setCursor(0,0); // lcd ilk satır
    lcd.write(".Valf Aciliyor..");
    lcd.setCursor(0,1); // lcd ikinci satır
    lcd.write("                ");
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    while(digitalRead(samandira) == 0){ // şamandıradan gelen veri 1 olana kadar valfi kapatmıyorum.
      Serial.println("Sulaniyor ...");
      lcd.setCursor(0,0); // lcd ilk satır
      lcd.write("...Sulaniyor ...");
      lcd.setCursor(0,1); // lcd ikinci satır
      lcd.write("                ");
      delay(1000);
    }
    Serial.println("Su Doldu ..."); // su doldu valfi kapat
    lcd.setCursor(0,0); // lcd ilk satır
    lcd.print("....Su Doldu ...");
    lcd.setCursor(0,1); // lcd ikinci satır
    lcd.print("                  ");
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    delay(1000);
  }
  else
  {
    Serial.print("Su Var ..."); // diğer durumlarda su var !
    lcd.setCursor(0,0); // lcd ilk satır
    lcd.print(".....Su Var ....");
    lcd.setCursor(0,1); // lcd ikinci satır
    lcd.print("                  ");
    delay(1000);
  }
  
  // suyun ph değeri ölçümü ve stabil hale getirilmesi
  // suyun ph değerinin 6 ile 7 arasında olmasını istiyoruz.
  static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U){                  // saniyede 1
        timepoint = millis();
        voltage = analogRead(PH_PIN)/1024.0*5000;  // voltaj okuyoruz.
        phValue = ph.readPH(voltage,temperature);  // voltaj a göre oda sıcaklığına bağlı olarak ph hesabını yapıyoruz.
        lcd.setCursor(0,0); // lcd ilk satır
        phString = "....pH : ";
        phString  += String(phValue);
        phString += "..."; // 16 haneye tamamlıyoruz.
        lcd.print(phString);
        Serial.println(phValue,2);
        lcd.setCursor(0,1); // lcd ikinci satır
        // 7 in altında 6 üstünde değerlerde motorlar çalışmayacak.
        if(phValue > 7)
        {
          digitalWrite(IN5, HIGH);
          digitalWrite(IN6, LOW);
          lcd.print(".PH - Calisiyor.");
          delay(500); 
          digitalWrite(IN5, LOW);
          digitalWrite(IN6, LOW); 
        }
        else if(phValue < 6)
        {
          digitalWrite(IN3, HIGH);
          lcd.print(".PH + Calisiyor.");
          delay(500);
          digitalWrite(IN3, LOW);
        } 
        else
          lcd.print("...PH Sabit !...");
        delay(1000);
    }
    ph.calibration(voltage,temperature);
  delay(1000);
}
