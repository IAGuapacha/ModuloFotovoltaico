/********* PINES ****************/

// Cruce por Cero
// int pinCruce = 0; // OutputCruce
const int pinInputCruce = 2;
//int pinCheck = 9;



// DHT22
#define DHTPIN 4 // Pin donde está conectado el sensor

// Bluetooth
const int BTRX = 5;
const int BTTX = 6;

// LCD Pines
const int pinVo = 3;
const int pinRS = 7;
const int pinEN = 8;
const int pinD4 = 9;
const int pinD5 = 10;
const int pinD6 = 11;
const int pinD7 = 12;

// GYML8511
int UVOUT = A3; //Output from the sensor
int REF_3V3 = A2; //3.3V power on the Arduino board

/******* LIBRERIAS **************/
// LCD
#include <LiquidCrystal.h>

// DHT
#include "DHT.h"

// Bluetooth
#include <SoftwareSerial.h>

// TSL2591
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
/************* LCD *************/
LiquidCrystal lcd(pinRS, pinEN, pinD4, pinD5, pinD6, pinD7); //    ( RS, EN, d4, d5, d6, d7)

/************* Sensor DHT  *************/
#define DHTTYPE DHT22   // Tipo de sensor a usar
//#define DHTTYPE DHT11   // Descomentar si se usa el DHT 11

DHT dht(DHTPIN, DHTTYPE);

/************* Sensor de voltaje *************/
// Para mayor precision medir y remplazar los valores reales de R1 y R2
double R1 = 10000;
double R2 = 100000;


/************ Cruce por cero  ****************/
volatile double tiempo1 = 0.0;
volatile double tiempo2 = 0.0;


/*********** BLUETOOTH *********/
SoftwareSerial BT(BTRX, BTTX); // RX, TX

/*********** TSL2591 ************/
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);


/************ Medidas Sensores **************/
// TSL
float uvIntensity = 0.00;
float lux = 0.00;
double visible = 0.00;
uint16_t ir = 0, full = 0; // Luz ir y full espectro
// Sens corriente
double corriente = 0.00;
// DHT22
double t = 0.00;  // Temperatura
double h = 0.00;  // Humedad
// Potencia
double I = 0.0; // Corriente
double V = 0.0; // Voltaje
double P = 0.0; // Potencia
// Medir frecuencia
volatile double periodo = 0.0;
volatile double frecuencia = 0.0;



double count = 0.00;
int tiempoEspera = 2000; // Tiempo de espera entre medidas, en milisegundos



void setup() {
  Serial.begin(9600);
  /************* Config LCD *************/
  lcd.begin(16, 2); // Fijar el numero de columnas y de filas de la LCD
  lcd.clear();
  pinMode(pinVo, OUTPUT);
  TCCR2B = 0;
  TCCR2B |= (1 << CS20) || (0 << CS21) || (0 << CS22);
  analogWrite(pinVo, 80);
  /************* Config DHT *************/
  dht.begin();
  pinMode(DHTPIN, INPUT_PULLUP);

  /*************** Config Cruce por cero ********************/
  //pinMode(pinCruce, OUTPUT); // Genera pulsos por cada cruce, solo simulacion
  pinMode(pinInputCruce, INPUT); // Entrada Cruce por cero
  attachInterrupt(0, cruceCero, RISING); // Interrupción 0, pin 2.
  EIMSK = (0 << INT0);

  /************ BLUETOOTH **************/
  BT.begin(9600);

  /************ TSL2591 **************/
  tsl.begin();
  configureSensor();

  /************ GYML8511 **************/
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
}


void loop() {
  
    count++;
    String data = d2s(random(-5,5))+","+d2s(random(-5,5))+","+d2s(random(-5,5))+","+d2s(random(-5,5))+","+d2s(random(-5,5))+","+d2s(random(-5,5))+","+d2s(random(-5,5));
    enviarDatoBT(data,0);
    delay(1000);
  

/*t
  enviarDatoBT("\n", 0);
  medirUV();
  delay(tiempoEspera);

  medirLuzIR();
  delay(tiempoEspera);

  medirLuzVisible();
  delay(tiempoEspera);

  medirLux();
  delay(tiempoEspera);

  medirTemperatura();
  delay(tiempoEspera);

  medirHumedad();
  delay(tiempoEspera);

    medirCorriente();
    delay(tiempoEspera);

    medirVoltaje();
    delay(tiempoEspera);

    medirPotencia();
    delay(tiempoEspera);

    medirFrecuencia();
    delay(tiempoEspera);
  */
}

void enviarData() {
  /*
    // TSL
    float uvIntensity = 0.00;
    float lux = 0.00;
    double visible = 0.00;
    uint16_t ir = 0, full = 0; // Luz ir y full espectro
    // Sens corriente
    double corriente = 0.00;
    // DHT22
    double t = 0.00;  // Temperatura
    double h = 0.00;  // Humedad
    // Potencia
    double I = 0.0; // Corriente
    double V = 0.0; // Voltaje
    double P = 0.0; // Potencia
    // Medir frecuencia
    volatile double periodo = 0.0;
    volatile double frecuencia = 0.0; */

  medirUV();
  medirLuzIR();
  medirLuzVisible();
  medirLux();
  medirTemperatura();
  medirHumedad();
  
  String data = d2s(t) + "," + d2s(h) + "," + d2s(uvIntensity) + "," + d2s(ir) + "," + d2s(V) + "," + d2s(I) + "," + d2s(P);
  BT.println(data);
}


String d2s(double n) {
  int ndec = 2;
  String r = "";

  int v = n;
  r += v;     // whole number part
  r += '.';   // decimal point
  int i;
  for (i = 0; i < ndec; i++) {
    // iterate through each decimal digit for 0..ndec
    n -= v;
    n *= 10;
    v = n;
    r += v;
  }

  return r;
}

void medirUV() {
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
  float outputVoltage = 3.3 / refLevel * uvLevel;
  uvIntensity = mapfloat(outputVoltage, 0.99, 2.9, 0.0, 15.0);
  imprimirMedida("RadiacionUV", uvIntensity, "mW/cm^2");
}

int averageAnalogRead(int pinToRead) {
  byte numberOfReadings = 8;
  unsigned int runningValue = 0;

  for (int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return (runningValue);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void medirLux() {
  uint32_t lum = tsl.getFullLuminosity();
  ir = lum >> 16;
  full = lum & 0xFFFF;
  lux = tsl.calculateLux(full, ir);
  imprimirMedida("Iluminacion", lux, "Lux");
}

void medirLuzVisible() {
  uint32_t lum = tsl.getFullLuminosity();
  ir = lum >> 16;
  full = lum & 0xFFFF;
  visible = (double)(full - ir);
  imprimirMedida("LuzVisible", visible, " ");
}

void medirLuzIR() {
  uint32_t lum = tsl.getFullLuminosity();
  ir = lum >> 16;
  full = lum & 0xFFFF;
  imprimirMedida("Luz IR", (double)ir, " ");
}

void configureSensor(void) {
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  //tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  tsl2591Gain_t gain = tsl.getGain();
}

void medirCorriente() {
  double sensibilidad = 0.066;  // Sensibilidad del sensor ACS712
  corriente = ( (analogRead(A0) * 5.0 / 1023) - 2.5) / sensibilidad;
  I = corriente;
  imprimirMedida("Corriente", corriente, "A");
}

void medirVoltaje() {
  double Vo = (analogRead(A1) * 5.0 / 1023);  // Voltaje a la salida del divisor
  double Vi = ((R1 + R2) / R1) * Vo; // Divisor de voltaje para conocer el voltaje de entrada
  V = Vi;
  imprimirMedida("Voltaje", Vi, "V");
}

void medirPotencia() {
  P = V * I;
  imprimirMedida("Potencia", P, "Watts");
}

void medirTemperatura() {
  t = dht.readTemperature(); // Lee temperatura en grados Celsius
  //float t = dht.readTemperature(true); //Descomentar para leer la temperatura en grados Fahrenheit
  imprimirMedida("Temperatura", t, "Celsius");
}

void medirHumedad() {
  h = dht.readHumidity(); // Lee temperatura en grados Celsius
  imprimirMedida("Humedad", h, "%RH");
}

void enviarDatoBT(String ID, double dato) {
  BT.println(ID);
  // BT.print(" ");
  //BT.println(dato);
}



void medirFrecuencia() {
  periodo = 0;
  frecuencia = 0;
  tiempo1 = 0;
  tiempo2 = 0;
  EIMSK = (1 << INT0);
  while (frecuencia == 0);
  EIMSK = (0 << INT0);
  imprimirMedida("Frecuencia", frecuencia, "Hz");
}

void imprimirMedida(String medida, double valor, String unidad) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(medida);
  lcd.setCursor(0, 1);
  lcd.print(valor);
  lcd.print(" ");
  lcd.print(unidad);
  Serial.print(medida);
  Serial.print(" ");
  Serial.print(valor);
  Serial.print(" ");
  Serial.println(unidad);
  enviarDatoBT(medida, valor);
}

void cruceCero() {
  //digitalWrite(pinCruce, HIGH);
  // digitalWrite(pinCruce, LOW);
  tiempo2 = tiempo1 == 0 ? tiempo2 : millis();
  if (tiempo1 && tiempo2 != 0) {
    periodo = (tiempo2 - tiempo1) * 2;
    frecuencia = 1000 / periodo;
    tiempo1 = 0;
    tiempo2 = 0;
  }
  tiempo1 = tiempo2 == 0 ? millis() : tiempo1;
}
