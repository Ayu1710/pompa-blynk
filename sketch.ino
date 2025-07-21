#define BLYNK_TEMPLATE_ID "TMPL6O_1Ip55-"
#define BLYNK_TEMPLATE_NAME "Pompa air blynk otomatis"
#define BLYNK_AUTH_TOKEN "i043k_PH3bq16VJp_jlnKUb8lgIx1qNS"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi LCD: alamat I2C 0x27, ukuran 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define SENSOR_LOW_PIN 34
#define SENSOR_HIGH_PIN 35
#define RELAY_PIN 26

int modeManual = 0;
int manualState = 0;

BLYNK_WRITE(V0) {
  modeManual = param.asInt(); // 0=auto, 1=manual
}

BLYNK_WRITE(V1) {
  manualState = param.asInt(); // pompa manual on/off
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Pompa Air Blynk");
  lcd.setCursor(0, 1);
  lcd.print("Status: Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  Blynk.run();

  int sensorLow = analogRead(SENSOR_LOW_PIN);
  int sensorHigh = analogRead(SENSOR_HIGH_PIN);

  bool lowAir = sensorLow < 1000;    // air habis
  bool highAir = sensorHigh > 3000;  // air penuh

  // Kirim data sensor ke Blynk
  Blynk.virtualWrite(V2, sensorLow);
  Blynk.virtualWrite(V3, sensorHigh);

  // Status air
  String statusAir;
  if (highAir) {
    statusAir = "PENUH";
  } else if (!lowAir && !highAir) {
    statusAir = "SETENGAH";
  } else {
    statusAir = "KOSONG";
  }
  Blynk.virtualWrite(V5, statusAir);

  // Status pompa
  String statusPompa;

  // Mode AUTO
  if (modeManual == 0) {
    if (lowAir && !highAir) {
      digitalWrite(RELAY_PIN, HIGH);
      Blynk.virtualWrite(V4, "ON");
      Blynk.logEvent("pompa_nyala", "Pompa Air Menyala Secara Otomatis!");
      statusPompa = "ON";
    } else {
      digitalWrite(RELAY_PIN, LOW);
      Blynk.virtualWrite(V4, "OFF");
      statusPompa = "OFF";
    }
  }

  // Mode MANUAL
  else {
    if (manualState == 1) {
      digitalWrite(RELAY_PIN, HIGH);
      Blynk.virtualWrite(V4, "ON");
      statusPompa = "ON";
    } else {
      digitalWrite(RELAY_PIN, LOW);
      Blynk.virtualWrite(V4, "OFF");
      statusPompa = "OFF";
    }
  }

  // Tampilkan ke LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode:");
  lcd.print(modeManual == 0 ? "AUTO" : "MANUAL");

  lcd.setCursor(0, 1);
  lcd.print("Pompa:");
  lcd.print(statusPompa);
  lcd.print(" ");
  lcd.print(statusAir);

  delay(1000);
}
