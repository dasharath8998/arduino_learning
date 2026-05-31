#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-cba987654321"

BLECharacteristic* pCharacteristic;
bool deviceConnected = false;

// =====================================
// Send Data To Flutter
// =====================================
void sendToFlutter(String message) {

  if (!deviceConnected) {
    Serial.println("⚠️ No Flutter device connected");
    return;
  }

  Serial.print("📤 ESP32 Sending: ");
  Serial.println(message);

  pCharacteristic->setValue(message);
  pCharacteristic->notify();
}

// =====================================
// Connection Callbacks
// =====================================
class MyServerCallbacks : public BLEServerCallbacks {

  void onConnect(BLEServer* pServer) {

    deviceConnected = true;

    Serial.println();
    Serial.println("================================");
    Serial.println("✅ Flutter Connected");
    Serial.println("================================");
  }

  void onDisconnect(BLEServer* pServer) {

    deviceConnected = false;

    Serial.println();
    Serial.println("================================");
    Serial.println("❌ Flutter Disconnected");
    Serial.println("================================");

    BLEDevice::startAdvertising();
  }
};

// =====================================
// Receive Data From Flutter
// =====================================
class MyCallbacks : public BLECharacteristicCallbacks {

  void onWrite(BLECharacteristic* pCharacteristic) {

    String received =
      String(pCharacteristic->getValue());

    Serial.println();
    Serial.print("📥 ESP32 Received: ");
    Serial.println(received);

    String response =
      "ESP32 Received -> " + received;

    sendToFlutter(response);
  }
};

// =====================================
// Setup
// =====================================
void setup() {

  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("================================");
  Serial.println("ESP32 BLE SERVER");
  Serial.println("================================");

  BLEDevice::init("MyESP32");

  Serial.print("BLE MAC: ");
  Serial.println(
    BLEDevice::getAddress()
      .toString()
      .c_str()
  );

  BLEServer* pServer =
      BLEDevice::createServer();

  pServer->setCallbacks(
      new MyServerCallbacks());

  BLEService* pService =
      pServer->createService(
          SERVICE_UUID);

  pCharacteristic =
      pService->createCharacteristic(
          CHARACTERISTIC_UUID,
          BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY
      );

  pCharacteristic->addDescriptor(
      new BLE2902());

  pCharacteristic->setCallbacks(
      new MyCallbacks());

  pCharacteristic->setValue(
      "ESP32 Ready");

  pService->start();

  BLEAdvertising* pAdvertising =
      BLEDevice::getAdvertising();

  pAdvertising->setScanResponse(true);

  pAdvertising->start();

  Serial.println();
  Serial.println("🚀 BLE Advertising Started");
  Serial.println("Device Name: MyESP32");
  Serial.println();
}

// =====================================
// Loop
// =====================================
unsigned long lastSendTime = 0;

void loop() {

  if (deviceConnected &&
      millis() - lastSendTime > 5000) {

    lastSendTime = millis();

    String msg =
      "Heartbeat: " + String(millis());

    sendToFlutter(msg);
  }

  delay(100);
}