int LED_PINS[] = {2, 3, 4, 5, 6, 7};
int BOTON_PIN = 8;
int patronActual = 0;
int estadoBoton = 0;
int ultimoEstadoBoton = 0;

unsigned long tiempoAnterior = 0;
int pasoActual = 0;       // paso dentro del patron
bool faseLED = false;      // para parpadeo: ON & OFF

void setup() {
  for (int i = 0; i < 6; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  pinMode(BOTON_PIN, INPUT);
  randomSeed(analogRead(0));
}

  void loop() {
  leerBoton();
  ejecutarPatron();
}

  void ejecutarPatron() {
  switch (patronActual) {
    case 0: patronSecuencia();   break;
    case 1: patronPersecucion(); break;
    case 2: patronParpadeo();    break;
    case 3: patronAleatorio();   break;
    case 4: patronOnda();        break;  }
}

void leerBoton() {
  estadoBoton = digitalRead(BOTON_PIN);

  if (estadoBoton == HIGH && ultimoEstadoBoton == LOW) {
    apagarTodos();
    pasoActual = 0;
    faseLED = false;
    tiempoAnterior = millis();
    patronActual = random(0, 5);  // aleatoriamente cambia el patron
  }

  ultimoEstadoBoton = estadoBoton;
}

void apagarTodos() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
}

void patronSecuencia() { // Enciende un LED --> espera - lo apaga y pasa al siguiente
  unsigned long intervalo = 150;
  if (millis() - tiempoAnterior >= intervalo) {
    tiempoAnterior = millis();

    if (!faseLED) {
      apagarTodos();
      digitalWrite(LED_PINS[pasoActual], HIGH);
      faseLED = true;
    } else {
      digitalWrite(LED_PINS[pasoActual], LOW);
      faseLED = false;
      pasoActual = (pasoActual + 1) % 6;
    }
  }
}

void patronPersecucion() { // Un LED corre dejando rastro que se apaga
  unsigned long intervalo = 100;
  if (millis() - tiempoAnterior >= intervalo) {
    tiempoAnterior = millis();

    apagarTodos();
    digitalWrite(LED_PINS[pasoActual], HIGH);
    pasoActual = (pasoActual + 1) % 6;
  }
}

void patronParpadeo() {
  unsigned long intervalo = 300;
  if (millis() - tiempoAnterior >= intervalo) {
    tiempoAnterior = millis();
    faseLED = !faseLED;

    for (int i = 0; i < 6; i++) {
      digitalWrite(LED_PINS[i], faseLED ? HIGH : LOW);
    }
  }
}

void patronAleatorio() { // Enciende y apaga un LED aleatorio
  unsigned long intervalo = 100;
  if (millis() - tiempoAnterior >= intervalo) {
    tiempoAnterior = millis();

    apagarTodos();
    int led = random(0, 6);
    digitalWrite(LED_PINS[led], HIGH);
  }
}

void patronOnda() { // Ida y vuelta 
  unsigned long intervalo = 80;
  if (millis() - tiempoAnterior >= intervalo) {
    tiempoAnterior = millis();

    apagarTodos();
    int idx;
    if (pasoActual < 6) {
      idx = pasoActual;
    } else {
      idx = 10 - pasoActual;
    }
    digitalWrite(LED_PINS[idx], HIGH);
    pasoActual = (pasoActual + 1) % 10;
  }
}