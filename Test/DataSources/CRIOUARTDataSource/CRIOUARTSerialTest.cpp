/**
 * @file CRIOUARTSerialTest.cpp
 * @brief Source file for class CRIOUARTSerialTest
 * @date 07/08/2018
 * @author Andre Neto
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
 * the class CRIOUARTSerialTest (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "CRIOUARTSerial.h"
#include "CRIOUARTSerialTest.h"
#include "EventSem.h"
#include "StreamString.h"
#include "Threads.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
struct SerialCallbackData {
    CRIOUARTSerial *serial;
    MARTe::EventSem *sem;
};

void SerialCallback(const void * const serialPtr) {
    using namespace MARTe;
    const SerialCallbackData * const data = reinterpret_cast<const SerialCallbackData * const >(serialPtr);
    StreamString buffer = "CRIOUARTSerialTest";
    uint32 size = buffer.Size() + 1u;
    data->serial->WaitWrite(1000000);
    data->serial->Write(const_cast<char8 *>(buffer.Buffer()), size);
    data->sem->Post();
}

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

bool CRIOUARTSerialTest::TestConstructor() {
    using namespace MARTe;
    CRIOUARTSerial serial;
    return true;
}

bool CRIOUARTSerialTest::TestSetSpeed() {
    using namespace MARTe;
    CRIOUARTSerial serial;
    uint32 speed = 115200;
    const uint32 speedTable[31] = { 0u, 50u, 75u, 110u, 134u, 150u, 200u, 300u, 600u, 1200, 1800, 2400u, 4800u, 9600u, 19200u, 38400u, 57600u, 115200u, 230400u, 460800u, 500000u, 576000u, 921600u,
            1000000u, 1152000u, 1500000u, 2000000u, 2500000u, 3000000, 3500000, 4000000u };
    bool ok = true;
    uint32 i = 0u;
    while ((ok) && (i < 31)) {
        speed = speedTable[i];
        ok = serial.SetSpeed(speed);
        i++;
    }
    if (ok) {
        speed = 115200;
        ok = serial.SetSpeed(speed);
    }
    if (ok) {
        ok = serial.Open("/dev/ttyUSB0");
    }

    serial.Close();
    return ok;
}

bool CRIOUARTSerialTest::TestSetSpeed_False() {
    using namespace MARTe;
    CRIOUARTSerial serial;
    uint32 speed = 115201;
    bool ok = !serial.SetSpeed(speed);
    return ok;
}

bool CRIOUARTSerialTest::TestSetSpeed_False_Open() {
    using namespace MARTe;
    CRIOUARTSerial serial;
    uint32 speed = 115200;
    bool ok = serial.SetSpeed(speed);
    if (ok) {
        ok = serial.Open("/dev/ttyUSB0");
    }
    if (ok) {
        ok = !serial.SetSpeed(speed);
    }
    serial.Close();
    return ok;
}

bool CRIOUARTSerialTest::TestOpen() {
    using namespace MARTe;
    CRIOUARTSerial serial;
    uint32 speed = 115200;
    serial.SetSpeed(speed);
    bool ok = serial.Open("/dev/ttyUSB0");
    serial.Close();
    return ok;
}

bool CRIOUARTSerialTest::TestOpen_WrongName() {
    using namespace MARTe;
    CRIOUARTSerial serial;
    uint32 speed = 115200;
    serial.SetSpeed(speed);
    bool ok = !serial.Open("/dev/ttyUSB0adsas");
    serial.Close();
    return ok;
}

bool CRIOUARTSerialTest::TestClose() {
    return TestOpen();
}

bool CRIOUARTSerialTest::TestRead() {
    using namespace MARTe;
    CRIOUARTSerial serial;

    EventSem sem;
    sem.Create();
    sem.Reset();

    uint32 speed = 9600;

    SerialCallbackData data;
    data.serial = &serial;
    data.sem = &sem;

    bool ok = serial.SetSpeed(speed);
    if (ok) {
        ok = serial.Open("/dev/ttyUSB0");
        Threads::BeginThread(&SerialCallback, &data);
        sem.Wait();
    }
    if (ok) {
        ok = serial.WaitRead(5000000);
    }
    const uint32 len = 19u;
    char8 dataRead[len];

    if (ok) {
        uint32 counter = 0u;
        uint32 size = 0;
        while ((size < len) && (ok)) {
            uint32 rsize = (len - size);
            ok = serial.Read(&dataRead[size], rsize);
            if (rsize == 0xffffffff) {
                counter++;
                if (counter < 30) {
                    Sleep::Sec(0.1);
                }
                else {
                    ok = false;
                }
            }
            else {
                size += rsize;
            }
        }
    }
    if (ok) {
        StreamString dataReadStr = dataRead;
        ok = (dataReadStr == "CRIOUARTSerialTest");
        serial.Close();
    }
    sem.Close();
    return ok;
}

bool CRIOUARTSerialTest::TestReadTimeout() {
    using namespace MARTe;
    CRIOUARTSerial serial;

    EventSem sem;
    sem.Create();
    sem.Reset();

    uint32 speed = 9600;

    SerialCallbackData data;
    data.serial = &serial;
    data.sem = &sem;

    bool ok = serial.SetSpeed(speed);
    if (ok) {
        ok = serial.Open("/dev/ttyUSB0");
        Threads::BeginThread(&SerialCallback, &data);
        sem.Wait();
    }
    const uint32 len = 19u;
    char8 dataRead[len];

    if (ok) {
        uint32 size = len;
        ok = serial.Read(&dataRead[0], size, 2000000);
    }
    if (ok) {
        StreamString dataReadStr = dataRead;
        ok = (dataReadStr == "CRIOUARTSerialTest");
        serial.Close();
    }
    sem.Close();
    return ok;
}

bool CRIOUARTSerialTest::TestWaitRead() {
    return TestRead();
}

bool CRIOUARTSerialTest::TestWrite() {
    return TestRead();
}

bool CRIOUARTSerialTest::TestWaitWrite() {
    return TestRead();
}
