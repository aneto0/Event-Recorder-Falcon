/**
 * @file CRIOUARTDataSource.h
 * @brief Header file for class CRIOUARTDataSource
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

 * @details This header file contains the declaration of the class CRIOUARTDataSource
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef CRIOUARTDATASOURCE_CRIOUARTDATASOURCE_H_
#define CRIOUARTDATASOURCE_CRIOUARTDATASOURCE_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "CRIOUARTSerial.h"
#include "EmbeddedServiceMethodBinderT.h"
#include "EventSem.h"
#include "MemoryDataSourceI.h"
#include "SingleThreadService.h"
#include "StructuredDataI.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

/**
 * @brief TODO
 */
class CRIOUARTDataSource: public MARTe::MemoryDataSourceI, public MARTe::EmbeddedServiceMethodBinderT<CRIOUARTDataSource> {
public:
    CLASS_REGISTER_DECLARATION()
    /**
     * @brief TODO
     */
CRIOUARTDataSource    ();

    /**
     * @brief TODO
     */
    virtual ~CRIOUARTDataSource();

    /**
     * @see TODO
     */
    virtual bool Initialise(MARTe::StructuredDataI &data);

    /**
     * @see TODO
     */
    virtual bool SetConfiguredDatabase(MARTe::StructuredDataI & data);

    /**
     * @see TODO
     */
    virtual bool GetInputOffset(const MARTe::uint32 signalIdx, const MARTe::uint32 numberOfSamples, MARTe::uint32 &offset);

    /**
     * @see TODO
     */
    virtual void PrepareInputOffsets();

    /**
     * @see TODO
     */
    virtual bool TerminateInputCopy(const MARTe::uint32 signalIdx, const MARTe::uint32 offset, const MARTe::uint32 numberOfSamples);

    /**
     * @see TODO
     */
    MARTe::ErrorManagement::ErrorType CRIOThreadCallback(MARTe::ExecutionInfo &info);

    /**
     * @see TODO
     */
    virtual const MARTe::char8 *GetBrokerName(MARTe::StructuredDataI &data, const MARTe::SignalDirection direction);

    /**
     * @see TODO
     */
    virtual bool Synchronise();

    /**
     * @see TODO
     */
    virtual bool PrepareNextState(const MARTe::char8 * const currentStateName, const MARTe::char8 * const nextStateName);

private:

    /**
     * @see TODO
     */
    MARTe::SingleThreadService executor;

    /**
     * TODO
     */
    MARTe::uint32 lastReadIdx;

    /**
     * TODO
     */
    MARTe::uint32 lastWrittenIdx;

    /**
     * TODO
     */
    MARTe::uint32 packetByteSize;

    /**
     * TODO
     */
    MARTe::EventSem eventSem;

    /**
     * TODO
     */
    MARTe::FastPollingMutexSem muxSem;

    /**
     * TODO
     */
    bool *writeMark;

    /**
     * TODO
     */
    bool lastDataCopyHadTimeout;

    /**
     * TODO
     */
    CRIOUARTSerial serial;

    /**
     * TODO
     */
    MARTe::uint32 serialTimeout;

};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* CRIOUARTDATASOURCE_CRIOUARTDATASOURCE_H_ */

