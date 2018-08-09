/**
 * @file PacketCRC.h
 * @brief Header file for class PacketCRC
 * @date 08/08/2018
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

 * @details This header file contains the declaration of the class PacketCRC
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef GAMS_PACKETGAM_PACKETCRC_H_
#define GAMS_PACKETGAM_PACKETCRC_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

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
class PacketCRC {
public:
    /**
     * @brief TODO
     */
    PacketCRC();

    /**
     * @brief TODO
     */
    virtual ~PacketCRC();

    /**
     * @brief TODO
     */
    void ComputeTable(MARTe::uint16 pol);

    /**
     * @brief TODO
     */
    void SetInitialCRC(MARTe::uint16 initCRCIn);

    /**
     * @brief TODO REMEMBER TO DOCUMENT THAT IF INPUT IS INVERTED, data MUST POINT AT THE LAST INDEX!!!
     */
    MARTe::uint16 ComputeCRC(MARTe::uint8 *data, MARTe::int32 size, bool inputInverted);

private:
    /**
     * TODO
     */
    MARTe::uint16 *crcTable;

    /**
     * TODO
     */
    MARTe::uint16 initCRC;
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* GAMS_PACKETGAM_PACKETCRC_H_ */

