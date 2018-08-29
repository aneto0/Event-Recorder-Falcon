/**
 * @file CRIOUARTDataSource.cpp
 * @brief Source file for class CRIOUARTDataSource
 * @date 25/06/2018
 * @author Andre' Neto
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
 * the class CRIOUARTDataSource (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "CRIOUARTDataSource.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

CRIOUARTDataSource::CRIOUARTDataSource() :
        MemoryDataSourceI(), MARTe::EmbeddedServiceMethodBinderT<CRIOUARTDataSource>(*this, &CRIOUARTDataSource::CRIOThreadCallback), executor(*this) {
    using namespace MARTe;
    lastReadIdx = 0u;
    lastWrittenIdx = 0u;
    serialTimeout = 0u;
    packetByteSize = 0u;
    lastDataCopyHadTimeout = true;
    writeMark = NULL_PTR(bool *);
}

CRIOUARTDataSource::~CRIOUARTDataSource() {
    using namespace MARTe;
    if (executor.Stop() != ErrorManagement::NoError) {
        if (executor.Stop() != ErrorManagement::NoError) {
            REPORT_ERROR(ErrorManagement::FatalError, "Could not stop the executor");
        }
    }
    if (!eventSem.Close()) {
        REPORT_ERROR(ErrorManagement::OSError, "Failed to close EventSem");
    }
    if (writeMark != NULL_PTR(bool *)) {
        delete[] writeMark;
    }
    serial.Close();
}

bool CRIOUARTDataSource::Initialise(MARTe::StructuredDataI &data) {
    using namespace MARTe;
    bool ok = MemoryDataSourceI::Initialise(data);

    if (ok) {
        ok = eventSem.Create();
        if (!ok) {
            REPORT_ERROR(ErrorManagement::OSError, "Failed to create EventSem");
        }
    }
    if (ok) {
        ok = eventSem.Reset();
        if (!ok) {
            REPORT_ERROR(ErrorManagement::OSError, "Failed to reset EventSem");
        }
    }
    StreamString portName;
    if (ok) {
        ok = data.Read("PortName", portName);
        if (ok) {
            REPORT_ERROR(ErrorManagement::Information, "The port name is set to %s", portName.Buffer());
        }
        else {
            REPORT_ERROR(ErrorManagement::ParametersError, "The port name property shall be set");
        }
    }
    uint32 baudRate = 0u;
    if (ok) {
        ok = data.Read("BaudRate", baudRate);
        if (ok) {
            REPORT_ERROR(ErrorManagement::Information, "The baud rate is set to %d", baudRate);
        }
        else {
            REPORT_ERROR(ErrorManagement::ParametersError, "The baud rate property shall be set");
        }
    }
    if (ok) {
        ok = data.Read("Timeout", serialTimeout);
        if (ok) {
            REPORT_ERROR(ErrorManagement::Information, "The serial timeout is set to %d", serialTimeout);
        }
        else {
            REPORT_ERROR(ErrorManagement::ParametersError, "The serial timeout property shall be set");
        }
    }
    if (ok) {
        ok = serial.SetSpeed(baudRate);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::OSError, "Failed to set baud rate %d", baudRate);
        }
    }
    if (ok) {
        ok = serial.Open(portName.Buffer());
        if (!ok) {
            REPORT_ERROR(ErrorManagement::OSError, "Failed to open port %s", portName.Buffer());
        }
    }
    //Do not allow to add signals in run-time
    if (ok) {
        ok = data.MoveRelative("Signals");
    }
    if (ok) {
        ok = data.Write("Locked", 1u);
    }
    if (ok) {
        ok = data.MoveToAncestor(1u);
    }
    if (ok) {
        ok = executor.Initialise(data);
    }
    if (ok) {
        muxSem.Create();
    }
    return ok;
}

bool CRIOUARTDataSource::SetConfiguredDatabase(MARTe::StructuredDataI & data) {
    using namespace MARTe;
    bool ok = MemoryDataSourceI::SetConfiguredDatabase(data);
    if (ok) {
        ok = (GetNumberOfSignals() == 2u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Exactly two signals shall be defined (DataOK and Data)");
        }
    }
    if (ok) {
        ok = (GetSignalType(0u) == UnsignedInteger8Bit);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The type of the first signal (DataOK shall be UnsignedInteger8)");
        }
    }
    if (ok) {
        uint32 elements;
        ok = GetSignalNumberOfElements(0u, elements);
        if (ok) {
            ok = (elements == 1u);
        }
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The first signal (DataOK) shall have only one element");
        }
    }
    if (ok) {
        ok = GetSignalByteSize(1u, packetByteSize);
    }
    if (ok) {
        REPORT_ERROR(ErrorManagement::Information, "Going to read %d bytes from the serial interface.", packetByteSize);

        writeMark = new bool[numberOfBuffers];
        uint32 i;
        for (i = 0u; i < numberOfBuffers; i++) {
            writeMark[i] = false;
        }

    }
    return ok;
}

MARTe::ErrorManagement::ErrorType CRIOUARTDataSource::CRIOThreadCallback(MARTe::ExecutionInfo &info) {
    using namespace MARTe;
    ErrorManagement::ErrorType err;
    if (info.GetStage() == ExecutionInfo::MainStage) {
        if (muxSem.FastLock()) {
            if (writeMark[lastWrittenIdx]) {
                REPORT_ERROR(ErrorManagement::Warning, "Buffer overflow at index %d", lastWrittenIdx);
            }
        }
        muxSem.FastUnLock();

        //Memory is not interleaved, thus there will be numberOfBuffers dataOK before the actual data
        uint32 serialWriteIdx = numberOfBuffers + (lastWrittenIdx * packetByteSize);
        uint32 bytesToRead = packetByteSize;
        uint8 *dataOK = reinterpret_cast<uint8 *>(&(memory[lastWrittenIdx]));
        if (serial.Read(reinterpret_cast<char8 *>(&(memory[serialWriteIdx])), bytesToRead, serialTimeout)) {
            if (muxSem.FastLock()) {
                *dataOK = 1u;
                writeMark[lastWrittenIdx] = true;
                lastWrittenIdx++;
                if (lastWrittenIdx == numberOfBuffers) {
                    lastWrittenIdx = 0u;
                }
                if (!eventSem.Post()) {
                    REPORT_ERROR(ErrorManagement::OSError, "Failed to post EventSem");
                }
            }
            muxSem.FastUnLock();
        }
        else {
            //No data received from the serial.Read. Allow the MARTe real-time thread to execute, but set the dataOK to false.
            if (bytesToRead == 0u) {
                if (muxSem.FastLock()) {
                    *dataOK = 0u;
                    if (!eventSem.Post()) {
                        REPORT_ERROR(ErrorManagement::OSError, "Failed to post EventSem");
                    }
                }
                muxSem.FastUnLock();
            }
            //Read only part of the packet from the serial.Read.
            else {
                //We are lost. Ignore anything coming from the serial until we have a silent period. After a period of no data, assume that the data is framed again...
                uint32 timeoutToSynchronise = 500000;
                REPORT_ERROR(ErrorManagement::Warning, "Failed to read %d bytes from serial. Trying to resynchronise by waiting %d us for no data", packetByteSize, timeoutToSynchronise);
                while (serial.WaitRead(500000u)) {
                    char8 ignoredMem;
                    uint32 ignoredSize = 1u;
                    (void) serial.Read(&ignoredMem, ignoredSize);
                }
                REPORT_ERROR(ErrorManagement::Warning, "No data arrived in the last %d us. As such, the next packet should be synchronised.", timeoutToSynchronise);
            }
        }
    }
    else if (info.GetStage() == ExecutionInfo::StartupStage) {
        //Empty the UART. If in one second no data arrives, assume to be synchronised.
        while (serial.WaitRead(1000000u)) {
            char8 ignoredMem;
            uint32 ignoredSize = 1u;
            (void) serial.Read(&ignoredMem, ignoredSize);
        }
        REPORT_ERROR(ErrorManagement::Warning, "UART should now be empty");
    }

    return err;
}

void CRIOUARTDataSource::PrepareInputOffsets() {
    using namespace MARTe;
    if (muxSem.FastLock()) {
        if (!writeMark[lastReadIdx]) {
            if (!eventSem.Reset()) {
                REPORT_ERROR(ErrorManagement::OSError, "Failed to reset EventSem");
            }
            muxSem.FastUnLock();
            if (!eventSem.Wait()) {
                REPORT_ERROR(ErrorManagement::OSError, "Failed to wait EventSem");
            }
        }
        else {
            muxSem.FastUnLock();
        }
    }
}

bool CRIOUARTDataSource::GetInputOffset(const MARTe::uint32 signalIdx, const MARTe::uint32 numberOfSamples, MARTe::uint32 &offset) {
    if (signalIdx == 0u) {
        //Remember that the memory is organised as non-interleaved, i.e. signal1_1, signal1_2, ..., signal1_N, signal2_1, ..., signal2_N
        offset = lastReadIdx;
        if (muxSem.FastLock()) {
            MARTe::uint8 *dataOK = reinterpret_cast<MARTe::uint8 *>(&(memory[offset]));
            //Remember with 100% guarantee if the data was copied with a timeout or not, so that the lastReadIdx does not get incremented if new data
            //become available between here and TerminateInputCopy
            //See comment below in TerminateInputCopy.
            lastDataCopyHadTimeout = (*dataOK == 0u);
        }
        muxSem.FastUnLock();
    }
    else {
        //Remember that the memory pointer returned by the MemoryDataSourceI::GetSignalMemoryBuffer (and used by the broker) already points
        //to the beginning of each signal memory, so that each signal memory starts with a zero offset. Nevertheless, the "memory" variable points to the beginning
        //of the full memory (i.e. at the beginning of the memory of signal zero).
        offset = (lastReadIdx * packetByteSize);
    }

    return true;
}

bool CRIOUARTDataSource::TerminateInputCopy(const MARTe::uint32 signalIdx, const MARTe::uint32 offset, const MARTe::uint32 numberOfSamples) {
    //Only clear the dataOK after the data signal (i.e. signalIdx == 1) has been copied.
    if (signalIdx == 1u) {
        //Only increment if there was no timeout. The status of dataOK could have changed between the GetInputOffset and the TerminateInputCopy
        //i.e. GetInputOffset could have copied data with a timeout for a given index, but when getting into TerminateInputCopy new data could
        //be available. This would imply writeMark[lastReadIdx] = true and as such the lastReadIdx would be incremented and the data in lastReadIdx - 1 would not get coppied in
        //the next GetInputOffset
        if (!lastDataCopyHadTimeout) {
            if (muxSem.FastLock()) {
                if (writeMark[lastReadIdx]) {
                    writeMark[lastReadIdx] = false;
                    //Data was already read. Reset to zero the DataOK flag. (Memory is not interleaved, thus there will be numberOfBuffers dataOK before the data)
                    MARTe::uint8 *dataOK = reinterpret_cast<MARTe::uint8 *>(&(memory[lastReadIdx]));
                    *dataOK = 0u;
                    lastReadIdx++;
                    if (lastReadIdx == numberOfBuffers) {
                        lastReadIdx = 0u;
                    }
                }
            }
            lastDataCopyHadTimeout = true;
            muxSem.FastUnLock();
        }
    }
    return true;
}

const MARTe::char8 *CRIOUARTDataSource::GetBrokerName(MARTe::StructuredDataI &data, const MARTe::SignalDirection direction) {
    return "MemoryMapMultiBufferInputBroker";
}

bool CRIOUARTDataSource::Synchronise() {
    return true;
}

bool CRIOUARTDataSource::PrepareNextState(const MARTe::char8 * const currentStateName, const MARTe::char8 * const nextStateName) {
    using namespace MARTe;
    bool ok = true;
    if (executor.GetStatus() == EmbeddedThreadI::OffState) {
        ok = (executor.Start() == ErrorManagement::NoError);
    }
    return ok;
}

CLASS_REGISTER(CRIOUARTDataSource, "1.0")
