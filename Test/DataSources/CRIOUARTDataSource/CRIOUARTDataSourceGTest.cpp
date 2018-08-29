/**
 * @file CRIOUARTDataSourceGTest.cpp
 * @brief Source file for class CRIOUARTDataSourceGTest
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
 * the class CRIOUARTDataSourceGTest (public, protected, and private). Be aware that some
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
#include "CRIOUARTDataSourceTest.h"

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
TEST(CRIOUARTDataSourceGTest,TestConstructor) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestConstructor());
}

TEST(CRIOUARTDataSourceGTest,TestCRIOThreadCallback) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestCRIOThreadCallback());
}

TEST(CRIOUARTDataSourceGTest,TestCRIOThreadCallback_FailedRead) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestCRIOThreadCallback_FailedRead());
}

TEST(CRIOUARTDataSourceGTest,TestGetBrokerName) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestGetBrokerName());
}

TEST(CRIOUARTDataSourceGTest,TestGetInputOffset) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestGetInputOffset());
}

TEST(CRIOUARTDataSourceGTest,TestInitialise) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestInitialise());
}

TEST(CRIOUARTDataSourceGTest,TestInitialise_False_Baud) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestInitialise_False_Baud());
}

TEST(CRIOUARTDataSourceGTest,TestInitialise_False_BaudRate) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestInitialise_False_BaudRate());
}

TEST(CRIOUARTDataSourceGTest,TestInitialise_False_Port) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestInitialise_False_Port());
}

TEST(CRIOUARTDataSourceGTest,TestInitialise_False_PortName) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestInitialise_False_PortName());
}

TEST(CRIOUARTDataSourceGTest,TestInitialise_False_Timeout) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestInitialise_False_Timeout());
}

TEST(CRIOUARTDataSourceGTest,TestPrepareInputOffsets) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestPrepareInputOffsets());
}

TEST(CRIOUARTDataSourceGTest,TestPrepareNextState) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestPrepareNextState());
}

TEST(CRIOUARTDataSourceGTest,TestSetConfiguredDatabase) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestSetConfiguredDatabase());
}

TEST(CRIOUARTDataSourceGTest,TestSetConfiguredDatabase_False_Signal1_Not_UInt8) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestSetConfiguredDatabase_False_Signal1_Not_UInt8());
}

TEST(CRIOUARTDataSourceGTest,TestSetConfiguredDatabase_False_Signal1_Not_1_Element) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestSetConfiguredDatabase_False_Signal1_Not_1_Element());
}

TEST(CRIOUARTDataSourceGTest,TestSetConfiguredDatabase_False_2_Signals) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestSetConfiguredDatabase_False_2_Signals());
}

TEST(CRIOUARTDataSourceGTest,TestSynchronise) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestSynchronise());
}

TEST(CRIOUARTDataSourceGTest,TestTerminateInputCopy) {
    CRIOUARTDataSourceTest test;
    ASSERT_TRUE(test.TestTerminateInputCopy());
}
