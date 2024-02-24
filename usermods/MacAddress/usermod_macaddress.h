#pragma once

#include "wled.h"
#include <WiFi.h>

namespace
{
  bool parseMac(const char *mac, uint8_t bytes[6])
  {
    return (6 == sscanf(mac, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
                        &bytes[0], &bytes[1], &bytes[2],
                        &bytes[3], &bytes[4], &bytes[5]));
  }
}

class MacAddressMod : public Usermod
{
  String _macStr;
  static constexpr auto CONFIG_KEY_MAC_ADDRESS = "Mac Address";

public:
  MacAddressMod() : Usermod("Mac Address", true) {} // WLEDM

  void setup()
  {
    initDone = true;
  }

  void loop()
  {
  }

  void connected()
  {
    // if (_macStr.isEmpty() || !trySetWifiMac(_macStr.c_str())) {
    //   _macStr = WiFi.macAddress();
    // }
  }

  bool trySetWifiMac(const char *mac)
  {

    Serial.printf("Setting mac to %s\n", mac);

    uint8_t macBytes[6];
    if (!parseMac(mac, macBytes))
    {
      Serial.println("Failed to parse mac");
      return false;
    }

    Serial.printf("Parsed mac bytes: %2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx\n", macBytes[0], macBytes[1], macBytes[2], macBytes[3], macBytes[4], macBytes[5]);
    esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, macBytes);

    if (err != ESP_OK)
    {
      Serial.printf("Failed to change mac: Error %d\n", err);
      return false;
    }

    Serial.println("Successfully changed mac");

    return true;
  }

  void addToConfig(JsonObject &root)
  {
    Usermod::addToConfig(root);
    JsonObject top = root[FPSTR(_name)];

    top[CONFIG_KEY_MAC_ADDRESS] = _macStr;

    Serial.printf("Writing mac to config: %s\n", _macStr.c_str());
  }

  bool readFromConfig(JsonObject &root)
  {
    bool configComplete = Usermod::readFromConfig(root);
    JsonObject top = root[FPSTR(_name)];

    String macStr;
    configComplete &= getJsonValue(top[CONFIG_KEY_MAC_ADDRESS], macStr);

    Serial.printf("Read mac from config: %s\n", macStr.c_str());

    uint8_t macBytes[6];
    if (parseMac(macStr.c_str(), macBytes))
    {
      _macStr = macStr;
      //WiFi.mode(WIFI_STA);
      trySetWifiMac(_macStr.c_str());
    }
    else
    {
      Serial.println("failed to parse mac");

      top[CONFIG_KEY_MAC_ADDRESS] = _macStr = WiFi.macAddress();
      Serial.printf("Resetting mac to %s\n", _macStr.c_str());
    }

    return configComplete;
  }
};