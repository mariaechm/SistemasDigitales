const int PIN_555 = 2;   // Señal del temporizador 555
const int PIN_FF  = 3;   // Señal del flip-flop

// informacion de cada señal
struct Canal {
  const char* nombre;        
  int pin;                   
  int estadoAnterior;        
  unsigned long tAnterior;   
  unsigned long tCambio;     
  unsigned long tHigh;       
  unsigned long tLow;        
};

// Se crean dos canales: uno para el 555 y otro para el flip-flop
Canal canal555 = {"555", PIN_555, LOW, 0, 0, 0, 0};
Canal canalFF  = {"FF ", PIN_FF,  LOW, 0, 0, 0, 0};

bool huboCambio(int actual, int anterior);
bool flancoAscendente(int actual, int anterior);
unsigned long medirPeriodo(Canal &c, unsigned long t);
float calcularFrecuencia(unsigned long periodo);
void actualizarDuty(Canal &c, int actual, unsigned long t);
float calcularDuty(Canal &c);
void procesarCanal(Canal &c, unsigned long t);

// Configuración inicial
void setup() {
  Serial.begin(9600);        // Inicia comunicacion serial
  pinMode(PIN_555, INPUT);   // Pin del 555 como entrada
  pinMode(PIN_FF,  INPUT);   // Pin del FF como entrada
}

void loop() {
  unsigned long t = millis();   
  procesarCanal(canal555, t);   
  procesarCanal(canalFF,  t);   
}

// cambio de estado
bool huboCambio(int actual, int anterior) {
  return actual != anterior;
}

// flanco ascendente (cuando la señal sube)
bool flancoAscendente(int actual, int anterior) {
  return (actual == HIGH && anterior == LOW);
}

// Calcula el periodo entre pulsos
unsigned long medirPeriodo(Canal &c, unsigned long t) {
  unsigned long periodo = t - c.tAnterior;
  c.tAnterior = t;
  return periodo;
}

// Calcula la frecuencia (Hz)
float calcularFrecuencia(unsigned long periodo) {
  if (periodo == 0) return 0;
  return 1000.0 / periodo; // millis → ms
}

// Actualiza tiempos HIGH y LOW (para duty cycle)
void actualizarDuty(Canal &c, int actual, unsigned long t) {
  if (actual == HIGH)
    c.tLow = t - c.tCambio;
  else
    c.tHigh = t - c.tCambio;

  c.tCambio = t;
}

// Calcula el duty cycle (% de tiempo en HIGH)
float calcularDuty(Canal &c) {
  unsigned long total = c.tHigh + c.tLow;
  if (total == 0) return 0;
  return (c.tHigh * 100.0) / total;
}

// analiza la señal completa
void procesarCanal(Canal &c, unsigned long t) {
  int actual = digitalRead(c.pin);  

  if (!huboCambio(actual, c.estadoAnterior)) return; 

  actualizarDuty(c, actual, t); 

  // En cada flanco ascendente calcula todo
  if (flancoAscendente(actual, c.estadoAnterior)) {
    unsigned long periodo = medirPeriodo(c, t);
    float frecuencia = calcularFrecuencia(periodo);
    float duty = calcularDuty(c);

    // resultados en el monitor serial
    Serial.print(c.nombre);
    Serial.print(" -> F: ");
    Serial.print(frecuencia);
    Serial.print(" Hz | T: ");
    Serial.print(periodo);
    Serial.print(" ms | Duty: ");
    Serial.print(duty);
    Serial.println(" %");
  }

  c.estadoAnterior = actual; 
}        