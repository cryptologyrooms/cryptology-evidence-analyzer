/* Arduino Includes */

#include <Keyboard.h>
#include <AStar32U4.h>
#include <MFRC522.h>

/* RAAT Includes */

#include "raat.hpp"
#include "adafruit-neopixel-raat.hpp"

/* Application Includes */

#include "leds.h"

/* Local Variables */

static bool bIdle = true;

/* Public Functions */

void leds_setup(AdafruitNeoPixelRAAT * pNeoPixels)
{
    pNeoPixels->clear();
    pNeoPixels->show();
}

void leds_run(AdafruitNeoPixelRAAT * pNeoPixels)
{
    if (!bIdle)
    {
        bIdle = false;
    }
}

void leds_start_match_animation()
{
    if (bIdle)
    {
        bIdle = false;
    }
}

void leds_start_no_match_animation()
{
    if (bIdle)
    {
        bIdle = false;
    }
}
