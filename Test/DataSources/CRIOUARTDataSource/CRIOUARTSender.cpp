/**
 * @file CRIOUARTSender.cpp
 * @brief Source file for class CRIOUARTSender
 * @date 07/08/2018
 * @author Andre Neto
 * @author Marta Baldris
 *
 * @copyright Copyright 2015 F4E | European Joint Undertaking for ITER and
 * the Development of Fusion Energy ('Fusion for Energy').
 * Licensed under the EUPL, Version 1.1 or - as soon they will be approved
 * by the European Commission - subsequent versions of the EUPL (the "Licence")
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl
 *
 * @warning Unless required by applicable law or agreed to in writing, 
 * software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the Licence permissions and limitations under the Licence.

 * @details This source file contains the definition of all the methods for
 * the class CRIOUARTSender (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "CompilerTypes.h"
#include "CRIOUARTSerial.h"
#include "Sleep.h"
#include "StreamString.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
void MainErrorProcessFunction(const MARTe::ErrorManagement::ErrorInformation &errorInfo, const char * const errorDescription) {
    MARTe::StreamString errorCodeStr;
    MARTe::ErrorManagement::ErrorCodeToStream(errorInfo.header.errorType, errorCodeStr);
    MARTe::StreamString err;
    err.Printf("[%s - %s:%d]: %s", errorCodeStr.Buffer(), errorInfo.fileName, errorInfo.header.lineNumber, errorDescription);
    printf("%s\n", err.Buffer());
}

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

int main(int argc, char **argv) {
    SetErrorProcessFunction(&MainErrorProcessFunction);
    using namespace MARTe;
    CRIOUARTSerial serial;
    if (argc < 2) {
        printf("Please specify the name of the serial port\n");
        return -1;
    }

    uint32 speed = 115200;
    bool ok = serial.SetSpeed(speed);
    if (ok) {
        ok = serial.Open(argv[1]);
    }
    else {
        printf("Failed to open serial @ %s\n", argv[1]);
    }
    uint8 **mem = NULL;
    const uint32 numberOfBytes = 15;
    uint32 numberOfMems = 3000;
    if (ok) {
        mem = new uint8*[numberOfMems];
        uint32 i = 0;
        uint32 j = 0;
        for (j = 0; j < numberOfMems; j++) {
            mem[j] = new uint8[numberOfBytes];
            for (i = 0; i < 7; i++) {
                mem[j][i] = j;
            }
            for (i = 8; i < 15; i++) {
                mem[j][i] = 2 * j;
            }
        }
    }
    if (ok) {
        uint32 c;

        for (c = 10u; c < numberOfMems; c++) {
            serial.WaitWrite(100000);
            ok = serial.Write(reinterpret_cast<char8 *>(mem[c]), numberOfBytes);
            if (!ok) {
                printf("Failed to write %d bytes\n", numberOfBytes);
            }
            Sleep::Sec(10e-6);
        }
    }

    if (ok) {
        uint32 j = 0;
        for (j = 0; j < numberOfMems; j++) {
            delete mem[j];
        }
        delete[] mem;
    }

    serial.Close();
    return 0;
}

