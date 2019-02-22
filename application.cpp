/* Arduino Includes */

#include <MFRC522.h>

/* RAAT Includes */

#include "raat.h"

#include "string-param.h"

#include "adafruit-neopixel-raat.h"
#include "rfid-rc522.h"

/* Application Includes */

/* Defines, typedefs, constants */

/* Private Variables */

/* Private Functions */

static bool check_rfid(uint8_t i)
{
    char uuid1[20] = {NULL};
    char uuid2[20] = {NULL};
    int len1, len2;

    len1 = s_pRFID->get(uuid1);

    s_pStoredRFIDs[i]->get(uuid2);
    len2 = strlen(uuid2);

    return strncmp(uuid1, uuid2, max(len1, len2)) == 0;
}

static void check_program_flag(uint8_t i)
{
    int32_t to_program = s_pProgramRFID->get();
    char uuid[20];
    uint8_t uuid_length = 0;

    if (to_program == (i+1))
    {
        raat_logln(LOG_APP, "Waiting for RFID %d", to_program);
        while(uuid_length == 0)
        {
            uuid_length = s_pRFID->get(uuid);
            if (uuid_length)
            {
                raat_logln(LOG_APP, "Saved RFID %lu: <%s>", to_program, uuid);
                s_pStoredRFIDs[to_program-1]->set(uuid);
                s_pStoredRFIDs[to_program-1]->save();
            }
        }
        s_pProgramRFID->set(0);
    }
}

/* RAAT Functions */

void raat_custom_setup(raat_devices_struct& devices, raat_params_struct& params)
{
    char uuid[20];
    for (uint8_t i=0; i<NUMBER_OF_RFID_TAGS; i++)
    {
        s_pStoredRFIDs[i] = params.pSavedRFID01 + i;
        s_pStoredRFIDs[i]->get(uuid);
        if (strlen(uuid))
        {
            raat_logln(LOG_APP, "Saved RFID %u: <%s>", i+1, uuid);
        }
        else
        {
            raat_logln(LOG_APP, "No saved RFID %u", i+1);
        }
    }

    s_pRFID = (RFID_RC522*)pdevices[5];

    s_pOnTime = (IntegerParam*)pparams[0];
    s_pProgramRFID = (IntegerParam*)pparams[1];

    
}

void raat_custom_loop(raat_devices_struct& devices, raat_params_struct& params)
{
    for (uint8_t i=0; i<NUMBER_OF_RFID_TAGS; i++)
    {
        run_output(i);
        check_program_flag(i);
    }
}