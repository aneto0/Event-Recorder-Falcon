/**
 * @file CRIOUARTDataSource.cpp
 * @brief Source file for class CRIOUARTDataSource
 * @date 25/06/2018
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
        ok = (GetNumberOfSignals() == 1u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Only one signal is accepted");
        }
    }
    packetByteSize = 0u;
    uint32 n;
    for (n = 0u; (n < numberOfSignals) && (ok); n++) {
        uint32 nBytes = 0u;
        ok = GetSignalByteSize(0u, nBytes);

        if (ok) {
            packetByteSize += nBytes;
        }
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

        uint32 writeIdx = (lastWrittenIdx * packetByteSize);
        uint32 bytesToRead = packetByteSize;
        if (serial.Read(reinterpret_cast<char8 *>(&(memory[writeIdx])), bytesToRead, serialTimeout)) {
            if (muxSem.FastLock()) {
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
            if (bytesToRead != 0u) {
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
    offset = (lastReadIdx * packetByteSize);

    return true;
}

bool CRIOUARTDataSource::TerminateInputCopy(const MARTe::uint32 signalIdx, const MARTe::uint32 offset, const MARTe::uint32 numberOfSamples) {
    if (muxSem.FastLock()) {
        writeMark[lastReadIdx] = false;
    }
    muxSem.FastUnLock();
    lastReadIdx++;
    if (lastReadIdx == numberOfBuffers) {
        lastReadIdx = 0u;
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
