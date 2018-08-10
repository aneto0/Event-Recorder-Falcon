/**
 * @file PacketGAM.h
 * @brief Header file for class PacketGAM
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

 * @details This header file contains the declaration of the class PacketGAM
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef GAMS_PACKETGAM_PACKETGAM_H_
#define GAMS_PACKETGAM_PACKETGAM_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "GAM.h"
#include "PacketCRC.h"
#include "StatefulI.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

/**
 * @brief GAM which copies the CRIO input to different signals ouptuts.
 *  Allows to check the reliability of the received packet, checking CRC, counters and synchronism bits.
 *
 */
class PacketGAM: public MARTe::GAM, public MARTe::StatefulI {

public:
    CLASS_REGISTER_DECLARATION()

    /**
     * @brief Default constructor. NOOP.
     */
PacketGAM    ();

    /**
     * @brief Destructor
     */
    virtual ~PacketGAM();

    /**
     * @brief TODO. REMEMBER TO TEST CONFIGURATION IS AS EXPECTED.
     * @return true if the input signals has been successfully copied.
     */
    virtual bool Setup();

    /**
     * @brief TODO
     */
    virtual bool Execute();

    /**
     * @brief TODO
     */
    virtual bool PrepareNextState(const MARTe::char8 * const currentStateName, const MARTe::char8 * const nextStateName);

private:
    /**
     * TODO
     */
    PacketCRC packetCRC;

    /**
     * TODO
     */
    MARTe::int8 lastPacketCounter;

    /**
     * TODO
     */
    MARTe::int8 lastSynchronismByte;

    /**
     * TODO
     */
    MARTe::uint64 lastTime;

    /**
     * TODO
     */
    bool firstTime;

};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* GAMS_PACKETGAM_PACKETGAM_H_ */

