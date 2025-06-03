#pragma once

#include "wled.h"

/*
 * Usermods allow you to add own functionality to WLED more easily
 * See: https://github.com/Aircoookie/WLED/wiki/Add-own-functionality
 *
 * This usermod can be used to drive a wordclock with a 16x16 pixel matrix with WLED.
 * The clock resolution is in minutes with an extra line creating the seconds. I also
 * managed to fit in the date displayed by the day numer and month abbreviation.
 * The language for the clock in Dutch, in the readme you can find the layout mask for
 * the words.
 * There are 2 parameters to change the behaviour:
 *
 * active: enable/disable usermod
 * diplayItIs: enable/disable display of "Es ist" on the clock.
 */

class WordClockUsermod1616NL : public Usermod
{
  private:
    unsigned long lastTime = 0;
    int lastTimeSeconds = -1;

    // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
    bool usermodActive = false;
    bool displayItIs = false;
    int ledOffset = 0;

    // defines for mask sizes
    #define maskSizeLeds        256
    #define maskSizeMinutes     8
    #define maskSizeHours       6
    #define maskSizeSeconds     1
    #define maskSizeMonths      3

    // "minute" masks (and some extra words)
    //   | 0123456789ABCDEF
    // --+-----------------
    // 0 | .......ZEVENEGEN
    // 1 | KWARTWEENVIERZES
    // 2 | VIJFDRIELFMINUUT
    // 3 | ACHTWAALFDERTIEN
    // 4 | VEERTIEN MINUTEN
    // 5 | VOOROVER HALF
    // 9 | S.NACHTS.MIDDAGS
    // A | S.AVONDS.MORGENS
    const int maskMinutes[26][maskSizeMinutes] =
    {
      { 141, 142, 143,  -1,  -1,  -1,  -1,  -1 }, //  0 - uur
      {  22,  23,  24,  -1,  -1,  -1,  -1,  -1 }, //  1 - een
      {  20,  21,  22,  23,  -1,  -1,  -1,  -1 }, //  2 - twee
      {  36,  37,  38,  39,  -1,  -1,  -1,  -1 }, //  3 - drie
      {  25,  26,  27,  28,  -1,  -1,  -1,  -1 }, //  4 - vier
      {  32,  33,  34,  35,  -1,  -1,  -1,  -1 }, //  5 - vijf
      {  29,  30,  31,  -1,  -1,  -1,  -1,  -1 }, //  6 - zes
      {   7,   8,   9,  10,  11,  -1,  -1,  -1 }, //  7 - zeven
      {  48,  49,  50,  51,  -1,  -1,  -1,  -1 }, //  8 - acht
      {  11,  12,  13,  14,  15,  -1,  -1,  -1 }, //  9 - negen
      {  60,  61,  62,  63,  -1,  -1,  -1,  -1 }, // 10 - tien
      {  39,  40,  41,  -1,  -1,  -1,  -1,  -1 }, // 11 - elf
      {  51,  52,  53,  54,  55,  56,  -1,  -1 }, // 12 - twaalf
      {  57,  58,  59,  60,  61,  62,  63,  -1 }, // 13 - dertien
      {  64,  65,  66,  67,  68,  69,  70,  71 }, // 14 - veertien
      {  16,  17,  18,  19,  20,  -1,  -1,  -1 }, // 15 - kwart
      {  90,  91,  92,  93,  -1,  -1,  -1,  -1 }, // 16 - half
      {  42,  43,  44,  45,  46,  47,  -1,  -1 }, // 17 - minuut
      {  73,  74,  75,  76,  77,  78,  79,  -1 }, // 18 - minuten
      {  80,  81,  82,  83,  -1,  -1,  -1,  -1 }, // 19 - voor
      {  85,  86,  87,  88,  -1,  -1,  -1,  -1 }, // 20 - over
      {   0,   1,   2,   4,   5,  -1,  -1,  -1 }, // 21 - het is (it is)
      { 144, 146, 147, 148, 149, 150, 151,  -1 }, // 22 - snachts
      { 151, 153, 154, 155, 156, 157, 158, 159 }, // 23 - smiddags
      { 160, 162, 163, 164, 165, 166, 167,  -1 }, // 24 - savonds
      { 167, 169, 170, 171, 172, 173, 174, 175 }  // 25 - smorgens
    };

    // hour masks
    //   | 0123456789ABCDEF
    // --+-----------------
    // 6 | TIENEGENVIERVIJF
    // 7 | DRIELFTWEENZEVEN
    // 8 | ACHTWAALFZES UUR
    const int maskHours[12][maskSizeHours] =
    {
      { 131, 132, 133, 134, 135, 136}, // 00: twaalf
      { 120, 121, 122,  -1,  -1,  -1}, // 01: een
      { 118, 119, 120, 121,  -1,  -1}, // 02: twee
      { 112, 113, 114, 115,  -1,  -1}, // 03: drie
      { 104, 105, 106, 107,  -1,  -1}, // 04: vier
      { 108, 109, 110, 111,  -1,  -1}, // 05: vijf
      { 137, 138, 139,  -1,  -1,  -1}, // 06: zes
      { 123, 124, 125, 126, 127,  -1}, // 07: zeven
      { 128, 129, 130, 131,  -1,  -1}, // 08: acht
      {  99, 100, 101, 102, 103,  -1}, // 09: negen
      {  96,  97,  98,  99,  -1,  -1}, // 10: tien
      { 115, 116, 117,  -1,  -1,  -1}  // 11: elf
    };

    const int maskSeconds[10][maskSizeSeconds] =
    {
      { 246 }, // 00
      { 247 }, // 01
      { 248 }, // 02
      { 249 }, // 03
      { 250 }, // 04
      { 251 }, // 05
      { 252 }, // 06
      { 253 }, // 07
      { 254 }, // 08
      { 255 }  // 09
    };

    const int maskSecondTens[6][maskSizeSeconds] =
    {
      {  -1 }, // 00
      { 241 }, // 01
      { 242 }, // 02
      { 243 }, // 03
      { 244 }, // 04
      { 245 }  // 05
    };

    // month masks
    //   | 0123456789ABCDEF
    // --+-----------------
    // B | .01230123456789.
    // C | ..JANFEBMRTAPR..
    // D | ..MEIJUNJULAUG..
    // E | ..SEPOKTNOVDEC..
    const int maskMonths[12][maskSizeMonths] =
    {
      { 194,195,196 }, // 00 jan
      { 197,198,199 }, // 01 feb
      { 200,201,202 }, // 02 mrt
      { 203,204,205 }, // 03 apr
      { 210,211,212 }, // 04 mei
      { 213,214,215 }, // 05 jun
      { 216,217,218 }, // 06 jul
      { 219,220,221 }, // 07 aug
      { 226,227,228 }, // 08 sep
      { 229,230,231 }, // 09 okt
      { 232,233,234 }, // 10 nov
      { 235,236,237 }  // 11 dec
    };

    const int maskDays[10][maskSizeSeconds] =
    {
      { 182 }, // 00
      { 183 }, // 01
      { 184 }, // 02
      { 185 }, // 03
      { 186 }, // 04
      { 187 }, // 05
      { 188 }, // 06
      { 189 }, // 07
      { 190 }, // 08
      { 191 }  // 09
    };

    const int maskDayTens[6][maskSizeSeconds] =
    {
      { 176 }, // 00
      { 177 }, // 01
      { 178 }, // 02
      { 179 }, // 03
      { 180 }, // 04
      { 181 }, // 05
    };


    // overall mask to define which LEDs are on
    int maskLedsOn[maskSizeLeds] =
    {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };

    // update led mask
    void updateLedMask(const int wordMask[], int arraySize)
    {
      // loop over array
      for (int x=0; x < arraySize; x++)
      {
        // check if mask has a valid LED number
        if (wordMask[x] >= 0 && wordMask[x] < maskSizeLeds)
        {
          // turn LED on
          maskLedsOn[wordMask[x]] = 1;
        }
      }
    }

    // update the display
    void updateDisplay(uint8_t months, uint8_t days, uint8_t hours, uint8_t minutes, uint8_t seconds)
    {
      // disable complete matrix at the begin
      for (int x = 0; x < maskSizeLeds; x++)
      {
        maskLedsOn[x] = 0;
      }

      int secondTens = seconds/10;
      int dayTens = days/10;
      int hoursIndex = hours;
      int minutesIndex = minutes;
      bool voor = false; // before / to
      bool over = false; // after / past
      bool half = false; // "half"-o-clock ;-)
      bool minuten = false;
      int partOfDayIndex;

      // calc part of day
      partOfDayIndex = 22;                                    // 22 - snachts
      if (hours>=7 && hours<12) partOfDayIndex = 25;          // 25 - smorgens
      if (hours>=12 && hours<18) partOfDayIndex = 23;         // 23 - smiddags
      if (hours==17 && minutes>15) partOfDayIndex = 24;       // 24 - savonds
      if (hours>=18) partOfDayIndex = 24;                     // 24 - savonds


      // calc hours index
      if ( minutes>15 ) hoursIndex++;
      hoursIndex %= 12;

      // calc minutes index
      if ( minutesIndex==15 ) {
        over = true;
      }
      if ( minutes==30 ) minutesIndex = 16;
      if ( minutes==45 ) {
        minutesIndex = 15;
        voor = true;
      }
      if( minutes > 0 && minutes < 15 ) {
        over = true;
        minuten = true;
      }
      if( minutes > 15 && minutes < 30 ) {
        minutesIndex = 30 - minutes;
        voor = true;
        half = true;
        minuten = true;
      }
      if( minutes > 30 && minutes < 45 ) {
        minutesIndex = minutes%30;
        over = true;
        half = true;
        minuten = true;
      }
      if( minutes > 45 && minutes < 60 ) {
        minutesIndex = 60 - minutes;
        voor = true;
        minuten = true;
      }
      if( minutesIndex > 21 )
        minutesIndex = 0;

      // updateLedMask
      if (half)
        updateLedMask(maskMinutes[16], maskSizeMinutes);
      if (minuten) {
        if (minutesIndex==1)
          updateLedMask(maskMinutes[17], maskSizeMinutes);
        else
          updateLedMask(maskMinutes[18], maskSizeMinutes);
      }
      if (voor)
        updateLedMask(maskMinutes[19], maskSizeMinutes);
      if (over)
        updateLedMask(maskMinutes[20], maskSizeMinutes);
      if (displayItIs) // display it is/het is if activated
        updateLedMask(maskMinutes[21], maskSizeMinutes);

      updateLedMask(maskHours[hoursIndex], maskSizeHours);
      updateLedMask(maskMinutes[minutesIndex], maskSizeMinutes);
      updateLedMask(maskMinutes[partOfDayIndex], maskSizeMinutes);
      updateLedMask(maskSeconds[(seconds%10)], maskSizeSeconds);
      updateLedMask(maskSecondTens[secondTens], maskSizeSeconds);
      updateLedMask(maskMonths[(months+11)%12], maskSizeMonths);
      updateLedMask(maskDays[(days%10)], maskSizeSeconds);
      updateLedMask(maskDayTens[dayTens], maskSizeSeconds);

    }

  public:
    //Functions called by WLED

    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * You can use it to initialize variables, sensors or similar.
     */
    void setup()
    {
    }

    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected()
    {
    }

    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     *
     * Tips:
     * 1. You can use "if (WLED_CONNECTED)" to check for a successful network connection.
     *    Additionally, "if (WLED_MQTT_CONNECTED)" is available to check for a connection to an MQTT broker.
     *
     * 2. Try to avoid using the delay() function. NEVER use delays longer than 10 milliseconds.
     *    Instead, use a timer check as shown here.
     */
    void loop() {

      // do it every 1 seconds
      if (millis() - lastTime > 1000)
      {
        // check the time
        int seconds = second(localTime);

        // check if we already updated this minute
        if (lastTimeSeconds != seconds)
        {
          // update the display with new time
          updateDisplay(month(localTime), day(localTime), hour(localTime), minute(localTime), second(localTime));
          // remember last update time
          lastTimeSeconds = seconds;
        }

        // remember last update
        lastTime = millis();
      }
    }

    /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     * Below it is shown how this could be used for e.g. a light sensor
     */
    /*
    void addToJsonInfo(JsonObject& root)
    {
    }
    */

    /*
     * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void addToJsonState(JsonObject& root)
    {
    }

    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void readFromJsonState(JsonObject& root)
    {
    }

    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     * If you want to force saving the current state, use serializeConfig() in your loop().
     *
     * CAUTION: serializeConfig() will initiate a filesystem write operation.
     * It might cause the LEDs to stutter and will cause flash wear if called too often.
     * Use it sparingly and always in the loop, never in network callbacks!
     *
     * addToConfig() will make your settings editable through the Usermod Settings page automatically.
     *
     * Usermod Settings Overview:
     * - Numeric values are treated as floats in the browser.
     *   - If the numeric value entered into the browser contains a decimal point, it will be parsed as a C float
     *     before being returned to the Usermod.  The float data type has only 6-7 decimal digits of precision, and
     *     doubles are not supported, numbers will be rounded to the nearest float value when being parsed.
     *     The range accepted by the input field is +/- 1.175494351e-38 to +/- 3.402823466e+38.
     *   - If the numeric value entered into the browser doesn't contain a decimal point, it will be parsed as a
     *     C int32_t (range: -2147483648 to 2147483647) before being returned to the usermod.
     *     Overflows or underflows are truncated to the max/min value for an int32_t, and again truncated to the type
     *     used in the Usermod when reading the value from ArduinoJson.
     * - Pin values can be treated differently from an integer value by using the key name "pin"
     *   - "pin" can contain a single or array of integer values
     *   - On the Usermod Settings page there is simple checking for pin conflicts and warnings for special pins
     *     - Red color indicates a conflict.  Yellow color indicates a pin with a warning (e.g. an input-only pin)
     *   - Tip: use int8_t to store the pin value in the Usermod, so a -1 value (pin not set) can be used
     *
     * See usermod_v2_auto_save.h for an example that saves Flash space by reusing ArduinoJson key name strings
     *
     * If you need a dedicated settings page with custom layout for your Usermod, that takes a lot more work.
     * You will have to add the setting to the HTML, xml.cpp and set.cpp manually.
     * See the WLED Soundreactive fork (code and wiki) for reference.  https://github.com/atuline/WLED
     *
     * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
     */
    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject(F("WordClockUsermod1616NL"));
      top[F("active")] = usermodActive;
      top[F("displayItIs")] = displayItIs;
      top[F("ledOffset")] = ledOffset;
    }

    void appendConfigData()
    {
      oappend(SET_F("addInfo('WordClockUsermod1616NL:ledOffset', 1, 'Number of LEDs before the letters');"));
    }

    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens immediately after boot, or after saving on the Usermod Settings page)
     *
     * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
     * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
     * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
     *
     * Return true in case the config values returned from Usermod Settings were complete, or false if you'd like WLED to save your defaults to disk (so any missing values are editable in Usermod Settings)
     *
     * getJsonValue() returns false if the value is missing, or copies the value into the variable provided and returns true if the value is present
     * The configComplete variable is true only if the "exampleUsermod" object and all values are present.  If any values are missing, WLED will know to call addToConfig() to save them
     *
     * This function is guaranteed to be called on boot, but could also be called every time settings are updated
     */
    bool readFromConfig(JsonObject& root)
    {
      // default settings values could be set here (or below using the 3-argument getJsonValue()) instead of in the class definition or constructor
      // setting them inside readFromConfig() is slightly more robust, handling the rare but plausible use case of single value being missing after boot (e.g. if the cfg.json was manually edited and a value was removed)

      JsonObject top = root[F("WordClockUsermod1616NL")];

      bool configComplete = !top.isNull();

      configComplete &= getJsonValue(top[F("active")], usermodActive);
      configComplete &= getJsonValue(top[F("displayItIs")], displayItIs);
      configComplete &= getJsonValue(top[F("ledOffset")], ledOffset);

      return configComplete;
    }

    /*
     * handleOverlayDraw() is called just before every show() (LED strip update frame) after effects have set the colors.
     * Use this to blank out some LEDs or set them to a different color regardless of the set effect mode.
     * Commonly used for custom clocks (Cronixie, 7 segment)
     */
    void handleOverlayDraw()
    {
      // check if usermod is active
      if (usermodActive == true)
      {
        // loop over all leds
        for (int x = 0; x < maskSizeLeds; x++)
        {
          // check mask
          if (maskLedsOn[x] == 0)
          {
            // set pixel off
            strip.setPixelColor(x + ledOffset, RGBW32(0,0,0,0));
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
      return USERMOD_ID_WORDCLOCK_1616_NL;
    }

   //More methods can be added in the future, this example will then be extended.
   //Your usermod will remain compatible as it does not need to implement all methods from the Usermod base class!
};