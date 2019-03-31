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

typedef enum game_state
{
    GAME_STATE_IDLE,
    GAME_STATE_SCANNED_NO_RFID,
    GAME_STATE_SCANNED_RFID_MATCHED,
    GAME_STATE_SCANNED_RFID_NOT_MATCHED
} GAME_STATE;

static const char NO_RFID_CHAR = 'S';
static const char NO_MATCH_CHAR = 'X';
static const char PRESS_DURING_SCAN_CHAR = 'Y';
static const char SCAN_START_CHAR = 'Z';

static const uint8_t NO_MATCH_RESULT = 0XFF;

/* Private Variables */

static GAME_STATE s_game_state = GAME_STATE_IDLE;

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
                raat_logln(LOG_APP, "Saved RFID %u: <%s>", (uint8_t)to_program, uuid);
                pStoredRFIDParam->set(uuid);
                pStoredRFIDParam->save();
            }
        }
        pRFIDToProgramParam->set(0);
    }
}

static void press_char(char c)
{
    Keyboard.press(c);
    Keyboard.release(c);
}

static void try_eeprom_logging(const raat_params_struct& params)
{
    static bool s_bLogged  = false;

    if (!s_bLogged)
    {
        if (Serial)
        {
            char uuid[20];
            for (uint8_t i=0; i<NUMBER_OF_RFID_TAGS; i++)
            {
                params.pSavedRFID[i]->get(uuid);
                raat_logln(LOG_APP, "Saved RFID %u: <%s>", i+1, strlen(uuid) ? uuid : "");
            }
            s_bLogged = true;
        }
    }
}

/* RAAT Functions */

void raat_custom_setup(const raat_devices_struct& devices, const raat_params_struct& params)
{
    (void)params;
    leds_setup(devices.pLEDs);
    leds_test(devices.pLEDs);
}

void raat_custom_loop(const raat_devices_struct& devices, const raat_params_struct& params)
{

    static char pending_char = 0;

    try_eeprom_logging(params);

    bool analyze_button_pressed = devices.pAnalyzeButton->check_low_and_clear();

    switch(s_game_state)
    {
        case GAME_STATE_IDLE:
            if (analyze_button_pressed)
            {
                char uuid[20];
                uint8_t uuid_length = devices.pRFID_Device->get(uuid);
                uint8_t match = NO_MATCH_RESULT;

                leds_pend_scan_animation();
                press_char(SCAN_START_CHAR);

                if (uuid_length)
                {
                    raat_logln(LOG_APP, "Search for <%s>", uuid);

                    for (uint8_t i=0; i<NUMBER_OF_RFID_TAGS; i++)
                    {
                        if (params.pSavedRFID[i]->strncmp(uuid, uuid_length) == 0)
                        {
                            match = (int8_t)i;
                        }
                    }

                    if (match != NO_MATCH_RESULT)
                    {
                        raat_logln(LOG_APP, "Match #%d (%c)", match+1, 'a' + match);
                        pending_char = 'a' + match;
                        s_game_state = GAME_STATE_SCANNED_RFID_MATCHED;
                    }
                    else
                    {
                        raat_logln(LOG_APP, "No match!");
                        pending_char = NO_MATCH_CHAR;
                        leds_pend_no_match_animation();
                        s_game_state = GAME_STATE_SCANNED_RFID_NOT_MATCHED;
                    }
                }
                else
                {
                    raat_logln(LOG_APP, "No RFID");
                    pending_char = NO_RFID_CHAR;
                    s_game_state = GAME_STATE_SCANNED_NO_RFID;
                }
                devices.pRFID_Device->forget();
            }
            (void)leds_run(
                devices.pLEDs, params.pScanColour,
                params.pScanNumber->get(),
                params.pScanTime->get()
            );
            break;

        case GAME_STATE_SCANNED_NO_RFID:
        case GAME_STATE_SCANNED_RFID_NOT_MATCHED:
        case GAME_STATE_SCANNED_RFID_MATCHED:
            if (analyze_button_pressed)
            {
                press_char(PRESS_DURING_SCAN_CHAR);
            }

            bool running = leds_run(
                devices.pLEDs, params.pScanColour,
                params.pScanNumber->get(),
                params.pScanTime->get()
            );

            if (!running && pending_char)
            {
                press_char(pending_char);
                pending_char = 0;
                s_game_state = GAME_STATE_IDLE;
            }
            break;
    }

    for (uint8_t i=0; i<NUMBER_OF_RFID_TAGS; i++)
    {
        check_program_flag(devices.pRFID_Device, params.pSavedRFID[i], params.pRFIDToProgram, i);
    }
}