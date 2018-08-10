/**
 * @file CRIODataSourceTest.cpp
 * @brief Source file for class CRIODataSourceTest
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
 * the class CRIODataSourceTest (public, protected, and private). Be aware that some 
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
#include "PacketGAM.h"
#include "PacketGAMTest.h"
#include "RealTimeApplication.h"
#include "StandardParser.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
/**
 * Gives access to the PacketGAM memory for the Execute test
 */
class PacketGAMHelper: public PacketGAM {
public:
    CLASS_REGISTER_DECLARATION()PacketGAMHelper() : PacketGAM() {
    }

    virtual ~PacketGAMHelper() {

    }

    void *GetInputSignalsMemory() {
        return PacketGAM::GetInputSignalsMemory();
    }

    void *GetOutputSignalMemory(MARTe::uint32 idx) {
        return PacketGAM::GetOutputSignalMemory(idx);
    }

    void *GetOutputSignalsMemory() {
        return PacketGAM::GetOutputSignalsMemory();
    }
};
CLASS_REGISTER(PacketGAMHelper, "1.0");

/**
 * A dummy PacketGAM which can be used to test different configuration interfaces
 *  to the PacketGAM
 */
class PacketGAMDataSourceHelper: public MARTe::MemoryDataSourceI {
public:
    CLASS_REGISTER_DECLARATION()

PacketGAMDataSourceHelper    () : MARTe::MemoryDataSourceI() {
    }

    virtual ~PacketGAMDataSourceHelper() {

    }

    virtual const MARTe::char8 *GetBrokerName(MARTe::StructuredDataI &data,
            const MARTe::SignalDirection direction) {
        if (direction == MARTe::InputSignals) {
            return "MemoryMapInputBroker";
        }
        return "MemoryMapOutputBroker";
    }

    virtual bool PrepareNextState(const MARTe::char8 * const currentStateName,
            const MARTe::char8 * const nextStateName) {
        return true;
    }

    virtual bool Synchronise() {
        return true;
    }

};

CLASS_REGISTER(PacketGAMDataSourceHelper, "1.0");

/**
 * Starts a MARTe application that uses this GAM instance.
 */
static bool TestIntegratedInApplication(const MARTe::char8 * const config, bool destroy = true) {
    using namespace MARTe;

    ConfigurationDatabase cdb;
    StreamString configStream = config;
    configStream.Seek(0);
    StandardParser parser(configStream, cdb);

    bool ok = parser.Parse();

    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();

    if (ok) {
        god->Purge();
        ok = god->Initialise(cdb);
    }
    ReferenceT<RealTimeApplication> application;
    if (ok) {
        application = god->Find("Test");
        ok = application.IsValid();
    }
    if (ok) {
        ok = application->ConfigureApplication();
    }

    if (destroy) {
        god->Purge();
    }
    return ok;
}

#define START_HVPS_BIT 79 
#define MODULATE_BIT 78
#define RESET_HVPS_BIT 77
#define EU_ARC_BIT 76
#define RU_ARC_BIT 75
#define PLC_TRIP_1_BIT 74
#define PLC_TRIP_2_BIT 73
#define START_TIME_BIT 72
#define CRITICAL_ERR_BIT 71
#define ADMIN_MODE_BIT 70
#define MODE_LOSS_BIT 69
#define STRAY_1_BIT 68
#define STRAY_2_BIT 67
#define RESET_FPGA_BIT 66
#define PRE_PROC_OUT_EU_ARC_BIT 65
#define PRE_PROC_OUT_RU_ARC_BIT 64
#define PRE_PROC_OUT_PLC_TRIP_1_BIT 63
#define PRE_PROC_OUT_PLC_TRIP_2_BIT 62
#define PRE_PROC_OUT_MODE_LOSS_BIT 61
#define PRE_PROC_OUT_STRAY_1_BIT 60
#define PRE_PROC_OUT_STRAY_2_BIT 59
#define PRE_PROC_OUT_L_EU_ARC_BIT 58
#define PRE_PROC_OUT_L_RU_ARC_BIT 57
#define PRE_PROC_OUT_L_PLC_TRIP_1_BIT 56
#define PRE_PROC_OUT_L_PLC_TRIP_2_BIT 55
#define PRE_PROC_OUT_L_MODE_LOSS_BIT 54
#define PRE_PROC_OUT_L_STRAY_1_BIT 53
#define PRE_PROC_OUT_L_STRAY_2_BIT 52
#define PRE_PROC_OUT_B_EU_ARC_BIT 51
#define PRE_PROC_OUT_B_RU_ARC_BIT 50
#define PRE_PROC_OUT_B_PLC_TRIP_1_BIT 49
#define PRE_PROC_OUT_B_PLC_TRIP_2_BIT 48
#define PRE_PROC_OUT_B_MODE_LOSS_BIT 47
#define PRE_PROC_OUT_B_STRAY_1_BIT 46
#define PRE_PROC_OUT_B_STRAY_2_BIT 45
#define ADHOC_TRIP_OUT_BIT 44
#define OUT_FLS_BIT 43
#define HVPS_OUT_TRIP_OUT_BIT 42
#define HVPS_OUT_ON_MPS_BIT 41
#define HVPS_OUT_ON_BPS_BIT 40
#define HVPS_STATE_OUT_BIT 39
#define OUT_IF_TRIP_PLC_BIT 38
#define OUT_IF_TRIP_MPS_BIT 37
#define OUT_IF_TRIP_BPS_BIT 36
#define FLS_MANAGER_STATE_OUT_BIT 35
#define ERROR_CODE_VALUE_BIT 34
#define LOGIC_MODE_VALUE_BIT 33
#define DEBUG_PACKET_COUNTER_BIT 32
#define DEBUG_SYNCH_BYTE_BIT 31

class CRIOFPSPacket {
public:

    CRIOFPSPacket() {
        packet = NULL_PTR(MARTe::uint8 *);
        packetCRC = NULL_PTR(PacketCRC *);
    }

    CRIOFPSPacket(bool initPacketMemory) {
        if (initPacketMemory) {
            packet = new MARTe::uint8[15];
            packetCRC = new PacketCRC();
            MARTe::uint32 i;
            for (i = 0; i < 15u; i++) {
                packet[i] = 0u;
            }
            packetCRC->SetInitialCRC(0xFFFF);
            packetCRC->ComputeTable(0x3D65);
            ComputeCRC();
        }
        else {
            packet = NULL_PTR(MARTe::uint8 *);
            packetCRC = NULL_PTR(PacketCRC *);
        }
    }

    virtual ~CRIOFPSPacket() {
        if (packet != NULL_PTR(MARTe::uint8 *)) {
            delete[] packet;
        }
        if (packetCRC != NULL_PTR(PacketCRC *)) {
            delete packetCRC;
        }
    }

    void SetPacket(MARTe::uint8 *p, bool autoComputeCRC = true) {
        if (packet != NULL_PTR(MARTe::uint8 *)) {
            delete[] packet;
        }
        if (packetCRC != NULL_PTR(PacketCRC *)) {
            delete packetCRC;
        }
        packet = new MARTe::uint8[15];
        packetCRC = new PacketCRC();
        MARTe::uint32 i;
        for (i = 0; i < 15u; i++) {
            packet[i] = p[i];
        }
        packetCRC->SetInitialCRC(0xFFFF);
        packetCRC->ComputeTable(0x3D65);
        if (autoComputeCRC) {
            ComputeCRC();
        }
    }

    void ComputeCRC() {
        SetCRC(packetCRC->ComputeCRC(&packet[14], 13, true));
    }

    void SetBit(MARTe::uint8 bit) {
        using namespace MARTe;
        uint8 byteN = bit / 8;
        uint8 byteR = bit % 8;
        packet[byteN] |= (1 << byteR);
    }

    void UnsetBit(MARTe::uint8 bit) {
        using namespace MARTe;
        uint8 byteN = bit / 8;
        uint8 byteR = bit % 8;
        packet[byteN] &= ~(1 << byteR);
    }

    bool IsBitSet(MARTe::uint8 bit) {
        using namespace MARTe;
        uint8 byteN = bit / 8;
        uint8 byteR = bit % 8;
        bool isSet = (((packet[byteN] >> byteR) & 0x1) == 1);
        return isSet;
    }

    MARTe::uint8 GetBitValue(MARTe::uint8 bit) {
        using namespace MARTe;
        uint8 byteN = bit / 8;
        uint8 byteR = bit % 8;
        uint8 ret = ((packet[byteN] >> byteR) & 0x1);
        return ret;
    }

    void SetCRC(MARTe::uint16 crc) {
        MARTe::uint8 *crcPtr = reinterpret_cast<MARTe::uint8 *>(&crc);
        packet[1u] = crcPtr[1u];
        packet[0u] = crcPtr[0u];
    }

    void SetTime(MARTe::uint64 timeIn) {
        MARTe::uint8 *timePtr = reinterpret_cast<MARTe::uint8 *>(&timeIn);
        packet[14u] = timePtr[4u];
        packet[13u] = timePtr[3u];
        packet[12u] = timePtr[2u];
        packet[11u] = timePtr[1u];
        packet[10u] = timePtr[0u];
    }

    MARTe::uint64 GetTime() {
        MARTe::uint64 ret = 0u;
        MARTe::uint8 *retPtr = reinterpret_cast<MARTe::uint8 *>(&ret);
        retPtr[4u] = packet[14u];
        retPtr[3u] = packet[13u];
        retPtr[2u] = packet[12u];
        retPtr[1u] = packet[11u];
        retPtr[0u] = packet[10u];

        return ret;
    }

    MARTe::uint16 GetCRC() {
        MARTe::uint16 ret = 0u;
        MARTe::uint8 *retPtr = reinterpret_cast<MARTe::uint8 *>(&ret);
        retPtr[1u] = packet[1u];
        retPtr[0u] = packet[0u];

        return ret;
    }

    MARTe::uint8 GetSynchByte() {
        return packet[2u];
    }

    MARTe::uint8 GetPacketCounter() {
        return (packet[3u] & 0x3);
    }

    void Copy(MARTe::uint8 *output) {
        MARTe::int32 i;
        for (i = 0; i < 15; i++) {
            output[i] = packet[i];
        }
    }

    void Print() {
        MARTe::int32 i;
        for (i = 0; i < 15; i++) {
            printf("0x%02x ", packet[i]);
        }
        printf("\n");
    }

private:
    MARTe::uint8 *packet;
    PacketCRC *packetCRC;

};

static bool CheckPacket(const MARTe::char8 * const config, CRIOFPSPacket *packets, MARTe::uint32 numberOfPackets) {
    using namespace MARTe;
    bool ok = TestIntegratedInApplication(config, false);
    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    ReferenceT<PacketGAMHelper> gam = god->Find("Test.Functions.GAM1");
    if (ok) {
        ok = gam.IsValid();
    }
    uint32 numberOfOutputSignals = 0u;
    if (ok) {
        numberOfOutputSignals = gam->GetNumberOfOutputSignals();
    }

    uint32 hpvsStateOutIdx;
    if (ok) {
        ok = gam->GetSignalIndex(OutputSignals, hpvsStateOutIdx, "HVPS_STATE_OUT");
    }
    uint32 fpgaErrorCodeIdx;
    if (ok) {
        ok = gam->GetSignalIndex(OutputSignals, fpgaErrorCodeIdx, "ERROR_CODE_VALUE");
    }
    uint32 flsManagerStateOutIdx;
    if (ok) {
        ok = gam->GetSignalIndex(OutputSignals, flsManagerStateOutIdx, "FLS_MANAGER_STATE_OUT");
    }

    uint32 k;
    for (k = 0; k < numberOfPackets; k++) {
        if (ok) {
            uint8 *gamInputMemory = reinterpret_cast<uint8 *>(gam->GetInputSignalsMemory());
            packets[k].Copy(gamInputMemory);

            uint32 i;
            for (i = 0; (i < numberOfOutputSignals); i++) {
                uint8 *outMem = static_cast<uint8 *>(gam->GetOutputSignalMemory(i));
                uint32 nBytes = 0u;
                ok = gam->GetSignalByteSize(OutputSignals, i, nBytes);
                uint32 n;
                for (n = 0; n < nBytes; n++) {
                    outMem[n] = 0;
                }
            }
        }
        if (ok) {
            ok = gam->Execute();
        }
        if (ok) {
            packets[k].Print();
            uint64 *outputTime = reinterpret_cast<uint64 *>(gam->GetOutputSignalMemory(0u));
            ok = (*outputTime == packets[k].GetTime());
            if (!ok) {
                printf("Invalid time got: 0x%llx expected 0x%llx\n", *outputTime, packets[k].GetTime());
            }
            uint32 n;
            //Do not crc, packet counter, synch byte, trigger, marte error code and raw packet
            uint32 signalsToCheck = gam->GetNumberOfOutputSignals() - 6u;
            //Do not check time => n=1
            n = 1;
            uint8 bit = 79;
            while ((n < signalsToCheck) && (ok)) {
                StreamString signalName;
                gam->GetSignalName(OutputSignals, n, signalName);
                //printf("Checking bit [%s] against output signal [%d] \n", signalName.Buffer(), n);

                uint8 *outputSignalPtr = reinterpret_cast<uint8 *>(gam->GetOutputSignalMemory(n));
                if (n == hpvsStateOutIdx) {
                    uint8 expectedValue = 0;
                    expectedValue |= packets[k].GetBitValue(37) << 2;
                    expectedValue |= packets[k].GetBitValue(38) << 1;
                    expectedValue |= packets[k].GetBitValue(39);
                    ok = (expectedValue == *outputSignalPtr);
                    bit -= 3;
                    if (!ok) {
                        printf("Checking hpvsStateOutIdx failed (%s) [%d] %d vs %d\n", signalName.Buffer(), n, expectedValue, *outputSignalPtr);
                    }
                }
                else if (n == fpgaErrorCodeIdx) {
                    uint8 expectedValue = 0;
                    expectedValue |= packets[k].GetBitValue(27) << 3;
                    expectedValue |= packets[k].GetBitValue(28) << 2;
                    expectedValue |= packets[k].GetBitValue(29) << 1;
                    expectedValue |= packets[k].GetBitValue(30);
                    ok = (expectedValue == *outputSignalPtr);
                    if (!ok) {
                        printf("Checking fpgaErrorCodeIdx failed (%s) [%d] %d vs %d\n", signalName.Buffer(), n, expectedValue, *outputSignalPtr);
                    }
                    bit -= 4;
                }
                else if (n == flsManagerStateOutIdx) {
                    uint8 expectedValue = 0;
                    expectedValue |= packets[k].GetBitValue(31) << 2;
                    expectedValue |= packets[k].GetBitValue(32) << 1;
                    expectedValue |= packets[k].GetBitValue(33);
                    ok = (expectedValue == *outputSignalPtr);
                    bit -= 3;
                    if (!ok) {
                        printf("Checking flsManagerStateOutIdx failed (%s) [%d] %d vs %d\n", signalName.Buffer(), n, expectedValue, *outputSignalPtr);
                    }
                }
                else {
                    ok = (packets[k].GetBitValue(bit) == *outputSignalPtr);
                    if (!ok) {
                        printf("Checking bit [%d] against output signal (%s) [%d] %d vs %d\n", bit, signalName.Buffer(), n, packets[k].GetBitValue(bit), *outputSignalPtr);
                    }
                    bit--;
                }
                n++;
            }
            uint8 *packetCounter = reinterpret_cast<uint8 *>(gam->GetOutputSignalMemory(n));
            if (ok) {
                ok = (*packetCounter == packets[k].GetPacketCounter());
                if (!ok) {
                    printf("Invalid packet counter got: 0x%x expected 0x%x\n", *packetCounter, packets[k].GetPacketCounter());
                }
            }
            uint8 *synchByte = reinterpret_cast<uint8 *>(gam->GetOutputSignalMemory(n + 1));
            if (ok) {
                ok = (*synchByte == packets[k].GetSynchByte());
                if (!ok) {
                    printf("Invalid synch byte got: 0x%x expected 0x%x\n", *synchByte, packets[k].GetSynchByte());
                }
            }

            uint16 *crc = reinterpret_cast<uint16 *>(gam->GetOutputSignalMemory(n + 2));
            if (ok) {
                ok = (*crc == packets[k].GetCRC());
                if (!ok) {
                    printf("Invalid synch byte got: 0x%x expected 0x%x\n", *crc, packets[k].GetCRC());
                }
            }
        }
    }
    return ok;
}

static bool CheckPacketWithError(const MARTe::char8 * const config, CRIOFPSPacket *packet, MARTe::uint32 numberOfPackets, MARTe::uint8 *expectedTrigger, MARTe::uint8 *expectedErrorCode) {
    using namespace MARTe;
    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    bool ok = TestIntegratedInApplication(config, false);
    ReferenceT<PacketGAMHelper> gam = god->Find("Test.Functions.GAM1");
    if (ok) {
        ok = gam.IsValid();
    }
    uint32 triggerIdx;
    uint8 *trigger;
    if (ok) {
        ok = gam->GetSignalIndex(OutputSignals, triggerIdx, "TRIGGER");
    }
    if (ok) {
        trigger = reinterpret_cast<uint8 *>(gam->GetOutputSignalMemory(triggerIdx));
    }
    uint32 marteErrorCodeIdx;
    uint8 *marteErrorCode;
    if (ok) {
        ok = gam->GetSignalIndex(OutputSignals, marteErrorCodeIdx, "MARTE_ERROR_CODE");
    }
    if (ok) {
        marteErrorCode = reinterpret_cast<uint8 *>(gam->GetOutputSignalMemory(marteErrorCodeIdx));
    }

    uint32 k;
    for (k = 0; (k < numberOfPackets) && (ok); k++) {
        if (ok) {
            uint8 *gamInputMemory = reinterpret_cast<uint8 *>(gam->GetInputSignalsMemory());
            packet[k].Copy(gamInputMemory);
        }
        if (ok) {
            ok = gam->Execute();
        }
        if (ok) {
            ok = (*trigger == expectedTrigger[k]);
        }
        if (ok) {
            ok = (*marteErrorCode == expectedErrorCode[k]);
        }
    }
    return ok;
}

//Standard configuration
using namespace MARTe;
const MARTe::char8 * const config1 = ""
        "$Test = {"
        "    Class = RealTimeApplication"
        "    +Functions = {"
        "        Class = ReferenceContainer"
        "        +GAM1 = {"
        "            Class = PacketGAMHelper"
        "            InputSignals = {"
        "               Packet = {"
        "                   DataSource = Drv1"
        "                   Type = uint8"
        "                   NumberOfElements = 5"
        "               }"
        "            }"
        "            OutputSignals = {"
        "               TIME = {"
        "                   DataSource = DDB1"
        "                   Type = uint64"
        "               }"
        "               START_HVPS = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               MODULATE = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               RESET_HVPS = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               EU_ARC = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               RU_ARC = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PLC_TRIP_1 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PLC_TRIP_2 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               START_TIME = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               CRITICAL_ERR = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               ADMIN_MODE = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               MODE_LOSS = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               STRAY_1 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               STRAY_2 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               RESET_FPGA = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_EU_ARC = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_RU_ARC = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_PLC_TRIP_1 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_PLC_TRIP_2 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_MODE_LOSS = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_STRAY_1 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_STRAY_2 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_L_EU_ARC = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_L_RU_ARC = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_L_PLC_TRIP_1 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_L_PLC_TRIP_2 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_L_MODE_LOSS = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_L_STRAY_1 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_L_STRAY_2 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_B_EU_ARC = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_B_RU_ARC = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_B_PLC_TRIP_1 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_B_PLC_TRIP_2 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_B_MODE_LOSS = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_B_STRAY_1 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               PRE_PROC_OUT_B_STRAY_2 = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               ADHOC_TRIP_OUT = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               OUT_FLS = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               HVPS_OUT_TRIP_OUT = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               HVPS_OUT_ON_MPS = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               HVPS_OUT_ON_BPS = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               HVPS_STATE_OUT = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               OUT_IF_TRIP_PLC = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               OUT_IF_TRIP_MPS = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               OUT_IF_TRIP_BPS = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               FLS_MANAGER_STATE_OUT = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               ERROR_CODE_VALUE = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               LOGIC_MODE_VALUE = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               DEBUG_PACKET_COUNTER = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               DEBUG_SYNCH_BYTE = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               DEBUG_CRC = {"
        "                   DataSource = DDB1"
        "                   Type = uint16"
        "               }"
        "               TRIGGER = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               MARTE_ERROR_CODE = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "               }"
        "               ORIGINAL_PACKET = {"
        "                   DataSource = DDB1"
        "                   Type = uint8"
        "                   NumberOfElements = 15"
        "               }"
        "            }"
        "        }"
        "    }"
        "    +Data = {"
        "        Class = ReferenceContainer"
        "        DefaultDataSource = DDB1"
        "        +DDB1 = {"
        "            Class = GAMDataSource"
        "        }"
        "        +Timings = {"
        "            Class = TimingDataSource"
        "        }"
        "        +Drv1 = {"
        "            Class = PacketGAMDataSourceHelper"
        "        }"
        "    }"
        "    +States = {"
        "        Class = ReferenceContainer"
        "        +State1 = {"
        "            Class = RealTimeState"
        "            +Threads = {"
        "                Class = ReferenceContainer"
        "                +Thread1 = {"
        "                    Class = RealTimeThread"
        "                    Functions = {GAM1}"
        "                }"
        "            }"
        "        }"
        "    }"
        "    +Scheduler = {"
        "        Class = GAMScheduler"
        "        TimingDataSource = Timings"
        "    }"
        "}";

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

bool PacketGAMTest::TestConstructor() {
    using namespace MARTe;

    PacketGAM gam;
    uint8 mem[3][15] = { { 0x0D, 0x48, 0x55, 0x00, 0x80, 0x1C, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10 }, { 0xA5, 0x1E, 0xAA, 0x01, 0x60, 0x1C, 0xF0, 0xFF, 0x03, 0x00, 0x01, 0x00, 0x00,
            0x01, 0x00 }, { 0xD7, 0x6C, 0x55, 0x02, 0x60, 0x1C, 0xF0, 0xFF, 0x43, 0x00, 0x5C, 0x8D, 0xCA, 0x12, 0x00 } };
    uint32 k;
    for (k = 0; k < 3; k++) {
        CRIOFPSPacket packet;
        packet.SetPacket(mem[k], true);
        packet.Print();
    }

    return true;
}

bool PacketGAMTest::TestExecute() {

    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    bool ok = true;
    const uint32 numberOfPackets = 30;
    uint8 packetTestDefinition[numberOfPackets][15] = { { 0x0D, 0x48, 0x55, 0x00, 0x80, 0x1C, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, { 0x02, 0x64, 0xAA, 0x01, 0x80, 0x1C, 0xF0, 0xFF,
            0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 }, { 0x87, 0x76, 0x55, 0x02, 0x80, 0x18, 0xF0, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00 }, { 0x35, 0x46, 0xAA, 0x03, 0x60, 0x1C, 0xF0, 0xFF,
            0x03, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00 }, { 0x92, 0xE0, 0x55, 0x00, 0x60, 0x1C, 0xF0, 0xFF, 0x43, 0x00, 0x5C, 0x8D, 0xCA, 0x12, 0x00 }, { 0x6B, 0x94, 0xAA, 0x01, 0x60, 0x1C, 0xF0, 0xFF,
            0x03, 0x00, 0x24, 0xAF, 0x05, 0x14, 0x00 }, { 0x57, 0x5E, 0x55, 0x02, 0x60, 0x1C, 0xF0, 0xFF, 0x07, 0x00, 0x4A, 0xF0, 0xDC, 0x15, 0x00 }, { 0x74, 0x46, 0xAA, 0x03, 0x60, 0x1C, 0xF0, 0xFF,
            0x03, 0x00, 0x75, 0x00, 0x1A, 0x17, 0x00 }, { 0x4D, 0x04, 0x55, 0x00, 0x80, 0x1C, 0xF0, 0xFF, 0x03, 0x00, 0x78, 0x00, 0x1A, 0x17, 0x00 }, { 0xD7, 0x04, 0xAA, 0x01, 0x80, 0x18, 0xF0, 0xFF,
            0x03, 0x00, 0x77, 0x00, 0x1A, 0x18, 0x00 }, { 0xEB, 0xB6, 0x55, 0x02, 0x60, 0x1C, 0xF0, 0xFF, 0x03, 0x00, 0x78, 0x00, 0x1A, 0x18, 0x00 }, { 0xAA, 0xA3, 0xAA, 0x03, 0x60, 0x1C, 0xF0, 0xFF,
            0x43, 0x00, 0x4C, 0x9E, 0xF6, 0x18, 0x00 }, { 0x92, 0xE2, 0x55, 0x00, 0x62, 0x1C, 0xF0, 0xFF, 0x43, 0x00, 0x78, 0x01, 0x5E, 0x1B, 0x00 }, { 0xFE, 0xCA, 0xAA, 0x01, 0x61, 0x1C, 0xF0, 0xFF,
            0x43, 0x00, 0xCA, 0x02, 0x1D, 0x1D, 0x00 }, { 0x21, 0xC6, 0x55, 0x02, 0x61, 0x04, 0xF0, 0xFF, 0x43, 0x00, 0xCB, 0x02, 0x1D, 0x1D, 0x00 }, { 0x79, 0x20, 0xAA, 0x03, 0x61, 0x04, 0x00, 0x00,
            0x40, 0x00, 0xCC, 0x02, 0x1D, 0x1D, 0x00 }, { 0xAE, 0xBB, 0x55, 0x00, 0x61, 0x04, 0xF0, 0xFF, 0x43, 0x00, 0xCE, 0x02, 0x1D, 0x1D, 0x00 }, { 0x59, 0x2B, 0xAA, 0x01, 0x61, 0x04, 0xF0, 0xFF,
            0x43, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00 }, { 0x8F, 0x7A, 0x55, 0x02, 0x61, 0x04, 0xF0, 0xFF, 0x43, 0x00, 0x97, 0xD9, 0xD6, 0x01, 0x00 }, { 0x36, 0xD5, 0xAA, 0x03, 0x61, 0x04, 0xF0, 0xFF,
            0x43, 0x20, 0xAD, 0x90, 0xB4, 0x03, 0x00 }, { 0x94, 0x2D, 0x55, 0x00, 0x81, 0x00, 0xF0, 0xFF, 0x43, 0x20, 0xAE, 0x90, 0xB4, 0x03, 0x00 }, { 0x0B, 0xA8, 0xAA, 0x01, 0x9D, 0x00, 0xF0, 0xFF,
            0x43, 0x20, 0xAF, 0x90, 0xB4, 0x03, 0x00 }, { 0xF3, 0x9B, 0x55, 0x02, 0x9D, 0x00, 0xF0, 0xFF, 0x43, 0x00, 0x65, 0x92, 0x91, 0x05, 0x00 }, { 0x17, 0x43, 0xAA, 0x03, 0x9D, 0x00, 0xF0, 0xFF,
            0x43, 0x80, 0x6B, 0xDB, 0xCD, 0x06, 0x00 }, { 0x4F, 0xB3, 0x55, 0x00, 0x5D, 0x02, 0xF0, 0xFF, 0x43, 0x80, 0x70, 0xDB, 0xCD, 0x06, 0x00 }, { 0xC2, 0x0B, 0xAA, 0x01, 0xDD, 0x03, 0xF0, 0xFF,
            0x43, 0x80, 0x31, 0x8C, 0x6A, 0x07, 0x00 }, { 0x54, 0xC9, 0x55, 0x02, 0xDD, 0x03, 0xF0, 0xFF, 0x43, 0xC0, 0x34, 0xCD, 0x7F, 0x0A, 0x00 }, { 0xE2, 0x8C, 0xAA, 0x03, 0x3D, 0x03, 0xF0, 0xFF,
            0x43, 0xC0, 0x1A, 0xD7, 0x1F, 0x0B, 0x00 }, { 0x9B, 0x5E, 0x55, 0x00, 0xDD, 0x03, 0xF0, 0xFF, 0x43, 0xC0, 0x61, 0xBF, 0x21, 0x0B, 0x00 }, { 0x47, 0x88, 0xAA, 0x01, 0x3D, 0x03, 0xF0, 0xFF,
            0x43, 0xC0, 0xAA, 0xA7, 0x23, 0x0B, 0x00 } };

    CRIOFPSPacket *packets = new CRIOFPSPacket[numberOfPackets];
    uint32 i;
    for (i = 0; (i < numberOfPackets) && (ok); i++) {
        packets[i].SetPacket(packetTestDefinition[i]);
    }
    if (ok) {
        ok = CheckPacket(config1, packets, numberOfPackets);
    }

    delete[] packets;
    god->Purge();
    return ok;
}

bool PacketGAMTest::TestExecute_FirstTime_NotZero() {
    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    bool ok = true;
    const uint32 numberOfPackets = 3;
    uint8 packetTestDefinition[numberOfPackets][15] = {
            { 0x54, 0xC9, 0x55, 0x02, 0xDD, 0x03, 0xF0, 0xFF, 0x43, 0xC0, 0x34, 0xCD, 0x7F, 0x0A, 0x00 },
            { 0xE2, 0x8C, 0xAA, 0x03, 0x3D, 0x03, 0xF0, 0xFF, 0x43, 0xC0, 0x1A, 0xD7, 0x1F, 0x0B, 0x00 },
            { 0x9B, 0x5E, 0x55, 0x00, 0xDD, 0x03, 0xF0, 0xFF, 0x43, 0xC0, 0x61, 0xBF, 0x21, 0x0B, 0x00 } };

    uint8 expectedTrigger[] = { 1, 1, 1 };
    uint8 expectedErrorCodes[] = { 0x0, 0x0, 0x0 };

    CRIOFPSPacket *packets = new CRIOFPSPacket[numberOfPackets];
    uint32 i;
    for (i = 0; (i < numberOfPackets) && (ok); i++) {
        packets[i].SetPacket(packetTestDefinition[i], false);
    }
    if (ok) {
        ok = CheckPacketWithError(config1, packets, numberOfPackets, expectedTrigger, expectedErrorCodes);
    }

    delete[] packets;
    god->Purge();
    return ok;
}

bool PacketGAMTest::TestExecute_CRC() {
    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    bool ok = true;
    const uint32 numberOfPackets = 3;
    uint8 packetTestDefinition[numberOfPackets][15] = { { 0x0D, 0x48, 0x55, 0x00, 0x80, 0x1C, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, { 0x02, 0x64, 0xAA, 0x02, 0x80, 0x1C, 0xF0, 0xFF,
            0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 }, { 0x87, 0x76, 0x55, 0x02, 0x80, 0x18, 0xF0, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00 } };

    uint8 expectedTrigger[] = { 1, 0, 1 };
    uint8 expectedErrorCodes[] = { 0x0, 0x1, 0x6 };

    CRIOFPSPacket *packets = new CRIOFPSPacket[numberOfPackets];
    uint32 i;
    for (i = 0; (i < numberOfPackets) && (ok); i++) {
        packets[i].SetPacket(packetTestDefinition[i], false);
    }
    if (ok) {
        ok = CheckPacketWithError(config1, packets, numberOfPackets, expectedTrigger, expectedErrorCodes);
    }

    delete[] packets;
    god->Purge();
    return ok;
}

bool PacketGAMTest::TestExecute_CRC_Synchronism() {
    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    bool ok = true;
    const uint32 numberOfPackets = 3;
    uint8 packetTestDefinition[numberOfPackets][15] = { { 0x0D, 0x48, 0x55, 0x00, 0x80, 0x1C, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, { 0x02, 0x64, 0x55, 0x02, 0x80, 0x1C, 0xF0, 0xFF,
            0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 }, { 0x87, 0x76, 0x55, 0x02, 0x80, 0x18, 0xF0, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00 } };

    uint8 expectedTrigger[] = { 1, 0, 1 };
    uint8 expectedErrorCodes[] = { 0x0, 0x1, 0x6 };

    CRIOFPSPacket *packets = new CRIOFPSPacket[numberOfPackets];
    uint32 i;
    for (i = 0; (i < numberOfPackets) && (ok); i++) {
        packets[i].SetPacket(packetTestDefinition[i], false);
    }
    if (ok) {
        ok = CheckPacketWithError(config1, packets, numberOfPackets, expectedTrigger, expectedErrorCodes);
    }

    delete[] packets;
    god->Purge();
    return ok;
}

bool PacketGAMTest::TestExecute_CRC_FirstTime() {
    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    bool ok = true;
    const uint32 numberOfPackets = 3;
    uint8 packetTestDefinition[numberOfPackets][15] = { { 0x02, 0x64, 0xAA, 0x02, 0x80, 0x1C, 0xF0, 0xFF, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 }, { 0xE2, 0x8C, 0xAA, 0x03, 0x3D, 0x03, 0xF0, 0xFF,
            0x43, 0xC0, 0x1A, 0xD7, 0x1F, 0x0B, 0x00 }, { 0x9B, 0x5E, 0x55, 0x00, 0xDD, 0x03, 0xF0, 0xFF, 0x43, 0xC0, 0x61, 0xBF, 0x21, 0x0B, 0x00 } };

    uint8 expectedTrigger[] = { 0, 1, 1 };
    uint8 expectedErrorCodes[] = { 0x1, 0x0, 0x0 };

    CRIOFPSPacket *packets = new CRIOFPSPacket[numberOfPackets];
    uint32 i;
    for (i = 0; (i < numberOfPackets) && (ok); i++) {
        packets[i].SetPacket(packetTestDefinition[i], false);
    }
    if (ok) {
        ok = CheckPacketWithError(config1, packets, numberOfPackets, expectedTrigger, expectedErrorCodes);
    }

    delete[] packets;
    god->Purge();
    return ok;
}

bool PacketGAMTest::TestExecute_Packet_Counter() {
    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    bool ok = true;
    const uint32 numberOfPackets = 3;
    uint8 packetTestDefinition[numberOfPackets][15] = { { 0x0D, 0x48, 0x55, 0x00, 0x80, 0x1C, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, { 0x35, 0x46, 0xAA, 0x03, 0x60, 0x1C, 0xF0, 0xFF,
            0x03, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00 }, { 0x92, 0xE0, 0x55, 0x00, 0x60, 0x1C, 0xF0, 0xFF, 0x43, 0x00, 0x5C, 0x8D, 0xCA, 0x12, 0x00 } };

    uint8 expectedTrigger[] = { 1, 1, 1 };
    uint8 expectedErrorCodes[] = { 0x0, 0x2, 0x0 };

    CRIOFPSPacket *packets = new CRIOFPSPacket[numberOfPackets];
    uint32 i;
    for (i = 0; (i < numberOfPackets) && (ok); i++) {
        packets[i].SetPacket(packetTestDefinition[i], false);
    }
    if (ok) {
        ok = CheckPacketWithError(config1, packets, numberOfPackets, expectedTrigger, expectedErrorCodes);
    }

    delete[] packets;
    god->Purge();
    return ok;
}

bool PacketGAMTest::TestExecute_Invalid_Synchronism() {
    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    bool ok = true;
    const uint32 numberOfPackets = 3;
    uint8 packetTestDefinition[numberOfPackets][15] = { { 0x0D, 0x48, 0x55, 0x00, 0x80, 0x1C, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, { 0x02, 0x03, 0xA1, 0x01, 0x60, 0x1C, 0xF0, 0xFF,
            0x03, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00 }, { 0x92, 0xE0, 0x55, 0x00, 0x60, 0x1C, 0xF0, 0xFF, 0x43, 0x00, 0x5C, 0x8D, 0xCA, 0x12, 0x00 } };

    uint8 expectedTrigger[] = { 1, 0, 1 };
    uint8 expectedErrorCodes[] = { 0x0, 0x8, 0x6 };

    CRIOFPSPacket *packets = new CRIOFPSPacket[numberOfPackets];
    uint32 i;
    for (i = 0; (i < numberOfPackets) && (ok); i++) {
        packets[i].SetPacket(packetTestDefinition[i], false);
    }
    if (ok) {
        ok = CheckPacketWithError(config1, packets, numberOfPackets, expectedTrigger, expectedErrorCodes);
    }

    delete[] packets;
    god->Purge();
    return ok;
}

bool PacketGAMTest::TestExecute_Repeated_Synchronism() {
    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    bool ok = true;
    const uint32 numberOfPackets = 3;
    uint8 packetTestDefinition[numberOfPackets][15] = { { 0x0D, 0x48, 0x55, 0x00, 0x80, 0x1C, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, { 0xA5, 0x1E, 0x55, 0x02, 0x60, 0x1C, 0xF0, 0xFF,
            0x03, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00 }, { 0xD7, 0x6C, 0xAA, 0x03, 0x60, 0x1C, 0xF0, 0xFF, 0x43, 0x00, 0x5C, 0x8D, 0xCA, 0x12, 0x00 } };

    uint8 expectedTrigger[] = { 1, 1, 1 };
    uint8 expectedErrorCodes[] = { 0x0, 0x6, 0x0 };

    CRIOFPSPacket *packets = new CRIOFPSPacket[numberOfPackets];
    uint32 i;
    for (i = 0; (i < numberOfPackets) && (ok); i++) {
        packets[i].SetPacket(packetTestDefinition[i], false);
    }
    if (ok) {
        ok = CheckPacketWithError(config1, packets, numberOfPackets, expectedTrigger, expectedErrorCodes);
    }

    delete[] packets;
    god->Purge();
    return ok;
}

bool PacketGAMTest::TestExecute_InvalidTime() {
    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    bool ok = true;
    const uint32 numberOfPackets = 3;
    uint8 packetTestDefinition[numberOfPackets][15] = { { 0xCB, 0x1C, 0x55, 0x00, 0x80, 0x1C, 0x00, 0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10 }, { 0xE0, 0x92, 0xAA, 0x01, 0x60, 0x1C, 0xF0, 0xFF,
            0x03, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00 }, { 0x47, 0x34, 0x55, 0x02, 0x60, 0x1C, 0xF0, 0xFF, 0x43, 0x00, 0x5C, 0x8D, 0xCA, 0x12, 0x00 } };

    uint8 expectedTrigger[] = { 1, 1, 1 };
    uint8 expectedErrorCodes[] = { 0x0, 0x10, 0x0 };

    CRIOFPSPacket *packets = new CRIOFPSPacket[numberOfPackets];
    uint32 i;
    for (i = 0; (i < numberOfPackets) && (ok); i++) {
        packets[i].SetPacket(packetTestDefinition[i], false);
    }
    if (ok) {
        ok = CheckPacketWithError(config1, packets, numberOfPackets, expectedTrigger, expectedErrorCodes);
    }

    delete[] packets;
    god->Purge();
    return ok;
}
