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
#include "PacketCRC.h"
#include "PacketGAM.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
PacketGAM::PacketGAM() : GAM() {
    packetCRC.SetInitialCRC(0xFFFFu);
    packetCRC.ComputeTable(0x3D65u);
    lastPacketCounter = 0;
    lastSynchronismByte = 0u;
    lastTime = 0u;
    outputMemorySize = 0u;
    firstTime = false;
}

PacketGAM::~PacketGAM() {

}

/*lint -e{9144} allow use of the using namespace directive inside a function*/
bool PacketGAM::CheckSignal(MARTe::uint32 signalIdx, const MARTe::SignalDirection direction, const MARTe::TypeDescriptor &expectedType, MARTe::uint32 expectedDimensions, MARTe::uint32 expectedElements) {
    using namespace MARTe;
    uint32 numberOfElements = 0u;
    uint32 numberOfDimensions = 0u;
    StreamString signalName;

    TypeDescriptor signalType = GetSignalType(direction, signalIdx);
    bool ok = GetSignalName(direction, signalIdx, signalName);
    if (ok) {
        ok = GetSignalNumberOfElements(direction, signalIdx, numberOfElements);
    }
    if (ok) {
        ok = GetSignalNumberOfDimensions(direction, signalIdx, numberOfDimensions);
    }
    if (ok) {
        ok = (signalType == expectedType);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The signal named %s at index %d shall be of type %s and instead is of type %s", signalName.Buffer(), signalIdx,
                         TypeDescriptor::GetTypeNameFromTypeDescriptor(expectedType), TypeDescriptor::GetTypeNameFromTypeDescriptor(signalType));
        }
    }
    if (ok) {
        ok = (numberOfElements == expectedElements);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The signal named %s at index %d shall have %d elements and instead it has %d elements", signalName.Buffer(), signalIdx, expectedElements,
                         numberOfElements);
        }
    }
    if (ok) {
        ok = (numberOfDimensions == expectedDimensions);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The signal named %s at index %d shall have dimension %d  and instead it has dimension %d", signalName.Buffer(), signalIdx,
                         expectedDimensions, numberOfDimensions);
        }
    }

    return ok;
}

bool PacketGAM::Setup() {
    using namespace MARTe;
    bool ok = (GetNumberOfInputSignals() == 2u);
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "The number of input signals shall be 2");
    }
    if (ok) {
        //The check signal shall be the first, have type uint8 and 1 element.
        ok = CheckSignal(0u, InputSignals, UnsignedInteger8Bit, 0u, 1u);
    }
    if (ok) {
        //The packet should have 15 elements.
        ok = CheckSignal(1u, InputSignals, UnsignedInteger8Bit, 1u, 15u);
    }
    uint32 numberOfOutputs = GetNumberOfOutputSignals();
    if (ok) {
        //There are 54 output signals
        ok = (numberOfOutputs == 54u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The number of input signals shall be 54");
        }
    }
    uint32 i;
    for (i = 0u; (i < numberOfOutputs) && (ok); i++) {
        uint32 byteSize = 0u;
        ok = GetSignalByteSize(OutputSignals, i, byteSize);
        if (ok) {
            const uint32 DEBUG_CRC_IDX = 50u;
            outputMemorySize += byteSize;
            //The time is a uint64
            if (i == 0u) {
                ok = CheckSignal(i, OutputSignals, UnsignedInteger64Bit, 0u, 1u);
            }
            else if (i == (numberOfOutputs - 1u)) {
                //The last signal has 15 bytes
                ok = CheckSignal(i, OutputSignals, UnsignedInteger8Bit, 1u, 15u);
            }
            else if (i == DEBUG_CRC_IDX) {
                //The DEBUG CRC is of type uint16
                ok = CheckSignal(i, OutputSignals, UnsignedInteger16Bit, 0u, 1u);
            }
            else {
                //All the other signals are uint8 with one element
                ok = CheckSignal(i, OutputSignals, UnsignedInteger8Bit, 0u, 1u);
            }
        }
    }

    firstTime = true;
    return ok;
}

/*lint -e{715} currentStateName and nextStateName are not used in this implementation.*/
bool PacketGAM::PrepareNextState(const MARTe::char8 * const currentStateName, const MARTe::char8 * const nextStateName) {
    lastTime = 0xFFFFFFFFFFFFFFFFu;
    return true;
}

bool PacketGAM::Execute() {
    using namespace MARTe;
    bool ok = MemoryOperationsHelper::Set(GetOutputSignalsMemory(), '\0', outputMemorySize);
    uint8 *checkPacket = reinterpret_cast<uint8 *>(GetInputSignalMemory(0u)); // Check the packet?
    if (*checkPacket == 1u) {
        /**
         *    ACQUIRE DATA FROM CRIO AND STORE IT AS SIGNALS
         *
         *    Input: *packet
         *    Output: *timeSignal
         *            *outputSignal
         *            *hvpsStateOut
         *            *flsStateOut
         *            *fpgaErrorCode
         *            *logicMode
         *            *packetSequenceCounter
         *            *synchronismByte
         *            *crc
         *            *trigger
         *            *marteErrorCode
         *            *originalMessageMemory
         */
        uint8 *packet = reinterpret_cast<uint8 *>(GetInputSignalMemory(1u)); // Input signal: 15 bytes of data from CRIO

        // packet[14u..10u] | OutputSignalMemory(0) => Data Time
        uint8 *timeSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(0u));

        timeSignal[4u] = packet[14u];
        timeSignal[3u] = packet[13u];
        timeSignal[2u] = packet[12u];
        timeSignal[1u] = packet[11u];
        timeSignal[0u] = packet[10u];

        // packet[9u] | OutputSignalMemory(1..8) => From outputs of input interface
        uint32 byteNumber = 9u;
        uint32 outputSignalIdx = 1u;
        int32 endSignalIdx = 7;
        int32 i;
        for (i = endSignalIdx; i >= 0; i--) {
            uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
            *outputSignal = ((packet[byteNumber] >> i) & 0x1u);
            outputSignalIdx++;
        }

        // packet[8u] | OutputSignalMemory(9..16) => From outputs of input interface (6 bits) & From outputs of FLS input processing (2bits)
        byteNumber = 8u;
        for (i = endSignalIdx; i >= 0; i--) {
            uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
            *outputSignal = ((packet[byteNumber] >> i) & 0x1u);
            outputSignalIdx++;
        }

        //packet[7u] | OutputSignalMemory(17..24)  => From outputs of FLS input processing (8bits)
        byteNumber = 7u;

        for (i = endSignalIdx; i >= 0; i--) {
            uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
            *outputSignal = ((packet[byteNumber] >> i) & 0x1u);
            outputSignalIdx++;
        }

        //packet[6u] | OutputSignalMemory(25..32)  => From outputs of FLS input processing (8bits)
        byteNumber = 6u;

        for (i = endSignalIdx; i >= 0; i--) {
            uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
            *outputSignal = ((packet[byteNumber] >> i) & 0x1u);
            outputSignalIdx++;
        }

        //packet[5u] | OutputSignalMemory(33..40) => From outputs of FLS input processing (3bits) & OutAdHoc (1bit) & OutFLS(1bit)& OutHVPS (3bit)
        byteNumber = 5u;

        for (i = endSignalIdx; i >= 0; i--) {
            uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
            *outputSignal = ((packet[byteNumber] >> i) & 0x1u);
            outputSignalIdx++;
        }

        //packet[4u]  | OutputSignalMemory(41) => Outputs HVPS (3bits) & Signals from output interface (3bit) & FLS_Man_State (2bit)
        //            | OutputSignalMemory(42..45) => Signals from output interface (3bit) & FLS_Man_State (2bit)
        //            | OutputSignalMemory(46) => FLS_Man_State (2bit) packet4 & (1bit) packet3
        byteNumber = 4u;
        uint8 *hvpsStateOut = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
        uint8 hvpsStateOutTemp = (static_cast<uint8>(packet[byteNumber] >> 5u) & 0x7u);

        //3 bit order is inverted for hvpsStateOut!
        *hvpsStateOut = 0u;
        *hvpsStateOut |= (static_cast<uint8>(hvpsStateOutTemp << 2u) & 0x4u);
        *hvpsStateOut |= (hvpsStateOutTemp & 0x2u);
        *hvpsStateOut |= (static_cast<uint8>(hvpsStateOutTemp & 0x4u) >> 2u);
        outputSignalIdx++;

        endSignalIdx = 4;
        for (i = endSignalIdx; i >= 2; i--) {
            uint8 *outputSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx)); //Signal to store From outputs of input interface (14bits/signals)
            *outputSignal = ((packet[byteNumber] >> i) & 0x1u);
            outputSignalIdx++;
        }

        // FLS_Man_State (3 bits) => 2bits(packet[4]) & 1bit(packet[3])
        // FLS_Man_State -> 2 bits from packet 4
        uint8 *flsStateOut = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
        uint8 flsStateOutTemp = (packet[byteNumber] & 0x3u);
        flsStateOutTemp = static_cast<uint8>(flsStateOutTemp << 1u);

        //packet[3u] | *OutputSignalMemory(46) => FLS_Man_State (2bit) packet4 & (1bit) packet3
        //           |  OutputSignalMemory(47) => FPGAErrorCode
        //           |  OutputsignalMemory(48) => LogicalMode
        //           |  OutputSignalMemory(49) => PacketSequence Counter

        // FLS_Man_State -> 1 bit from packet 3
        byteNumber = 3u;
        flsStateOutTemp |= ((packet[byteNumber] >> 7u) & 0x1u);
        *flsStateOut = 0u;
        *flsStateOut |= (static_cast<uint8>(flsStateOutTemp << 2u) & 0x4u);
        *flsStateOut |= (flsStateOutTemp & 0x2u);
        *flsStateOut |= (static_cast<uint8>(flsStateOutTemp & 0x4u) >> 2u);
        outputSignalIdx++;

        //FPGA error code (4 bits)
        uint8 *fpgaErrorCode = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
        *fpgaErrorCode = static_cast<uint8>((packet[byteNumber] >> 3u) & 0xfu);
        outputSignalIdx++;

        //Logic mode (1 bit)
        uint8 *logicMode = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
        *logicMode = ((packet[byteNumber] >> 2u) & 0x1u);
        outputSignalIdx++;

        //Packet sequence counter
        int8 *packetSequenceCounter = reinterpret_cast<int8 *>(GetOutputSignalMemory(outputSignalIdx));
        uint8 packetSequenceCounterUInt8 = static_cast<uint8>((packet[byteNumber]) & 0x3u);
        *packetSequenceCounter = static_cast<int8>(packetSequenceCounterUInt8);
        outputSignalIdx++;

        //packet[2u] | OutputMemorysignal(50u) => Packet Sequence Counter
        byteNumber = 2u;
        uint8 *synchronismByte = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
        *synchronismByte = packet[byteNumber];
        outputSignalIdx++;

        //packet[1u..0u] | OutputMemorysignal(51u..52u) => CRC code
        uint16 *crc = reinterpret_cast<uint16 *>(GetOutputSignalMemory(outputSignalIdx));
        //lint -e{928} cast to access each byte individually
        uint8 *crb8b = reinterpret_cast<uint8 *>(crc);
        crb8b[1u] = packet[1u];
        crb8b[0u] = packet[0u];

        /**
         *    CHECK FOR PACKET ERRORS
         *
         *    This block is to analyse the reliability of the data acquired and send an error message if some bad behaviour is detected.
         *    ErrorCodes:
         *          FatalErrorCode: 0x1u
         *          InvalidPacketCounter: 0x2u
         *          SynchronismByteError: 0x4u
         *          TimesError: 0x10u
         */

        // CRC Check
        uint8 packetCodeError = 0u;
        bool fatalPacket = false;
        uint16 expectedCRC = packetCRC.ComputeCRC(&packet[14], 13, true);
        if (expectedCRC != *crc) {
            REPORT_ERROR(ErrorManagement::FatalError, "Invalid CRC detected. Expected: %x Read: %x", expectedCRC, *crc);
            fatalPacket = true;
            packetCodeError = 0x1u;
        }

        // Lost Packet Check: Detect packet counter increment errors. It has to increase from 00 to 03.
        if (!fatalPacket) {
            int8 packetCounterDifference = (*packetSequenceCounter - lastPacketCounter);
            if ((packetCounterDifference != 1) && (packetCounterDifference != -3)) {
                if (!firstTime) {
                    REPORT_ERROR(ErrorManagement::Warning, "Invalid packet counter difference detected. Expected: -1 or -3 and Read: %d", packetCounterDifference);
                    packetCodeError |= 0x2u;
                }
            }
            lastPacketCounter = *packetSequenceCounter;

            //Synchronism check: Detect synchronism byte errors. It has to be 0x55 or 0xAA, and change to the other in the next packet.
            if ((*synchronismByte == 0x55u) || (*synchronismByte == 0xAAu)) {
                if (*synchronismByte == lastSynchronismByte) {
                    if (!firstTime) {
                        //Packet repeated twice
                        REPORT_ERROR(ErrorManagement::Warning, "Synchronism byte repeated at least twice [%x]", *synchronismByte);
                        packetCodeError |= 0x4u;
                    }
                }
            }
            else {
                REPORT_ERROR(ErrorManagement::FatalError, "Invalid synchronism byte difference detected. Expected: 0x55 or 0xAA and Read: 0x%x", *synchronismByte);
                fatalPacket = true;
                packetCodeError = 0x8u;
            }
            //Do not remember last value if there was a serious issue.
            if ((packetCodeError & 0x8u) != 0x8u) {
                lastSynchronismByte |= *synchronismByte;
            }

            //Detect times going into the past!
            //lint -e{927} -e{826} memory order is as prescribed*/
            uint64 *currentTime = reinterpret_cast<uint64 *>(&timeSignal[0u]);
            if (lastTime >= *currentTime) {
                //Accept zero in the first packet
                if ((lastTime != 0xFFFFFFFFFFFFFFFFu) && (*currentTime != 0u)) {
                    REPORT_ERROR(ErrorManagement::Warning, "Time is going backwards or not increasing. Expected: currentTime (%.8u) to be > lastTime (%.8u)", *currentTime, lastTime);
                    packetCodeError = 0x10u;
                }
            }
            lastTime = *currentTime;

            firstTime = false;
        }

        /**
         * Additional Signals:
         *        trigger: signal send to MDS+ to store the data
         *        marteErrorCode: Indicates the error type
         *        OriginalMessageMemory: Store the original data
         */
        outputSignalIdx++;
        uint8 *trigger = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
        *trigger = !fatalPacket;

        outputSignalIdx++;
        uint8 *marteErrorCode = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
        *marteErrorCode = packetCodeError;

        outputSignalIdx++;
        //Store the raw packet at the end
        uint8 *originalMessageMemory = reinterpret_cast<uint8 *>(GetOutputSignalMemory(outputSignalIdx));
        if (ok) {
            ok = MemoryOperationsHelper::Copy(&originalMessageMemory[0], &packet[0], 15u);
        }
    }
    return ok;
}

CLASS_REGISTER(PacketGAM, "1.0")

