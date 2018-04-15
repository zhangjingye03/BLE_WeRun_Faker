#include <Arduino.h>
#include <WiFi.h>

WiFiClass& init_wifi() {
	Serial.println("- Turning off WiFi...");
	WiFi.mode(WIFI_OFF);
	//WiFi.disconnect();
	//delay(100);
	Serial.println("- [OK] WiFi down.");
	return WiFi;
}
