#pragma once

#include "wled.h"

volatile bool motionBottomDetected = false; // indicates whether motionBottom interrupt pin has gone high
static void IRAM_ATTR motionBottom() {
  motionBottomDetected = true;
}

#define triggerPinBottom  25
#define nrOfSteps         15
#define ledsPerStep       15

// This is an v2 usermod which uses interrupts to light the stairs.

class StairsUsermod : public Usermod {
  private:
    bool usermodActive = true; // Enable this usermod

  void publishMqtt(const char* state, bool retain = false); // example for publishing MQTT message

  public:
    void setup() {
      pinMode(triggerPinBottom, INPUT_PULLDOWN);
      attachInterrupt(digitalPinToInterrupt(triggerPinBottom), motionBottom, RISING);
    }

  void loop() {
    if (!usermodActive || strip.isUpdating()) return;
    if (motionBottomDetected) {
      publishMqtt("Yo Bottom Movement!");
      motionBottomDetected = false;
    }
  }

  void handleOverlayDraw()
    {
      // int maskSizeLeds = 225;
      // // check if usermod is active
      // if (usermodActive == true)
      // {
      //   // loop over all leds
      //   for (int x = 0; x < maskSizeLeds; x++)
      //   {
      //     // check mask
      //     if (maskLedsOn[x] == 0)
      //     {
      //       // set pixel off
      //       strip.setPixelColor(x + ledOffset, RGBW32(0,0,0,0));
      //     }
      //   }
      // }
    }

    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId()
    {
      return USERMOD_ID_STAIRS;
    }

};

void StairsUsermod::publishMqtt(const char* state, bool retain)
{
#ifndef WLED_DISABLE_MQTT
  //Check if MQTT Connected, otherwise it will crash the 8266x
  if (WLED_MQTT_CONNECTED) {
    char subuf[64];
    strcpy(subuf, mqttDeviceTopic);
    strcat_P(subuf, PSTR("/bottom"));
    mqtt->publish(subuf, 0, retain, state);
  }
#endif
}