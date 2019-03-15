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

static bool check_rfid(
    RFID_RC522 * pRFIDDevice,
    StringParam * pStoredRFIDs[NUMBER_OF_RFID_TAGS],
    uint8_t i)
{
    char uuid1[20] = {'\0'};
    char uuid2[20] = {'\0'};
    int len1, len2;

    len1 = pRFIDDevice->get(uuid1);

    pStoredRFIDs[i]->get(uuid2);
    len2 = strlen(uuid2);

    return strncmp(uuid1, uuid2, max(len1, len2)) == 0;
}

static void check_program_flag(
    RFID_RC522 * pRFIDDevice,
    StringParam * pStoredRFIDs[NUMBER_OF_RFID_TAGS],
    IntegerParam * pProgramRFIDParam,
    uint8_t i)
{
    int32_t to_program = pProgramRFIDParam->get();
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
                pStoredRFIDs[to_program-1]->set(uuid);
                pStoredRFIDs[to_program-1]->save();
            }
        }
        pProgramRFIDParam->set(0);
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
}

void raat_custom_loop(const raat_devices_struct& devices, const raat_params_struct& params)
{
    for (uint8_t i=0; i<NUMBER_OF_RFID_TAGS; i++)
    {
        check_program_flag(devices.pRFID_Device, params.pSavedRFID, params.pProgram_RFID, i);
    }
}