#pragma once

#include "wled.h"

volatile bool motionBottomDetected = false; // indicates whether motionBottom interrupt pin has gone high
static void IRAM_ATTR motionBottom() {
  motionBottomDetected = true;
}

#define triggerPinBottom  25
#define nrOfSteps         15
#define nrOfLeds         225

// This is an v2 usermod which uses interrupts to light the stairs.

class StairsUsermod : public Usermod {
  private:
    int delayTime = 100 ; //millis
    int onTime = 20 ; //seconds

    bool lightingCeremonyActive = false;
    bool stairsOn = false;
    bool usermodActive = true; // Enable this usermod
    unsigned long lastTime = 0;
    unsigned long offTime = 0;
    int curLed = 0;
    int lightingSteps = nrOfSteps + (nrOfLeds/nrOfSteps)/2 ; // nrOfSteps + half stepwidth
    int maskLedsOn[nrOfLeds] = // overall mask to define which LEDs are on
    {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };

  void publishMqtt(const char* state, bool retain = false); // example for publishing MQTT message

  public:
    void setup() {
      pinMode(triggerPinBottom, INPUT_PULLDOWN);
      attachInterrupt(digitalPinToInterrupt(triggerPinBottom), motionBottom, RISING);
    }

  void updateMask() {
    for (int x = 0; x < 8; x++) { maskLedsOn[7+x]=x+1; maskLedsOn[7-x]=x+1;}
    for (int i = 0; i < 14; i++)
      for (int j = 0; j < 15; j++)
         maskLedsOn[15+i*15+j]=maskLedsOn[i*15+j]+1;
    for (int x = 0; x < nrOfLeds; x++)
      if (maskLedsOn[x]>curLed) maskLedsOn[x] = 0;
  }

  void loop() {
    if (!usermodActive || strip.isUpdating()) return;

    if (motionBottomDetected) {
      publishMqtt("Yo Bottom Movement!");
      motionBottomDetected = false;
      if (!lightingCeremonyActive && !stairsOn) {
          curLed = 0;
          lightingCeremonyActive = true;
          stairsOn = true;
      }
      offTime = millis() + onTime*1000;
    }

    if (lightingCeremonyActive == true){
      if ((millis() - lastTime) > delayTime) {
        lastTime = millis();
        if (curLed<lightingSteps)curLed++; // move pointer so led wil be lit
        updateMask();
        strip.trigger();
        if (curLed>=lightingSteps) {
          lightingCeremonyActive = false;
          publishMqtt("curled on max");
        }
      }
    }

    if (offTime < millis()){
      curLed=0;
      updateMask();
      stairsOn=false;
      publishMqtt("stairs off");
    }
  }

void handleOverlayDraw()
    {
      // check if usermod is active
      if (usermodActive == true)
      {
        // loop over all leds
        for (int x = 0; x < nrOfLeds; x++)
        {
          // check mask
          if (maskLedsOn[x] == 0)
          {
            // set pixel off
            //strip.setPixelColor(x + ledOffset, RGBW32(0,0,0,0));
            strip.setPixelColor(nrOfLeds-1-x , RGBW32(0,0,0,0));
          }
        }
      }
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
    strcat_P(subuf, PSTR("/stairs"));
    mqtt->publish(subuf, 0, retain, state);
  }
#endif
}