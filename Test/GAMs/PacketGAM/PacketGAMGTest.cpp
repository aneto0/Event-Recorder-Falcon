/**
 * @file PacketGAMGTest.cpp
 * @brief Source file for class PacketGAMGTest
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

 * @details This source file contains the definition of all the methods for
 * the class PacketGAMGTest (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include <limits.h>
#include "gtest/gtest.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
#include "PacketGAMTest.h"

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
TEST(PacketGAMGTest,TestConstructor) {
    PacketGAMTest test;
    ASSERT_TRUE(test.TestConstructor());
}

TEST(PacketGAMGTest,TestExecute) {
    PacketGAMTest test;
    ASSERT_TRUE(test.TestExecute());
}

TEST(PacketGAMGTest,TestExecute_FirstTime_NotZero) {
    PacketGAMTest test;
    ASSERT_TRUE(test.TestExecute_FirstTime_NotZero());
}

TEST(PacketGAMGTest,TestExecute_CRC) {
    PacketGAMTest test;
    ASSERT_TRUE(test.TestExecute_CRC());
}

TEST(PacketGAMGTest,TestExecute_CRC_Synchronism) {
    PacketGAMTest test;
    ASSERT_TRUE(test.TestExecute_CRC_Synchronism());
}

TEST(PacketGAMGTest,TestExecute_CRC_FirstTime) {
    PacketGAMTest test;
    ASSERT_TRUE(test.TestExecute_CRC_FirstTime());
}

TEST(PacketGAMGTest,TestExecute_Packet_Counter) {
    PacketGAMTest test;
    ASSERT_TRUE(test.TestExecute_Packet_Counter());
}

TEST(PacketGAMGTest,TestExecute_Invalid_Synchronism) {
    PacketGAMTest test;
    ASSERT_TRUE(test.TestExecute_Invalid_Synchronism());
}

TEST(PacketGAMGTest,TestExecute_Repeated_Synchronism) {
    PacketGAMTest test;
    ASSERT_TRUE(test.TestExecute_Repeated_Synchronism());
}

TEST(PacketGAMGTest,TestExecute_InvalidTime) {
    PacketGAMTest test;
    ASSERT_TRUE(test.TestExecute_InvalidTime());
}
