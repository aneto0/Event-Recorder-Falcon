/**
 * @file PacketCRCTest.h
 * @brief Header file for class PacketCRCTest
 * @date 07/08/2018
 * @author Andre Neto
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

 * @details This header file contains the declaration of the class PacketCRCTest
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef TEST_GAMS_PACKETCRCTEST_H_
#define TEST_GAMS_PACKETCRCTEST_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "PacketCRC.h"
/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
/**
 * @brief Tests the PacketCRC public methods.
 */
class PacketCRCTest {
public:
    /**
     * @brief Tests the constructor.
     */
    bool TestConstructor();

    /**
     * @brief Tests the compute table with the polynomial 0x3D65.
     */
    bool TestComputeTable_0x3D65();

    /**
     * @brief Tests the compute table with the polynomial 0x3D65 and an inverted input.
     */
    bool TestComputeTable_0x3D65_inverted();

    /**
     * @brief Tests the compute table with the polynomial 0x3D65 and initial crc != 0.
     */
    bool TestComputeTable_0x3D65_initialCRC();

    /**
     * @brief Tests the compute table with the polynomial 0x3D65, an inverted input and initial crc != 0.
     */
    bool TestComputeTable_0x3D65_inverted_initialCRC();

    /**
     * @brief Tests that after computing the CRC of the data, recomputing the CRC using the computed CRC will output zero.
     */
    bool TestComputableTable_CRC_0();

    /**
     * @brief Tests that after computing the CRC of the data (with inverted input) with the computed CRC is zero.
     */
    bool TestComputableTable_CRC_0_inverted();

    /**
     * @brief Tests that after computing the CRC of the data, recomputing the CRC using the computed CRC will output zero.
     */
    bool TestComputableTable_CRC_0_initialCRC();

    /**
     * @brief As TestComputableTable_CRC_0_initialCRC with inverted inputs.
     */
    bool TestComputableTable_CRC_0_inverted_initialCRC();

private:
    /**
     * @brief Computes the CRC against four different data arrays.
     */
    bool TestComputeCRC(MARTe::uint16 pol, MARTe::uint16 initialCRC, bool inputInverted, MARTe::uint16 *expectedCRC);

    /**
     * @brief Tests that after computing the CRC of the data with the computed CRC is zero.
     */
    bool TestComputeCRC0(MARTe::uint16 pol, MARTe::uint16 initialCRC, bool inputInverted, MARTe::uint8 *data);

};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* TEST_GAMS_PACKETCRCTEST_H_ */

