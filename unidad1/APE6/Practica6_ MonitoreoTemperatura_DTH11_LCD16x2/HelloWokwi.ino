#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// ===============================
// SENSOR DHT22
// ===============================
#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// ===============================
// LCD I2C
// ===============================
LiquidCrystal_I2C lcd(0x27, 20, 4);

// ===============================
// PINES DE LEDs
// ===============================
const int LED_AZUL  = 3;
const int LED_VERDE = 4;
const int LED_ROJO  = 5;

// ===============================
// VARIABLES 
// ===============================
float tempMin = 100.0;
float tempMax = -100.0;

// Control de tiempo con millis()
unsigned long tiempoAnterior = 0;
const long intervaloLectura = 3000; // 3 segundos

// ===============================
// SETUP
// ===============================
void setup() {

  // Configurar LEDs
  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);

  // Inicializar sensor
  dht.begin();

  // Inicializar LCD
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("SISTEMA MONITOREO");

  lcd.setCursor(0, 1);
  lcd.print("Temp. Act:");

  lcd.setCursor(0, 2);
  lcd.print("Min:      Max:");

  lcd.setCursor(0, 3);
  lcd.print("Estado:");
}

void loop() {

  unsigned long tiempoActual = millis();

  // Verificar intervalo sin bloquear el sistema
  if (tiempoActual - tiempoAnterior >= intervaloLectura) {

    tiempoAnterior = tiempoActual;

    // Leer temperatura del sensor
    float temp = dht.readTemperature();

    // Validar lectura correcta
    if (isnan(temp)) {
      return;
    }

    // Actualizar valores históricos
    actualizarMinMax(temp);

    // Determinar estado y LEDs
    String estado = obtenerEstado(temp);

    // Mostrar información en LCD
    actualizarLCD(temp, estado);
  }
}

// ===============================
// ACTUALIZAR TEMPERATURA MÍNIMA Y MÁXIMA
// ===============================
void actualizarMinMax(float temp) {

  if (temp < tempMin) {
    tempMin = temp;
  }

  if (temp > tempMax) {
    tempMax = temp;
  }
}

// ===============================
// EVALUAR TEMPERATURA Y CONTROLAR LEDs
// ===============================
String obtenerEstado(float temp) {

  // Apagar todos los LEDs
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO, LOW);

  // Evaluar rangos de temperatura
  if (temp < 20.0) {

    digitalWrite(LED_AZUL, HIGH);
    return "Frio";

  } else if (temp >= 20.0 && temp <= 30.0) {

    digitalWrite(LED_VERDE, HIGH);
    return "Normal";

  } else {

    digitalWrite(LED_ROJO, HIGH);
    return "Caliente";
  }
}

void actualizarLCD(float temp, String estado) {

  // ----- Temperatura Actual -----
  lcd.setCursor(12, 1);
  lcd.print("       ");

  lcd.setCursor(12, 1);
  lcd.print(temp, 1);
  lcd.print((char)223);
  lcd.print("C");

  // ----- Temperatura Mínima -----
  lcd.setCursor(4, 2);
  lcd.print("     ");

  lcd.setCursor(4, 2);
  lcd.print(tempMin, 1);

  // ----- Temperatura Máxima -----
  lcd.setCursor(14, 2);
  lcd.print("     ");

  lcd.setCursor(14, 2);
  lcd.print(tempMax, 1);

  // ----- Estado -----
  lcd.setCursor(8, 3);
  lcd.print("          ");

  lcd.setCursor(8, 3);
  lcd.print(estado);
}