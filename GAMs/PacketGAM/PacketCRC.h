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
 * @brief Computes the expected CRC of any given input packet (see ComputeCRC).
 * @details The implementation is generic (for a 16 bit CRC) and can be used for any input.
 *
 * The CRC table shall be pre-computed by calling ComputeTable. The initial CRC value shall be set using SetInitialCRC, otherwise zero is assumed.
 *
 * The implementation is based on the algorithm implementation described in http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
 */
class PacketCRC {
public:
    /**
     * @brief NOOP.
     */
    PacketCRC();

    /**
     * @brief Destroys the previously allocated polynomial division lookup table.
     */
    virtual ~PacketCRC();

    /**
     * @brief Computes a lookup table for a given polynomial divisor.
     * @param[in] pol the polynomial divisor to be used for this CRC instance.
     */
    void ComputeTable(MARTe::uint16 pol);

    /**
     * @brief Sets the initial value of the CRC.
     */
    void SetInitialCRC(MARTe::uint16 initCRCIn);

    /**
     * @brief Computes the CRC for \a size bytes in \a data.
     * @param[in] data the bytes against which the CRC will be computed.
     * @param[in] size the number of bytes in \a data.
     * @param[in] inputInverted if true, the order of the bytes to compute the CRC will be data[0], data[-1], data[-2] ... data[-size + 1].
     * This implies that, when using inputInverted, the data must be pointing at the last byte (since negative indexes will be used)
     * @return the CRC value.
     */
    MARTe::uint16 ComputeCRC(MARTe::uint8 *data, MARTe::int32 size, bool inputInverted);

private:
    /**
     * Lookup table for a given polynomial divisor.
     */
    MARTe::uint16 *crcTable;

    /**
     * Initial value of the CRC (default is zero).
     */
    MARTe::uint16 initCRC;
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* GAMS_PACKETGAM_PACKETCRC_H_ */

