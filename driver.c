#pragma once

#define WIN32_LEAN_AND_MEAN

#include <QCoreApplication>
#include "AgE36xx.h"
#include <stdio.h>
#include <tchar.h>
#include <assert.h>

#ifndef checkErr
#define checkErr(fCall) if (error = (fCall), (error = (error < 0) ? error : VI_SUCCESS)) {goto Error; } else error = error;
#endif

int main(int argc, char *argv[])
{
    ViStatus error = VI_SUCCESS;
    QCoreApplication a(argc, argv);
    ViStatus status;
    ViSession session;
    ViInt32 ErrorCode;
    ViChar ErrorMessage[256];
    ViReal64 measVal;
    ViReal64 CurrLim;
    ViReal64 VoltLev;

    char resource[] = "USB::0x0699::0x0409::C024533::INSTR";
    char options[]  = "QueryInstrStatus=true, Simulate=true";

    ViBoolean idQuery = VI_TRUE;
    ViBoolean reset   = VI_TRUE;

    status = AgE36xx_InitWithOptions(resource, idQuery, reset, options, &session);

    if(status)
    {
        // Initialization failed
        AgE36xx_GetError(session, &ErrorCode, 255, ErrorMessage);
        printf("Error: %d, %s\n", ErrorCode, ErrorMessage);
        printf("\nDone - Press Enter to Exit");
        getchar();
        return ErrorCode;
    }
    assert(session != VI_NULL);
    printf("Driver Initialized \n");
    checkErr(AgE36xx_ConfigureCurrentLimit(session, "Output1", AGE36XX_VAL_CURRENT_REGULATE, 1.25));
    checkErr(AgE36xx_ConfigureVoltageLevel(session, "Output1", 2.5));
    checkErr(AgE36xx_Measure(session, "Output1", AGE36XX_VAL_MEASURE_VOLTAGE, &measVal));
    printf("Output 1 Measurement = %f Volts\n", measVal);
    checkErr(AgE36xx_GetAttributeViReal64(session, "", AGE36XX_ATTR_CURRENT_LIMIT, &CurrLim));
    printf("Output 1 Current Limit = %f Amps\n", CurrLim);
    checkErr(AgE36xx_GetAttributeViReal64(session, "", AGE36XX_ATTR_VOLTAGE_LEVEL, &VoltLev));
    printf("Output 1 CVoltage Level = %f Volts\n", VoltLev);
    Error:
        if (error != VI_SUCCESS)
        {
             AgE36xx_GetError(session, &ErrorCode, 255, ErrorMessage);
             assert(status == VI_SUCCESS);
             printf("error_query: %d, %s\n", ErrorCode, ErrorMessage);
             getchar();
             return ErrorCode;
        }
        status = AgE36xx_close(session);
        assert(status == VI_SUCCESS);
        session = VI_NULL;
        printf("Driver Closed \n");

        printf("\nDone - Press Enter to Exit");
        getchar();

        return 0;

    return a.exec();
}
