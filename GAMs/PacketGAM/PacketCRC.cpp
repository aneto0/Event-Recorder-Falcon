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

void PacketCRC::ComputeTable(MARTe::uint16 generator) {
    using namespace MARTe;
    crcTable = new uint16[256u];

    uint32 div;
    for (div = 0; div < 256u; div++) {
        uint16 curByte = static_cast<uint16>(div << 8u); /* move divident byte into MSB of 16Bit CRC */

        uint8 bit;
        for (bit = 0u; bit < 8u; bit++) {
            if ((curByte & 0x8000u) != 0) {
                curByte <<= 1;
                curByte ^= generator;
            }
            else {
                curByte <<= 1;
            }
        }

        crcTable[div] = curByte;
    }
}

void PacketCRC::SetInitialCRC(MARTe::uint16 initCRCIn) {
    initCRC = initCRCIn;

}

MARTe::uint16 PacketCRC::ComputeCRC(MARTe::uint8 *data, MARTe::int32 size, bool inputInverted) {
    using namespace MARTe;
    int32 b;
    uint32 crc = initCRC;

    for (b = 0u; b < size; b++) {
        /* XOR-in next input byte into MSB of crc, that's our new intermediate divident */
        uint8 pos = static_cast<uint8>((crc >> 8) ^ data[inputInverted ? -b : b]); /* equal: ((crc ^ (b << 8)) >> 8) */
        /* Shift out the MSB used for division per lookuptable and XOR with the remainder */
        crc = static_cast<uint16>((crc << 8) ^ static_cast<uint16>(crcTable[pos]));
    }

    return crc;

}

