/**
 * @file CRIOUARTSerialGTest.cpp
 * @brief Source file for class CRIOUARTSerialGTest
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
 * the class CRIOUARTSerialGTest (public, protected, and private). Be aware that some
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
#include "CRIOUARTSerialTest.h"

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
TEST(CRIOUARTSerialGTest,TestConstructor) {
    CRIOUARTSerialTest test;
    ASSERT_TRUE(test.TestConstructor());
}

TEST(CRIOUARTSerialGTest,TestRead) {
    CRIOUARTSerialTest test;
    ASSERT_TRUE(test.TestRead());
}

TEST(CRIOUARTSerialGTest,TestReadTimeout) {
    CRIOUARTSerialTest test;
    ASSERT_TRUE(test.TestReadTimeout());
}

TEST(CRIOUARTSerialGTest,TestClose) {
    CRIOUARTSerialTest test;
    ASSERT_TRUE(test.TestClose());
}

TEST(CRIOUARTSerialGTest,TestOpen) {
    CRIOUARTSerialTest test;
    ASSERT_TRUE(test.TestOpen());
}

TEST(CRIOUARTSerialGTest,TestOpen_WrongName) {
    CRIOUARTSerialTest test;
    ASSERT_TRUE(test.TestOpen_WrongName());
}

TEST(CRIOUARTSerialGTest,TestSetSpeed) {
    CRIOUARTSerialTest test;
    ASSERT_TRUE(test.TestSetSpeed());
}

TEST(CRIOUARTSerialGTest,TestSetSpeed_False) {
    CRIOUARTSerialTest test;
    ASSERT_TRUE(test.TestSetSpeed_False());
}

TEST(CRIOUARTSerialGTest,TestSetSpeed_False_Open) {
    CRIOUARTSerialTest test;
    ASSERT_TRUE(test.TestSetSpeed_False_Open());
}

TEST(CRIOUARTSerialGTest,TestWaitRead) {
    CRIOUARTSerialTest test;
    ASSERT_TRUE(test.TestWaitRead());
}

TEST(CRIOUARTSerialGTest,TestWrite) {
    CRIOUARTSerialTest test;
    ASSERT_TRUE(test.TestWrite());
}
