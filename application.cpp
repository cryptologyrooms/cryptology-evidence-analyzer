/* Arduino Includes */

#include <MFRC522.h>

/* RAAT Includes */

#include "raat.hpp"

#include "string-param.hpp"

#include "adafruit-neopixel-raat.hpp"
#include "rfid-rc522.hpp"

/* Application Includes */

/* Defines, typedefs, constants */

/* Private Variables */

/* Private Functions */

static bool check_rfid(RFID_RC522 * pRFID, StringParam * pRFIDParam, uint8_t i)
{
    char uuid1[20] = {NULL};
    char uuid2[20] = {NULL};
    int len1, len2;

    len1 = pRFID->get(uuid1);

    pRFIDParam->get(uuid2);
    len2 = strlen(uuid2);

    return strncmp(uuid1, uuid2, max(len1, len2)) == 0;
}

static void check_program_flag(RFID_RC522 * pRFID, IntegerParam * pRFIDToProgramParam, StringParam * pRFIDStorageParam, uint8_t i)
{
    int32_t to_program = pRFIDToProgramParam->get();
    char uuid[20];
    uint8_t uuid_length = 0;

    if (to_program == (i+1))
    {
        raat_logln(LOG_APP, "Waiting for RFID %d", to_program);
        while(uuid_length == 0)
        {
            uuid_length = pRFID->get(uuid);
            if (uuid_length)
            {
                raat_logln(LOG_APP, "Saved RFID %lu: <%s>", to_program, uuid);
                pRFIDStorageParam->set(uuid);
                pRFIDStorageParam->save();
            }
        }
        pRFIDToProgramParam->set(0);
    }
}

/* RAAT Functions */

void raat_custom_setup(raat_devices_struct& devices, raat_params_struct& params)
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
}

void raat_custom_loop(raat_devices_struct& devices, raat_params_struct& params)
{
    for (uint8_t i=0; i<NUMBER_OF_RFID_TAGS; i++)
    {
        check_rfid(devices.pRFID, params.pSavedRFID[i], i);
        check_program_flag(devices.pRFID, params.pRFIDToProgram, params.pSavedRFID[i], i);
    }
}