const int PIN_555 = 2;   // Señal del temporizador 555
const int PIN_FF  = 3;   // Señal del flip-flop

char modo = 'p';   // 'p' = Plotter
// informacion de cada señal
struct Canal {
  const char* nombre;        
  int pin;                   
  int estadoAnterior;        
  unsigned long tAnterior;   
  unsigned long tCambio;     
  unsigned long tHigh;       
  unsigned long tLow;
  float frecuencia;
  float periodo;
  float duty;
};

// Se crean dos canales: uno para el 555 y otro para el flip-flop
Canal canal555 = {"555", PIN_555, LOW, 0, 0, 0, 0, 0, 0, 0};
Canal canalFF  = {"FF ", PIN_FF,  LOW, 0, 0, 0, 0, 0, 0, 0};

bool huboCambio(int actual, int anterior);
bool flancoAscendente(int actual, int anterior);
void actualizarDuty(Canal &c, int actual, unsigned long t);
void procesarCanal(Canal &c, unsigned long t);
void mostrarOndas(unsigned long t);
void mostrarMonitor(unsigned long t);

unsigned long ultimaImpresion = 0;   // temporizador ondas
unsigned long ultimoMonitor   = 0;   // temporizador texto

void setup() {
  Serial.begin(9600);        
  pinMode(PIN_555, INPUT);   
  pinMode(PIN_FF,  INPUT);
  Serial.println("Comandos: p = Plotter | m = Monitor");
}

void loop() {
  unsigned long t = millis();   
  procesarCanal(canal555, t);   
  procesarCanal(canalFF,  t);

  // cambio de modo por teclado
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'p' || cmd == 'm') modo = cmd;
  }

  if (modo == 'p') mostrarOndas(t);
  else             mostrarMonitor(t);
}

// FUNCIÓN DE ONDAS (Plotter)
void mostrarOndas(unsigned long t) {
  // ondas cada 10ms
  if (t - ultimaImpresion >= 10) {
    ultimaImpresion = t;
    int plotFF  = digitalRead(PIN_FF)  * 200 + 50;   // 50 o 250
    int plot555 = digitalRead(PIN_555) * 200 + 350;  // 350 o 550
    Serial.print(0);        Serial.print(",");
    Serial.print(plotFF);   Serial.print(",");
    Serial.print(plot555);  Serial.print(",");
    Serial.println(600);
  }
  // texto de analisis cada 1000ms
  if (t - ultimoMonitor >= 1000) {
    ultimoMonitor = t;
    Serial.println("=== CANAL 555 ===");
    Serial.print("F: ");    Serial.print(canal555.frecuencia); Serial.println(" Hz");
    Serial.print("T: ");    Serial.print(canal555.periodo);    Serial.println(" ms");
    Serial.print("Duty: "); Serial.print(canal555.duty);       Serial.println(" %");
    Serial.println("=== CANAL FF ===");
    Serial.print("F: ");    Serial.print(canalFF.frecuencia);  Serial.println(" Hz");
    Serial.print("T: ");    Serial.print(canalFF.periodo);     Serial.println(" ms");
    Serial.print("Duty: "); Serial.print(canalFF.duty);        Serial.println(" %");
    Serial.println("---------------------------");
  }
}

void mostrarMonitor(unsigned long t) {
  if (t - ultimoMonitor >= 1000) {
    ultimoMonitor = t;
    Serial.println("=== CANAL 555 ===");
    Serial.print("F: ");    Serial.print(canal555.frecuencia); Serial.println(" Hz");
    Serial.print("T: ");    Serial.print(canal555.periodo);    Serial.println(" ms");
    Serial.print("Duty: "); Serial.print(canal555.duty);       Serial.println(" %");
    Serial.println("=== CANAL FF ===");
    Serial.print("F: ");    Serial.print(canalFF.frecuencia);  Serial.println(" Hz");
    Serial.print("T: ");    Serial.print(canalFF.periodo);     Serial.println(" ms");
    Serial.print("Duty: "); Serial.print(canalFF.duty);        Serial.println(" %");
    Serial.println("---------------------------");
  }
}

// cambio de estado
bool huboCambio(int actual, int anterior) {
  return actual != anterior;
}

// flanco ascendente (cuando la señal sube)
bool flancoAscendente(int actual, int anterior) {
  return (actual == HIGH && anterior == LOW);
}

// Actualiza tiempos HIGH y LOW
void actualizarDuty(Canal &c, int actual, unsigned long t) {
  if (actual == HIGH)
    c.tLow = t - c.tCambio;
  else
    c.tHigh = t - c.tCambio;
  c.tCambio = t;
}

// analiza la señal completa
void procesarCanal(Canal &c, unsigned long t) {
  int actual = digitalRead(c.pin);  
  if (!huboCambio(actual, c.estadoAnterior)) return; 
  actualizarDuty(c, actual, t);
  if (flancoAscendente(actual, c.estadoAnterior)) {
    if (c.tHigh > 0 && c.tLow > 0) {
      c.periodo    = c.tHigh + c.tLow;
      c.frecuencia = 1000.0 / c.periodo;
      c.duty       = (c.tHigh * 100.0) / c.periodo;
    }
  }
  c.estadoAnterior = actual; 
}