#include <MFRC522.h> //RFID-RC522 kart okuyucu icin gerekli kutuphane
#include <LiquidCrystal_I2C.h> // LCD ve I2C haberlesmesi icin gerekli kutuphane
#include <SPI.h> //Serial haberlesme protokolu icin gerekli kutuphane
#include <Servo.h> //Serbvo motor icin gerekli kutuphane
#include <Wire.h> //I2C haberlesme yontemiyle donanimsal olarak senkron seri haberlesme saglayan kutuphane

//RFID tanimlamalari
int RST_PIN = 9;
int SS_PIN = 6;

//RC522 RFID karti ici pin aramasi
MFRC522 rfid(SS_PIN, RST_PIN);
//Kartınıza ait ID bilgisini giriniz.
//Girilen ID değeri örnektir.
byte ID[4] = {123, 45, 67, 89};

// Ultrasonik Sinyal pinleri
const int trigPin = 10;
const int echoPin = 7;

// LCD hakkinda, Ekranin türü
LiquidCrystal_I2C lcd(0x27,16,2);

//Servo
int servoPin = 8;
Servo motor;

//Mesafe islemleri
long duration;
int distance;

//BUZZER
int buzzerPin = 4;

void setup() 
{
  lcd.init(); //LCD ekranin kurulumunu gerceklestirmek icin kullanilir
  lcd.backlight(); //LCD ekranin arka isigi
  
  pinMode(buzzerPin, OUTPUT); //Buzzer cikis atamasi
  pinMode(trigPin, OUTPUT); //Trig pini cikis atamasi
  pinMode(echoPin, INPUT); //Echo pini giris atamasi

  motor.attach(servoPin); //Servo motor pin atamasi
  Serial.begin(9600); //Seri ara birimi icin hazir halde
  SPI.begin(); //SPI baglanti noktasi kurmak icin
  rfid.PCD_Init(); //RC522 modulunu baslatmak icin

  lcd.setCursor(0,0); //LCD ekranin hangi satir ve sutununa yazi yazilmaya baslayacak
  lcd.print("Final Projesine");  
  lcd.setCursor(2,1);  
  lcd.print("Hos Geldiniz");
  delay(3000);

  kartiokutunuz();
}

void loop()
{  
  // Yeni kart kontrolu
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Kartlardan birini sec
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  // Temas edilen Kart veya Akbil'e ait ID degerinin 4 parametrik degerinin sorgulama
  if (rfid.uid.uidByte[0] == ID[0] &&
    rfid.uid.uidByte[1] == ID[1] && 
    rfid.uid.uidByte[2] == ID[2] && 
    rfid.uid.uidByte[3] == ID[3] ) {
        digitalWrite(buzzerPin,HIGH);
        delay(200);
        digitalWrite(buzzerPin,LOW);
        delay(200);
        
        
        lcd.setCursor(0,0);
        lcd.print("Hos Geldiniz");
        ekranaYazdir();
        projeBaslat();
    }
    else{
      // Yetkisiz ID degerinde 75 ms araliklarla 3 defa buzzer tetikleniyor
      lcd.clear();
      lcd.print("Yetkisiz Kart!!!");
      digitalWrite(buzzerPin,HIGH);
      delay(75);
      digitalWrite(buzzerPin,LOW);
      delay(75);
      digitalWrite(buzzerPin,HIGH);
      delay(75);
      digitalWrite(buzzerPin,LOW);
      delay(75);
      digitalWrite(buzzerPin,HIGH);
      delay(75);
      digitalWrite(buzzerPin,LOW);
      delay(75);
      ekranaYazdir();
      kartiokutunuz();
    }
  // Okumayi birak
  rfid.PICC_HaltA();

}

void ekranaYazdir(){  
  // Kartin ID'sini goruntule
  lcd.setCursor(0,1);
  lcd.print("ID: ");
  for(int sayac = 0; sayac < 4; sayac++){
    lcd.print(rfid.uid.uidByte[sayac]);
    lcd.print(" ");
  }
  lcd.print("");
  delay(3000);
  lcd.clear();
}

void kartiokutunuz() {
  // Ilk acilistan 3 saniye sonra veya yanlis kart denemelerinde sonra gosterilecek fonksiyon
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("LUTFEN KARTI");  
  lcd.setCursor(4,1);  
  lcd.print("OKUTUNUZ");
}

void projeBaslat(){
  while(1){
        lcd.clear();
        // 15 derece ile 165 derece arasinda donme
        for(int i=15;i<=165;i++){  
          //Servo motor'a integer i degernini 100 ms araliklarla gonderme
          motor.write(i);
          delay(100);
          // Integer distance degerini calculateDistance fonksiyonunda gelen degere esitleme
          distance = calculateDistance();
  
          //lcd
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Donme  : +");
          lcd.print(i);
          lcd.setCursor(0,1);
          lcd.print("Mesafe : ");
          lcd.print(distance);         
          lcd.print(" cm");
        }

        // 165 derece ile 15 derece arasinda donme
        for(int i=165;i>15;i--){  
          //Servo motor'a integer i degernini 100 ms araliklarla gonderme
          motor.write(i);
          delay(100);
          // Integer distance degerini calculateDistance fonksiyonunda gelen degere esitleme
          distance = calculateDistance();
  
          //lcd
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Donme  : -");
          lcd.print(i);
          lcd.setCursor(0,1);
          lcd.print("Mesafe : ");
          lcd.print(distance);
          lcd.print(" cm");
    }
  }
}

int calculateDistance(){ 

  // Trig pinine dusuk guc gonderme
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);

  // Trig pinine yuksek guc gonderme
  digitalWrite(trigPin, HIGH); 
  //fonksiyonu, tek bir tam sayi (veya sayi) bagimsiz degiskenini kabul eder.
  delayMicroseconds(10);
  // Trig pinine dusuk guc gonderme
  digitalWrite(trigPin, LOW);
  // Sureyi olcmek
  duration = pulseIn(echoPin, HIGH); 
  //Mesafe degerini hesaplama
  distance= duration*0.034/2;
  // Hesaplanan mesafe degeri calculateDistance icin geriye deger dondurur
  return distance;
}
