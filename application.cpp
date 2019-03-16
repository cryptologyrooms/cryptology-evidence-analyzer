/* Arduino Includes */

#include <Keyboard.h>
#include <AStar32U4.h>
#include <MFRC522.h>

/* RAAT Includes */

#include "raat.hpp"

#include "string-param.hpp"

#include "adafruit-neopixel-raat.hpp"
#include "rfid-rc522.hpp"

/* Application Includes */

#include "leds.h"

/* Defines, typedefs, constants */

static const char CHARACTERS[] = "abcdefghijklmnopqrstuvwxyz";
static const char NO_MATCH_CHARACTER = 'Z';

static const uint8_t NO_MATCH_RESULT = 0XFF;

/* Private Variables */

/* Private Functions */

static void check_program_flag(
    RFID_RC522 * pRFIDDevice,
    StringParam * pStoredRFIDParam,
    IntegerParam * pRFIDToProgramParam,
    uint8_t i)
{
    int32_t to_program = pRFIDToProgramParam->get();
    char uuid[20];
    uint8_t uuid_length = 0;

    if (to_program == (i+1))
    {
        raat_logln(LOG_APP, "Waiting for RFID %d", to_program);
        while(uuid_length == 0)
        {

            uuid_length = pRFIDDevice->get(uuid);
            if (uuid_length)
            {
                raat_logln(LOG_APP, "Saved RFID %lu: <%s>", to_program, uuid);
                pStoredRFIDParam->set(uuid);
                pStoredRFIDParam->save();
            }
        }
        pRFIDToProgramParam->set(0);
    }
}

/* RAAT Functions */

void raat_custom_setup(const raat_devices_struct& devices, const raat_params_struct& params)
{
    char uuid[20];
    for (uint8_t i=0; i<NUMBER_OF_RFID_TAGS; i++)
    {
        params.pSavedRFID[i]->get(uuid);
        if (strlen(uuid))
        {
            raat_logln(LOG_APP, "Saved RFID %u: <%s>", i+1, uuid);
        }
        else
        {
            raat_logln(LOG_APP, "No saved RFID %u", i+1);
        }
    }
    leds_setup(devices.pLEDs);
}

void raat_custom_loop(const raat_devices_struct& devices, const raat_params_struct& params)
{
    bool analyze_button_pressed = devices.pAnalyzeButton->check_low_and_clear();

    if (analyze_button_pressed)
    {
        char uuid[20];
        uint8_t uuid_length = devices.pRFID_Device->get(uuid);
        uint8_t match = NO_MATCH_RESULT;
        if (uuid_length)
        {
            raat_logln(LOG_APP, "Scanning RFIDs for <%s>", uuid);

            for (uint8_t i=0; i<NUMBER_OF_RFID_TAGS; i++)
            {
                if (params.pSavedRFID[i]->strncmp(uuid, uuid_length) == 0)
                {
                    match = (int8_t)i;
                }
            }

            if (match != NO_MATCH_RESULT)
            {
                raat_logln(LOG_APP, "Matched RFID #%d (%c)", match+1, CHARACTERS[match]);
                Keyboard.press(CHARACTERS[match]);
                Keyboard.release(CHARACTERS[match]);
                leds_start_match_animation();
            }
            else
            {
                raat_logln(LOG_APP, "No match!");
                Keyboard.press(NO_MATCH_CHARACTER);
                Keyboard.release(NO_MATCH_CHARACTER);
                leds_start_no_match_animation();
            }
        }
        else
        {
            raat_logln(LOG_APP, "No card found");
        }
    }

    for (uint8_t i=0; i<NUMBER_OF_RFID_TAGS; i++)
    {
        check_program_flag(devices.pRFID_Device, params.pSavedRFID[i], params.pRFIDToProgram, i);
    }

    leds_run(devices.pLEDs);
}