#include "common.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define DEFAULT_BLE_NAME "ZJY智能计步器"

// 0000fee7-0000-1000-8000-00805f9b34fb
#define WECHAT_SERVICE_UUID "\xe7\xfe"
#define WECHAT_CHAR_MAC_UUID "\xc9\xfe"
#define WECHAT_CHAR_PEDOMETER_UUID "\xa1\xfe"
#define WECHAT_CHAR_TARGET_UUID "\xa2\xfe"

// 00002254-0000-1000-8000-00805f9b34fb
#define ZJY_SERVICE_UUID "\x54\x22"
#define ZJY_CHAR_BLUE_NAME_UUID "\x01\xff"
#define ZJY_CHAR_ADD_HOW_MANY_ONE_TIME_UUID "\x02\xff"
#define ZJY_CHAR_RANDOM_TARGET_UUID "\x03\xff"

BLEServer *pServer;
BLEAdvertising *pAdvertising;
BLEService *pService;
BLECharacteristic *pChar_mac;
BLECharacteristic *pChar_pedo;
BLECharacteristic *pChar_target;
BLEService *pService2;
BLECharacteristic *pChar_name;
BLECharacteristic *pChar_addpertime;
BLECharacteristic *pChar_targetz;

class PedoCallbackHandler : public BLECharacteristicCallbacks {
	void onRead(BLECharacteristic* pChar_pedo) {
		uint8_t pedo[4] = {0x01, steps << 16 >> 16, steps << 8 >> 16, steps >> 16};
		pChar_pedo->setValue(pedo, 4);
		Serial.print("onRead pedo, current value is ");
		Serial.println(steps);
	}
};

class TargetCallbackHandler : public BLECharacteristicCallbacks {
	void onRead(BLECharacteristic* pChar_target) {
		uint8_t target[4] = {0x01, targets << 16 >> 16, targets << 8 >> 16, targets >> 16};
		pChar_target->setValue(target, 4);
		Serial.print("onRead target, current value is ");
		Serial.println(targets);
	}

	void onWrite(BLECharacteristic* pChar_target) {
		std::string t = pChar_target->getValue();
		targets = t[1] + t[2] * 0x100 + t[3] * 0x10000;
		Serial.print("onWrite target, set to ");
		Serial.println(targets);
		pref->putUInt("targets", targets);
	}
};

class ZJYNameCallbackHandler : public BLECharacteristicCallbacks {
	void onRead(BLECharacteristic* p) {
		String name = pref->getString("name", "ZJY智能计步器");
		p->setValue(name.c_str());
		Serial.print("onRead name, eeprom value is ");
		Serial.println(name);
	}

	void onWrite(BLECharacteristic* p) {
		std::string s = p->getValue();
		String t(s.c_str());
		Serial.print("onWrite name, set to ");
		Serial.println(t);
		pref->putString("name", t);
	}
};

class ZJYAddPerTimeCallbackHandler : public BLECharacteristicCallbacks {
	void onRead(BLECharacteristic* p) {
		char c = pref->getChar("addPerTime", 0);
		uint8_t t[1]; t[0] = c;
		p->setValue(t, 1);
		Serial.print("onRead addPerTime, eeprom value is ");
		Serial.println(c);
	}

	void onWrite(BLECharacteristic* p) {
		std::string s = p->getValue();
		Serial.print("onWrite addPerTime, set to ");
		Serial.println(s[0]);
		pref->putChar("addPerTime", s[0]);
	}
};

class ZJYTargetCallbackHandler : public BLECharacteristicCallbacks {
	void onRead(BLECharacteristic* p) {
		int i = pref->getInt("targets", 29999);
		uint8_t t[4] = {i << 16 >> 16, i << 8 >> 16, i >> 16, 0};
		p->setValue(t, 4);
		Serial.print("onRead targetz, eeprom value is ");
		Serial.println(i);
	}

	void onWrite(BLECharacteristic* p) {
		std::string s = p->getValue();
		int t = s[2] * 0x10000 + s[1] * 0x100 + s[0];
		Serial.print("onWrite targetz, set to ");
		Serial.println(t);
		pref->putInt("targets", t);
	}
};

class ConnectionCallbackHelper : public BLEServerCallbacks {
	void onConnect(BLEServer* pServer) {
		Serial.println("Device connected."); connectedDevices++;
		// dirty patch for multiple client connection
		pServer->startAdvertising();
	}

	void onDisconnect(BLEServer* pServer) {
		Serial.println("Device disconnected."); connectedDevices--;
		pServer->startAdvertising();
	}
};

void init_ble() {
	Serial.println("- Initializing BLE services...");
	// Bluetooth device name
	String name = pref->getString("name", DEFAULT_BLE_NAME);
	Serial.print("set name to ");
	Serial.println(name);
	Serial.println(name.c_str());
	BLEDevice::init(name.c_str());
	Serial.println("set name done");
	pServer = BLEDevice::createServer();
	pServer->setCallbacks(new ConnectionCallbackHelper());
	// 0xfee7: wechat service
	pService = pServer->createService((std::string) WECHAT_SERVICE_UUID, 3);
	// 0xfec9: mac characteristic
	pChar_mac = pService->createCharacteristic(WECHAT_CHAR_MAC_UUID, BLECharacteristic::PROPERTY_READ);
	BLEAddress mac_addr = BLEDevice::getAddress();
	pChar_mac->setValue(*mac_addr.getNative(), 6);
	// 0xfea1: pedometer characteristic
	pChar_pedo = pService->createCharacteristic(WECHAT_CHAR_PEDOMETER_UUID, BLECharacteristic::PROPERTY_READ |/* BLECharacteristic::PROPERTY_INDICATE |*/ BLECharacteristic::PROPERTY_NOTIFY);
	pChar_pedo->addDescriptor(new BLE2902());
	uint8_t pedo[] = {0x01, 0x39, 0x30, 0x00};
	pChar_pedo->setValue(pedo, 4);
	pChar_pedo->setCallbacks(new PedoCallbackHandler());
	// 0xfea2: target characteristic
	pChar_target = pService->createCharacteristic(WECHAT_CHAR_TARGET_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_INDICATE);
	pChar_target->addDescriptor(new BLE2902());
	uint8_t target[] = {0x01, 0x10, 0x27, 0x00};
	pChar_target->setCallbacks(new TargetCallbackHandler());
	Serial.println("set wechat service done");

	// 0x2254: ZJY private service
	pService2 = pServer->createService((std::string) ZJY_SERVICE_UUID, 3);
	// 0xff01: bluetooth name
	pChar_name = pService2->createCharacteristic(ZJY_CHAR_BLUE_NAME_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
	pChar_name->setCallbacks(new ZJYNameCallbackHandler());
	// 0xff02: add how many steps one time
	pChar_addpertime = pService2->createCharacteristic(ZJY_CHAR_ADD_HOW_MANY_ONE_TIME_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
	pChar_addpertime->setCallbacks(new ZJYAddPerTimeCallbackHandler());
	// 0xff03: target
	pChar_targetz = pService2->createCharacteristic(ZJY_CHAR_RANDOM_TARGET_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
	pChar_targetz->setCallbacks(new ZJYTargetCallbackHandler());
	Serial.println("set up ZJY service done");

	// start service
	pService->start();
	pService2->start();
	pAdvertising = pServer->getAdvertising();
	// need to **broadcast** 0xfee7 wechat service uuid
	pAdvertising->addServiceUUID(WECHAT_SERVICE_UUID);
	// set manufacture data, which is needed by wechat
	BLEAdvertisementData adv;
	uint8_t md[9] = {0x22, 0x54};
	memcpy(md + 2, *mac_addr.getNative(), 6);
	md[8] = 0; // string need \x00 at the end
	std::string mds = (char*) md;
	adv.setManufacturerData(mds);
	adv.setAppearance(0x440);
	pAdvertising->setAdvertisementData(adv);
	pAdvertising->start();
	Serial.println("- [OK] BLE services initialized.");
}
