/* Arduino Includes */

#include <Keyboard.h>
#include <AStar32U4.h>
#include <MFRC522.h>

/* RAAT Includes */

#include "raat.hpp"
#include "adafruit-neopixel-raat.hpp"
#include "led-effect.hpp"
#include "raat-oneshot-timer.hpp"

/* Application Includes */

#include "leds.h"

/* Defines, constants, typedefs */

#define SCAN_COLOURS 128, 64, 128
#define NO_MATCH_COLOURS 128, 16, 0

static const uint8_t N_LARSON_LEDS = 5;

/* Private Function Prototypes */

static void larson_value_callback(uint8_t index, uint8_t * pMultiplier, uint8_t * pDivisor);

/* Local Variables */

static bool bRunning = false;
static bool bScanPending = false;
static bool bNoMatchPending = false;

uint8_t s_actual[NUMBER_OF_LEDS][3] = {0xFF};
LarsonScanner s_larson = LarsonScanner((uint8_t*)s_actual, NUMBER_OF_LEDS, 5, larson_value_callback);

static RAATOneShotTimer s_scan_timer(40);

/* Private Functions */

static void larson_value_callback(uint8_t index, uint8_t * pMultiplier, uint8_t * pDivisor)
{
    uint8_t multiplier = index+1;
    *pMultiplier = multiplier * multiplier;
    *pDivisor = ((N_LARSON_LEDS + 1) * (N_LARSON_LEDS + 1)) / 6;
}

static void copy_actual_values_to_leds_and_show(AdafruitNeoPixelRAAT * pNeoPixels)
{
    for (uint8_t i=0;i<NUMBER_OF_LEDS;i++)
    {
        pNeoPixels->setPixelColor(i, s_actual[i][0], s_actual[i][1], s_actual[i][2]);
    }
    pNeoPixels->show();
}

static void flash_no_match(AdafruitNeoPixelRAAT * pNeoPixels)
{
    for (uint8_t i=0;i<NUMBER_OF_LEDS;i++)
    {
        pNeoPixels->setPixelColor(i, NO_MATCH_COLOURS);
    }
    pNeoPixels->show(); 
    delay(300);
    pNeoPixels->clear(); 
    pNeoPixels->show(); 
    delay(200);
}

/* Public Functions */

void leds_setup(AdafruitNeoPixelRAAT * pNeoPixels)
{
    pNeoPixels->clear();
    pNeoPixels->show();

    s_scan_timer.start();
}

bool leds_run(AdafruitNeoPixelRAAT * pNeoPixels, RGBParam * pScanColour, uint8_t nscans, uint32_t scantime)
{
    if (!bRunning)
    {
        if (bScanPending)
        {
            bScanPending = false;
            s_scan_timer.start(scantime / (nscans * NUMBER_OF_LEDS));
            s_larson.start(pScanColour->get(eR), pScanColour->get(eG), pScanColour->get(eB), nscans);
            //s_larson.start(SCAN_COLOURS, 3);
            bRunning = true;
            copy_actual_values_to_leds_and_show(pNeoPixels);
        }
        else if (bNoMatchPending)
        {
            bNoMatchPending = false;
            // TODO: Fade red LEDs up/down
            flash_no_match(pNeoPixels);
            flash_no_match(pNeoPixels);
            flash_no_match(pNeoPixels);
        }
    }

    if (bRunning && s_scan_timer.check_and_restart())
    {
        bRunning = s_larson.update();
        copy_actual_values_to_leds_and_show(pNeoPixels);
    }
    return bRunning;
}

void leds_test(AdafruitNeoPixelRAAT * pNeoPixels)
{
    s_larson.start(128,32,32, 1);
    copy_actual_values_to_leds_and_show(pNeoPixels);
    while(s_larson.update())
    {
        delay(40);
        copy_actual_values_to_leds_and_show(pNeoPixels);
    }

    s_larson.start(32,128,32, 1);
    copy_actual_values_to_leds_and_show(pNeoPixels);
    while(s_larson.update())
    {
        delay(40);
        copy_actual_values_to_leds_and_show(pNeoPixels);
    }

    s_larson.start(32, 32, 128, 1);
    copy_actual_values_to_leds_and_show(pNeoPixels);
    while(s_larson.update())
    {
        delay(40);
        copy_actual_values_to_leds_and_show(pNeoPixels);
    }

    pNeoPixels->clear();
    pNeoPixels->show();
}

void leds_pend_scan_animation()
{
    bScanPending = true;
}

void leds_pend_no_match_animation()
{
    bNoMatchPending = true;
}
