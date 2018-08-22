/**
 * @file PacketCRCTest.cpp
 * @brief Source file for class PacketCRCTest
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
 * the class PacketCRCTest (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include <stdio.h>

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "GAMScheduler.h"
#include "MemoryDataSourceI.h"
#include "MemoryMapInputBroker.h"
#include "MemoryMapOutputBroker.h"
#include "MemoryOperationsHelper.h"
#include "ObjectRegistryDatabase.h"
#include "PacketCRC.h"
#include "PacketCRCTest.h"
#include "RealTimeApplication.h"
#include "StandardParser.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

bool PacketCRCTest::TestConstructor() {
    using namespace MARTe;

    PacketCRC crc;
    return true;
}

bool PacketCRCTest::TestComputeTable_0x3D65() {
    using namespace MARTe;
    MARTe::uint16 expectedCRC[] = { 0x4841, 0xFC81, 0x9EC9, 0x90D0, 0x397E, 0xEF87, 0xBC97, 0xE5AA };
    return TestComputeCRC(0x3D65, 0, false, expectedCRC);
}

bool PacketCRCTest::TestComputeTable_0x3D65_inverted() {
    using namespace MARTe;
    MARTe::uint16 expectedCRC[] = { 0x8E2D, 0x6826, 0x94EE, 0x8DF4, 0x272D, 0x708D, 0xEE98, 0xF4D8 };
    return TestComputeCRC(0x3D65, 0, true, expectedCRC);
}

bool PacketCRCTest::TestComputeTable_0x3D65_initialCRC() {
    using namespace MARTe;
    MARTe::uint16 expectedCRC[] = { 0xAF28, 0x1BE8, 0x79A0, 0x77B9, 0x44B4, 0x924D, 0xC15D, 0x9860 };
    return TestComputeCRC(0x3D65, 0xFFEE, false, expectedCRC);
}

bool PacketCRCTest::TestComputeTable_0x3D65_inverted_initialCRC() {
    using namespace MARTe;
    MARTe::uint16 expectedCRC[] = { 0x7068, 0x9663, 0x6AAB, 0x73B1, 0x5C09, 0xBA9, 0x95BC, 0x8FFC };
    return TestComputeCRC(0x3D65, 0xAABB, true, expectedCRC);
}

bool PacketCRCTest::TestComputeCRC(MARTe::uint16 pol, MARTe::uint16 initialCRC, bool inputInverted, MARTe::uint16 *expectedCRC) {
    using namespace MARTe;
    uint8 data1[16] = { 0xF, 0x0, 0xF, 0x0, 0xAA, 0x55, 0xAA, 0x55, 0xBB, 0xDD, 0xFF, 0xEE, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8 data2[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
    uint8 data3[16] = { 0x1, 0xFF, 0x2, 0xFE, 0x3, 0xFD, 0x4, 0xFC, 0x5, 0xFB, 0x6, 0xFA, 0x7, 0xEF, 0x8, 0xEE };
    uint8 data4[16] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

    PacketCRC packetCRC;
    packetCRC.SetInitialCRC(initialCRC);
    packetCRC.ComputeTable(pol);
    //Force delete of existent table (for coverage).
    packetCRC.ComputeTable(pol);
    uint32 size = 16;
    uint16 crc[8];
    crc[0] = packetCRC.ComputeCRC(&data1[inputInverted ? 15 : 0], size, inputInverted);
    crc[1] = packetCRC.ComputeCRC(&data2[inputInverted ? 15 : 0], size, inputInverted);
    crc[2] = packetCRC.ComputeCRC(&data3[inputInverted ? 15 : 0], size, inputInverted);
    crc[3] = packetCRC.ComputeCRC(&data4[inputInverted ? 15 : 0], size, inputInverted);
    size = 10;
    crc[4] = packetCRC.ComputeCRC(&data1[inputInverted ? 15 : 0], size, inputInverted);
    crc[5] = packetCRC.ComputeCRC(&data2[inputInverted ? 15 : 0], size, inputInverted);
    crc[6] = packetCRC.ComputeCRC(&data3[inputInverted ? 15 : 0], size, inputInverted);
    crc[7] = packetCRC.ComputeCRC(&data4[inputInverted ? 15 : 0], size, inputInverted);
    bool ok = true;
    uint32 i;
    for (i = 0; (i < 8) && (ok); i++) {
        ok = (crc[i] == expectedCRC[i]);
    }
    return ok;
}

bool PacketCRCTest::TestComputableTable_CRC_0() {
    using namespace MARTe;
    uint8 data1[16] = { 0xF, 0x0, 0xF, 0x0, 0xAA, 0x55, 0xAA, 0x55, 0xBB, 0xDD, 0xFF, 0xEE, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8 data2[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
    uint8 data3[16] = { 0x1, 0xFF, 0x2, 0xFE, 0x3, 0xFD, 0x4, 0xFC, 0x5, 0xFB, 0x6, 0xFA, 0x7, 0xEF, 0x8, 0xEE };
    uint8 data4[16] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

    bool ok = TestComputeCRC0(0x589, 0x0, false, &data1[0]);
    ok &= TestComputeCRC0(0x589, 0x0, false, &data2[0]);
    ok &= TestComputeCRC0(0x589, 0x0, false, &data3[0]);
    ok &= TestComputeCRC0(0x589, 0x0, false, &data4[0]);

    return ok;
}

bool PacketCRCTest::TestComputableTable_CRC_0_inverted() {
    using namespace MARTe;
    uint8 data1[16] = { 0xF, 0x0, 0xF, 0x0, 0xAA, 0x55, 0xAA, 0x55, 0xBB, 0xDD, 0xFF, 0xEE, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8 data2[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
    uint8 data3[16] = { 0x1, 0xFF, 0x2, 0xFE, 0x3, 0xFD, 0x4, 0xFC, 0x5, 0xFB, 0x6, 0xFA, 0x7, 0xEF, 0x8, 0xEE };
    uint8 data4[16] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

    bool ok = TestComputeCRC0(0x589, 0x0, true, &data1[0]);
    ok &= TestComputeCRC0(0x589, 0x0, true, &data2[0]);
    ok &= TestComputeCRC0(0x589, 0x0, true, &data3[0]);
    ok &= TestComputeCRC0(0x589, 0x0, true, &data4[0]);

    return ok;
}

bool PacketCRCTest::TestComputableTable_CRC_0_initialCRC() {
    using namespace MARTe;
    uint8 data1[16] = { 0xF, 0x0, 0xF, 0x0, 0xAA, 0x55, 0xAA, 0x55, 0xBB, 0xDD, 0xFF, 0xEE, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8 data2[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
    uint8 data3[16] = { 0x1, 0xFF, 0x2, 0xFE, 0x3, 0xFD, 0x4, 0xFC, 0x5, 0xFB, 0x6, 0xFA, 0x7, 0xEF, 0x8, 0xEE };
    uint8 data4[16] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

    bool ok = TestComputeCRC0(0x589, 0x0A, false, &data1[0]);
    ok &= TestComputeCRC0(0x589, 0x0B, false, &data2[0]);
    ok &= TestComputeCRC0(0x589, 0x0C, false, &data3[0]);
    ok &= TestComputeCRC0(0x589, 0x0D, false, &data4[0]);

    return ok;
}

bool PacketCRCTest::TestComputableTable_CRC_0_inverted_initialCRC() {
    using namespace MARTe;
    uint8 data1[16] = { 0xF, 0x0, 0xF, 0x0, 0xAA, 0x55, 0xAA, 0x55, 0xBB, 0xDD, 0xFF, 0xEE, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8 data2[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
    uint8 data3[16] = { 0x1, 0xFF, 0x2, 0xFE, 0x3, 0xFD, 0x4, 0xFC, 0x5, 0xFB, 0x6, 0xFA, 0x7, 0xEF, 0x8, 0xEE };
    uint8 data4[16] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

    bool ok = TestComputeCRC0(0x589, 0xA0, true, &data1[0]);
    ok &= TestComputeCRC0(0x589, 0xB0, true, &data2[0]);
    ok &= TestComputeCRC0(0x589, 0xC0, true, &data3[0]);
    ok &= TestComputeCRC0(0x589, 0xD0, true, &data4[0]);

    return ok;
}

bool PacketCRCTest::TestComputeCRC0(MARTe::uint16 pol, MARTe::uint16 initialCRC, bool inputInverted, MARTe::uint8 *data) {
    using namespace MARTe;

    PacketCRC packetCRC;
    packetCRC.SetInitialCRC(initialCRC);
    packetCRC.ComputeTable(pol);
    uint32 size = 14;
    uint16 crc;
    uint8 *pcrc = reinterpret_cast<uint8 *>(&crc);
    crc = packetCRC.ComputeCRC(&data[inputInverted ? 15 : 0], size, inputInverted);
    if (inputInverted) {
        data[0] = pcrc[0];
        data[1] = pcrc[1];
    }
    else {
        data[14] = pcrc[1];
        data[15] = pcrc[0];
    }
    size = 16;
    crc = packetCRC.ComputeCRC(&data[inputInverted ? 15 : 0], size, inputInverted);

    return (crc == 0);
}

