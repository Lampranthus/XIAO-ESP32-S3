#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Configuración OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// UUID del servicio y característica BLE (puedes cambiarlos)
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;

bool deviceConnected = false;

// Callback para eventos de conexión BLE
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Dispositivo conectado!");
    }

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Dispositivo desconectado");
      // Reiniciar advertising para permitir una nueva conexión
      pServer->getAdvertising()->start();
    }
};

void setup() {
  Serial.begin(115200);

  // Inicializar OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error en OLED");
    while(1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Iniciando BLE...");
  display.display();

  // Configurar BLE
  BLEDevice::init("Xiao-ESP32-S3"); // Nombre del dispositivo BLE
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setValue("Hola desde Xiao!");
  pService->start();

  // Iniciar advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Parámetros para mejorar la conexión
  BLEDevice::startAdvertising();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("BLE activo");
  display.println("Nombre: Xiao-ESP32-S3");
  display.println("Esperando conexión...");
  display.display();
}

void loop() {
  if (deviceConnected) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("BLE Conectado!");
    display.println("Usa una app BLE como");
    display.println("'nRF Connect' para");
    display.println("comunicarte.");
    display.display();
  }
  delay(1000);
}