#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Configuración OLED (I2C)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pines de los botones (pull-up interno)
const int BOTON_1 = 7;   // GPIO7
const int BOTON_2 = 8;   // GPIO8
const int BATERIA_PIN = 9; // GPIO9 (ADC1_CH8)

// Parámetros del divisor de voltaje (ajusta si usas otra relación)
const float DIVISOR_VOLTAJE = 2.0; // Divisor a la mitad (R1 = R2)
const float VOLTAJE_MAXIMO = 4.2;  // Voltaje máximo de la batería (LiPo 3.7V)

void setup() {
  Serial.begin(115200);
  
  // Configurar pines
  pinMode(BOTON_1, INPUT_PULLUP);
  pinMode(BOTON_2, INPUT_PULLUP);
  pinMode(BATERIA_PIN, INPUT);
  
  // Inicializar OLED (dirección I2C 0x3C o 0x3D)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error en OLED");
    while(1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Sistema listo");
  display.display();
  delay(1000);
}

void loop() {
  // Leer botones
  bool estadoBoton1 = digitalRead(BOTON_1);
  bool estadoBoton2 = digitalRead(BOTON_2);
  
  // Leer voltaje de la batería (promedio de 10 lecturas para mayor precisión)
  float voltajeRaw = 0;
  for (int i = 0; i < 10; i++) {
    voltajeRaw += analogRead(BATERIA_PIN);
    delay(1);
  }
  voltajeRaw /= 10.0;
  
  // Convertir a voltaje real (ajusta según tu ESP32-S3)
  float voltajeBateria = (voltajeRaw * 3.3 / 4095.0) * DIVISOR_VOLTAJE;
  
  // Mostrar en OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Estado del sistema:");
  display.println("------------------");
  
  // Botones
  display.print("Boton 1: ");
  display.println(estadoBoton1 == LOW ? "ON" : "OFF");
  
  display.print("Boton 2: ");
  display.println(estadoBoton2 == LOW ? "ON" : "OFF");
  
  // Batería
  display.print("Bateria: ");
  display.print(voltajeBateria, 2); // 2 decimales
  display.println("V");
  
  // Porcentaje aproximado (ajusta para tu batería)
  float porcentaje = (voltajeBateria / VOLTAJE_MAXIMO) * 100;
  if (porcentaje > 100) porcentaje = 100;
  display.print("Carga: ");
  display.print(porcentaje, 0); // 0 decimales
  display.println("%");
  
  display.display();
  delay(100); // Actualización cada 0.5 segundos
}