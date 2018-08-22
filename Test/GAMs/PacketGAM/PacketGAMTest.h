/**
 * @file PacketGAMTest.h
 * @brief Header file for class PacketGAMTest
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

 * @details This header file contains the declaration of the class PacketGAMTest
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef TEST_GAMS_PACKETGAMTEST_H_
#define TEST_GAMS_PACKETGAMTEST_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
/**
 * @brief Tests the PacketGAM public methods.
 */
class PacketGAMTest {
public:
    /**
     * @brief Tests the constructor.
     */
    bool TestConstructor();

    /**
     * @brief Tests the Setup method.
     */
    bool TestSetup();

    /**
     * @brief Tests the Setup method with a number of input signals which is != 2.
     */
    bool TestSetup_Not2Inputs();

    /**
     * @brief Tests the Setup method with a bad signal 0 definition.
     */
    bool TestSetup_BadSignal0();

    /**
     * @brief Tests the Setup method with a bad signal 1 definition.
     */
    bool TestSetup_BadSignal1();

    /**
     * @brief Tests the Setup method with a number of output signals which is != 54.
     */
    bool TestSetup_Not54Outputs();

    /**
     * @brief Tests the Setup method with a time signal that is not uint64.
     */
    bool TestSetup_BadTimeSignal();

    /**
     * @brief Tests the Setup method with a last signal that is not uint8[15].
     */
    bool TestSetup_BadLastSignal();

    /**
     * @brief Tests the Setup method with a CRC signal that is not uint16.
     */
    bool TestSetup_BadCRCSignal();

    /**
     * @brief Tests the Setup method with an output bit signal that is not uint8.
     */
    bool TestSetup_BadBitSignal();

    /**
     * @brief Tests the Setup method with an output bit signal that has the wrong number of dimensions.
     */
    bool TestSetup_BadDimensions();

    /**
     * @brief Tests the Execute method.
     */
    bool TestExecute();

    /**
     * @brief Tests the Execute method with a first time that is not zero.
     */
    bool TestExecute_FirstTime_NotZero();

    /**
     * @brief Tests that the Execute method detects an invalid CRC.
     */
    bool TestExecute_CRC();

    /**
     * @brief Tests that the Execute method detects an invalid CRC and a wrong synchronism byte.
     */
    bool TestExecute_CRC_Synchronism();

    /**
     * @brief Tests that the Execute method detects an invalid CRC at the first time.
     */
    bool TestExecute_CRC_FirstTime();

    /**
     * @brief Tests that the Execute method detects an invalid packet counter.
     */
    bool TestExecute_Packet_Counter();

    /**
     * @brief Tests that the Execute method detects an invalid synchronism byte.
     */
    bool TestExecute_Invalid_Synchronism();

    /**
     * @brief Tests that the Execute method detects a repeated synchronism byte.
     */
    bool TestExecute_Repeated_Synchronism();

    /**
     * @brief Tests that the Execute method detects time going backwards.
     */
    bool TestExecute_InvalidTime();

    /**
     * @brief Tests the PrepareNextState method.
     */
    bool TestPrepareNextState();
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* TEST_GAMS_PACKETGAMTEST_H_ */

