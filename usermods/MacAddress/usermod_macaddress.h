#pragma once

#include "wled.h"
#include <Wifi.h>

class MacAddressMod : public Usermod
{
public:
  MacAddressMod() : Usermod("Mac Address", true) {} // WLEDM

  void setup()
  {
    initDone = true;
  }

  void loop()
  {
  }

  void addToConfig(JsonObject &root)
  {
    Usermod::addToConfig(root);
    JsonObject top = root[FPSTR(_name)];

    top["Mac Address"] = WiFi.macAddress();
  }
};