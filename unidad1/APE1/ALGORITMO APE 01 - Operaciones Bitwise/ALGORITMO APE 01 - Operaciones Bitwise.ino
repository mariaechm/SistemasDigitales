#define LED_PIN 13

byte estadoLed = 0;
int contador = 0;
//declaracion de variales de varios tipos
long numeroGrande = 854758;
float decimal = 5.23;

bool encendido = false;

void setup() {
  pinMode(LED_PIN, OUTPUT); //CONFIGURAR LED_PIN como SALIDA
  Serial.begin(9600);

  // operaciones bitwise
  Serial.println("Operaciones Bitwise:");

  Serial.print("AND: ");
  Serial.println(5 & 3, BIN); // 101 & 011 = 001

  Serial.print("OR: ");
  Serial.println(5 | 3, BIN); // 111

  Serial.print("XOR: ");
  Serial.println(5 ^ 3, BIN); // 110

  Serial.print("NOT: ");
  Serial.println(~5, BIN);

  Serial.print("SHIFT LEFT: ");
  Serial.println(1 << 2, BIN); // 100

  Serial.print("SHIFT RIGHT: ");
  Serial.println(4 >> 1, BIN); // 010

  estadoLed = estadoLed | (1 << 0);
}

void loop() {

  // Cambiar estado con XOR
  estadoLed = estadoLed ^ 0b00000001;

  // Verificar estado
  if ((estadoLed & 1) == 1) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(500);

  contador = (contador + 1) % 8;

  //Serial.print("contador: ");
  Serial.println(1 << contador, BIN);

  delay(500);
}