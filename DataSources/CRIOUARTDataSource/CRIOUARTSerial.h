/**
 * @file CRIOUARTSerial.h
 * @brief Header file for class CRIOUARTSerial
 * @date 07/08/2018
 * @author Filippo Sartori
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

 * @details This header file contains the declaration of the class CRIOUARTSerial
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef DATASOURCES_CRIOUARTDATASOURCE_CRIOUARTSERIAL_H_
#define DATASOURCES_CRIOUARTDATASOURCE_CRIOUARTSERIAL_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/
#include <fcntl.h>

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "CompilerTypes.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
/**
 * @brief TODO
 */
class CRIOUARTSerial {
public:
    /**
     * @brief TODO
     */
    CRIOUARTSerial();

    /**
     * @brief TODO
     */
    virtual ~CRIOUARTSerial();

    /**
     * @brief TODO
     */
    bool SetSpeed(MARTe::uint32 &speed);

    /**
     * @brief TODO
     */
    bool Open(const MARTe::char8 *name);

    /**
     * @brief TODO
     */
    void Close();

    /**
     * @brief TODO
     */
    bool Read(MARTe::char8 *buffer, MARTe::uint32 &size);

    /**
     * @brief TODO
     */
    bool Read(MARTe::char8 *buffer, MARTe::uint32 &size, MARTe::uint32 timeoutUsec);

    /**
     * @brief TODO
     */
    bool WaitRead(MARTe::uint32 timeoutUsec);

    /**
     * @brief TODO
     */
    bool Write(MARTe::char8 *buffer, MARTe::uint32 size);

    /**
     * @brief TODO
     */
    bool WaitWrite(MARTe::uint32 timeoutUsec);
private:
    /**
     * TODO
     */
    MARTe::int32 fileDescriptor;

    /**
     * TODO
     */
    MARTe::uint32 speedCode;

    /**
     * TODO
     */
    fd_set readFDS;

    /**
     * TODO
     */
    fd_set readFDS_done;

    /**
     * TODO
     */
    fd_set writeFDS;

    /**
     * TODO
     */
    fd_set writeFDS_done;
};
/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* DATASOURCES_CRIOUARTDATASOURCE_CRIOUARTSERIAL_H_ */

