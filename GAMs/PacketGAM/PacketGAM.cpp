/**
 * @file PacketGAM.cpp
 * @brief Source file for class PacketGAM
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
 * the class PacketGAM (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "PacketGAM.h"
#include "PacketCRC.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
PacketGAM::PacketGAM() {
    packetCRC.SetInitialCRC(0xFFFF);
    packetCRC.ComputeTable(0x3D65);
    lastPacketCounter = 0u;
    lastSynchronismByte = 0u;
    firstTime = false;
}

PacketGAM::~PacketGAM() {

}

bool PacketGAM::Setup() {
    using namespace MARTe;
    bool ok = (GetNumberOfInputSignals() == 1u);
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "The number of input signals shall be 1");
    }
    firstTime = true;
    return ok;
}

bool PacketGAM::Execute() {
    using namespace MARTe;
    bool ok = true;
    uint8 *packet = reinterpret_cast<uint8 *>(GetInputSignalMemory(0u));
    uint8 *timeSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(0u));

    timeSignal[4u] = packet[14u];
    timeSignal[3u] = packet[13u];
    timeSignal[2u] = packet[12u];
    timeSignal[1u] = packet[11u];
    timeSignal[0u] = packet[10u];

    //packet[9u] => From outputs of input interface
    uint32 byteNumber = 9u;
    uint32 outputSignalIdx = 1u;
    int32 endSignalIdx = 7;
    int32 i;
    for (i = endSignalIdx; i >= 0; i--) {
        uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
        *outputSignal = ((packet[byteNumber] >> i) & 0x1);
        outputSignalIdx++;
    }

    //packet[8u] => From outputs of input interface (6 bits) & From outputs of FLS input processing (2bits)
    byteNumber = 8u;
    for (i = endSignalIdx; i >= 0; i--) {
        uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
        *outputSignal = ((packet[byteNumber] >> i) & 0x1);
        outputSignalIdx++;
    }

    //packet[7u] => From outputs of FLS input processing (8bits)
    byteNumber = 7u;

    for (i = endSignalIdx; i >= 0; i--) {
        uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
        *outputSignal = ((packet[byteNumber] >> i) & 0x1);
        outputSignalIdx++;
    }

    //packet[6u] => From outputs of FLS input processing (8bits)
    byteNumber = 6u;

    for (i = endSignalIdx; i >= 0; i--) {
        uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
        *outputSignal = ((packet[byteNumber] >> i) & 0x1);
        outputSignalIdx++;
    }

    //packet[5u] => From outputs of FLS input processing (3bits) & OutAdHoc (1bit) & OutFLS(1bit)& OutHVPS (3bit)
    byteNumber = 5u;

    for (i = endSignalIdx; i >= 0; i--) {
        uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
        *outputSignal = ((packet[byteNumber] >> i) & 0x1);
        outputSignalIdx++;
    }

    //packet[4u] => Outputs HVPS (3bits) & Signals from output interface (3bit) & FLS_Man_State (2bit)
    byteNumber = 4u;
    uint8 *hvpsStateOut = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
    uint8 hvpsStateOutTemp = ((packet[byteNumber] >> 5) & 0x7);
    //3 bit order is inverted for hvpsStateOut!
    *hvpsStateOut = 0u;
    *hvpsStateOut |= ((hvpsStateOutTemp << 2u) & 0x4);
    *hvpsStateOut |= (hvpsStateOutTemp & 0x2);
    *hvpsStateOut |= ((hvpsStateOutTemp & 0x4) >> 2u);
    outputSignalIdx++;

    endSignalIdx = 4;
    for (i = endSignalIdx; i >= 2; i--) {
        uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
        *outputSignal = ((packet[byteNumber] >> i) & 0x1u);
        outputSignalIdx++;
    }

    // FLS State_Out (3 bits)
    //2 bits B4 & 1 bit B3
    uint8 *flsStateOut = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
    uint8 flsStateOutTemp = (packet[byteNumber] & 0x3u);
    flsStateOutTemp = flsStateOutTemp << 1u;
    byteNumber = 3u;
    flsStateOutTemp |= ((packet[byteNumber] >> 7u) & 0x1u);
    *flsStateOut = 0u;
    *flsStateOut |= ((flsStateOutTemp << 2u) & 0x4);
    *flsStateOut |= (flsStateOutTemp & 0x2);
    *flsStateOut |= ((flsStateOutTemp & 0x4) >> 2u);
    outputSignalIdx++;

    //FPGA error code (4 bits)
    uint8 *fpgaErrorCode = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
    *fpgaErrorCode = ((packet[byteNumber] >> 3u) & 0xf);
    outputSignalIdx++;

    //Logic mode (1 bit)
    uint8 *logicMode = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
    *logicMode = ((packet[byteNumber] >> 2u) & 0x1);
    outputSignalIdx++;

    //Packet sequence counter
    int8 *packetSequenceCounter = reinterpret_cast<int8 *>(GetOutputSignalMemory(outputSignalIdx));
    *packetSequenceCounter = (packet[byteNumber] & 0x3);
    outputSignalIdx++;

    //Packet sequence counter
    byteNumber = 2u;
    uint8 *synchronismByte = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
    *synchronismByte = packet[byteNumber];
    outputSignalIdx++;

    //CRC
    uint16 *crc = reinterpret_cast<uint16 *>(GetOutputSignalMemory(outputSignalIdx));
    uint8 *crb8b = reinterpret_cast<uint8 *>(crc);
    crb8b[1u] = packet[1u];
    crb8b[0u] = packet[0u];

    uint8 packetCodeError = 0u;
    bool fatalPacket = false;
    uint16 expectedCRC = packetCRC.ComputeCRC(&packet[14], 13, true);
    if (expectedCRC != *crc) {
        REPORT_ERROR(ErrorManagement::FatalError, "Invalid CRC detected. Expected: %x Read: %x", expectedCRC, *crc);
        fatalPacket = true;
        packetCodeError = 0x1;
    }

    //Check if we have lost a packet counter
    if (!fatalPacket) {
        int8 packetCounterDifference = (*packetSequenceCounter - lastPacketCounter);
        if ((packetCounterDifference != 1) && (packetCounterDifference != -3)) {
            if (!firstTime) {
                REPORT_ERROR(ErrorManagement::Warning, "Invalid packet counter difference detected. Expected: -1 or -3 and Read: %d", packetCounterDifference);
                packetCodeError |= 0x2;
            }
        }
        lastPacketCounter = *packetSequenceCounter;

        //Detect synchronism byte errors. It has to be 0x55 or 0xAA
        if ((*synchronismByte == 0x55) || (*synchronismByte == 0xAA)) {
            if (*synchronismByte == lastSynchronismByte) {
                if (!firstTime) {
                    //Packet repeated twice
                    REPORT_ERROR(ErrorManagement::Warning, "Synchronism byte repeated at least twice [%x]", *synchronismByte);
                    packetCodeError |= 0x4;
                }
            }
        }
        else {
            REPORT_ERROR(ErrorManagement::FatalError, "Invalid synchronism byte difference detected. Expected: 0x55 or 0xAA and Read: 0x%x", *synchronismByte);
            fatalPacket = true;
            packetCodeError = 0x8;
        }
        //Do not remember last value if there was a serious issue.
        if ((packetCodeError & 0x8) != 0x8) {
            lastSynchronismByte |= *synchronismByte;
        }
    }

    outputSignalIdx++;
    uint8 *trigger = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
    *trigger = !fatalPacket;

    outputSignalIdx++;
    uint8 *marteErrorCode = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
    *marteErrorCode = packetCodeError;

    firstTime = false;
    return ok;
}

CLASS_REGISTER(PacketGAM, "1.0")

