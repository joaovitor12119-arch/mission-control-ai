#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ===== DHT22 =====
#define DHTPIN 2
#define DHTTYPE DHT22

// ===== PINOS =====
#define LDR_PIN A0
#define BUTTON_PIN 3

#define LED_GREEN 8
#define LED_YELLOW 9
#define LED_RED 10

#define BUZZER_PIN 11

// ===== OBJETOS =====
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

  Serial.begin(9600);

  // Inicia sensores
  dht.begin();

  // LCD
  lcd.init();
  lcd.backlight();

  // Botão usando resistor interno do Arduino
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // LEDs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  // Tela inicial
  lcd.setCursor(0, 0);
  lcd.print("SpaceSafe");
  lcd.setCursor(0, 1);
  lcd.print("Inicializando");

  delay(2000);
  lcd.clear();
}

void loop() {

  // =====================
  // LEITURA TEMPERATURA
  // =====================
  float temperatura = dht.readTemperature();

  // Corrige erro de leitura
  if (isnan(temperatura)) {
    temperatura = 0;
  }

  // =====================
  // LEITURA LUZ
  // =====================
  int luzBruta = analogRead(LDR_PIN);

  int luminosidade = map(luzBruta, 0, 1023, 100, 0);

  // =====================
  // BOTÃO = VIBRAÇÃO
  // =====================
  bool vibracao = digitalRead(BUTTON_PIN) == LOW;

  // =====================
  // RESET DOS ALERTAS
  // =====================
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  noTone(BUZZER_PIN);

  String status = "NORMAL";

  bool alerta = false;
  bool critico = false;

  // =====================
  // TEMPERATURA
  // =====================
  if (temperatura > 35) {
    critico = true;
  }
  else if (temperatura >= 29) {
    alerta = true;
  }

  // =====================
  // LUMINOSIDADE
  // =====================
  if (luminosidade < 20) {
    critico = true;
  }
  else if (luminosidade < 50) {
    alerta = true;
  }

  // =====================
  // VIBRAÇÃO
  // =====================
  if (vibracao) {
    alerta = true;
  }

  // =====================
  // STATUS DA CAPSULA
  // =====================
  if (critico) {

    status = "CRITICO";

    digitalWrite(LED_RED, HIGH);

    tone(BUZZER_PIN, 1000);
  }
  else if (alerta) {

    status = "ALERTA";

    digitalWrite(LED_YELLOW, HIGH);

    tone(BUZZER_PIN, 700);
    delay(200);
    noTone(BUZZER_PIN);
  }
  else {

    status = "NORMAL";

    digitalWrite(LED_GREEN, HIGH);
  }

  // =====================
  // LCD
  // =====================
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperatura, 1);
  lcd.print("C ");

  lcd.print("L:");
  lcd.print(luminosidade);

  lcd.setCursor(0, 1);
  lcd.print("Status:");
  lcd.print(status);

  // =====================
  // SERIAL MONITOR
  // =====================
  Serial.println("===== CAPSULA ESPACIAL =====");

  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" C");

  Serial.print("Luminosidade: ");
  Serial.println(luminosidade);

  Serial.print("Vibracao: ");
  Serial.println(vibracao ? "SIM" : "NAO");

  Serial.print("Status: ");
  Serial.println(status);

  Serial.println("============================");
  Serial.println();

  delay(500);
}
