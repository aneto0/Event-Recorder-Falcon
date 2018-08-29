/**
 * @file PacketCRC.cpp
 * @brief Source file for class PacketCRC
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

 * @details This source file contains the definition of all the methods for
 * the class PacketCRC (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "PacketCRC.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

PacketCRC::PacketCRC() {
    initCRC = 0x0u;
    crcTable = NULL_PTR(MARTe::uint16 *);
}

PacketCRC::~PacketCRC() {
    if (crcTable != NULL_PTR(MARTe::uint16 *)) {
        delete[] crcTable;
        crcTable = NULL_PTR(MARTe::uint16 *);
    }
}

void PacketCRC::ComputeTable(const MARTe::uint16 pol) {
    using namespace MARTe;
    if (crcTable != NULL_PTR(MARTe::uint16 *)) {
        delete[] crcTable;
        crcTable = NULL_PTR(MARTe::uint16 *);
    }
    crcTable = new uint16[256u];

    uint32 d; //divisor
    for (d = 0u; d < 256u; d++) {
        uint16 curByte = static_cast<uint16>(d << 8u); /* move divident byte into MSB of 16Bit CRC */

        uint8 bit;
        for (bit = 0u; bit < 8u; bit++) {
            if ((curByte & 0x8000u) != 0u) {
                curByte <<= 1;
                curByte ^= pol;
            }
            else {
                curByte <<= 1;
            }
        }

        crcTable[d] = curByte;
    }
}

void PacketCRC::SetInitialCRC(const MARTe::uint16 initCRCIn) {
    initCRC = initCRCIn;
}

MARTe::uint16 PacketCRC::ComputeCRC(const MARTe::uint8 * const data, const MARTe::int32 size, const bool inputInverted) const {
    using namespace MARTe;
    int32 b;
    uint16 crc = initCRC;

    for (b = 0; b < size; b++) {
        uint8 pos = static_cast<uint8>((crc >> 8) ^ data[inputInverted ? -b : b]);
        /*lint -e{613} crcTable is not NULL if pre-condition is met*/
        crc = static_cast<uint16>(static_cast<uint16>(crc << 8) ^ crcTable[pos]);
    }

    return crc;
}

