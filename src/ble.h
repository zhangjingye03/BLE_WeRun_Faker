#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

extern void init_ble();
extern void destroy_ble();
extern BLECharacteristic *pChar_pedo;
extern BLECharacteristic *pChar_target;
extern BLEServer *pServer;
extern BLEAdvertising *pAdvertising;
