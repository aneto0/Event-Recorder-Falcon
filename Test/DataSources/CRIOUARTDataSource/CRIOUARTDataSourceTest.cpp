/**
 * @file CRIODataSourceTest.cpp
 * @brief Source file for class CRIODataSourceTest
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
 * the class CRIODataSourceTest (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "CRIOUARTDataSource.h"
#include "CRIOUARTSerial.h"
#include "CRIOUARTDataSourceTest.h"
#include "GAM.h"
#include "ObjectRegistryDatabase.h"
#include "RealTimeApplication.h"
#include "StandardParser.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
class CRIOUARTDataSourceTestGAM: public MARTe::GAM {
public:
    CLASS_REGISTER_DECLARATION()

CRIOUARTDataSourceTestGAM    () {
        expectedPatterns = NULL_PTR(MARTe::uint8 **);
        numberOfExpectedPatterns = 0u;
        numberOfPatternsFound = 0u;
        currentPattern = 0u;
        patternSize = 0u;
        executedAtLeastOnce = false;
    }

    virtual ~CRIOUARTDataSourceTestGAM() {
        if (expectedPatterns != NULL_PTR(MARTe::uint8 **)) {
            MARTe::uint32 i;
            for (i=0; i<numberOfExpectedPatterns; i++) {
                delete [] expectedPatterns[i];
            }
            delete [] expectedPatterns;
        }
    }

    bool Initialise(MARTe::StructuredDataI & data) {
        using namespace MARTe;
        bool ok = GAM::Initialise(data);
        AnyType arrayDescription = data.GetType("ExpectedPatterns");
        ok = arrayDescription.GetDataPointer() != NULL_PTR(void *);
        uint32 numberOfDimensions = 0u;
        if (ok) {
            numberOfDimensions = arrayDescription.GetNumberOfDimensions();
            ok = (numberOfDimensions == 2u);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "ExpectedPatterns is not a Matrix");
            }
        }
        if (ok) {
            numberOfExpectedPatterns = arrayDescription.GetNumberOfElements(1u);
            patternSize = arrayDescription.GetNumberOfElements(0u);
            ok = (numberOfExpectedPatterns > 0u);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "No rows defined in the matrix");
            }
        }
        if (ok) {
            ok = (patternSize > 0u);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "No columns defined in the matrix");
            }
        }
        if (ok) {
            expectedPatterns = new uint8*[numberOfExpectedPatterns];
            MARTe::uint32 i;
            for (i=0; i<numberOfExpectedPatterns; i++) {
                expectedPatterns[i] = new uint8[patternSize];
            }
            Matrix<uint8> expectedPatternsMatrix(expectedPatterns, numberOfExpectedPatterns, patternSize);
            ok = data.Read("ExpectedPatterns", expectedPatternsMatrix);
            if (ok) {
                REPORT_ERROR(ErrorManagement::Information, "expectedPatternsMatrix set to %d", expectedPatternsMatrix);
            }
            else {
                REPORT_ERROR(ErrorManagement::ParametersError, "Could not read the ExpectedPatterns");
            }
        }

        return ok;
    }

    bool Execute () {
        MARTe::uint8 *validSignal = reinterpret_cast<MARTe::uint8 *>(GetInputSignalMemory(0));
        MARTe::uint8 *dataSignal = reinterpret_cast<MARTe::uint8 *>(GetInputSignalMemory(1));
        if (*validSignal) {
            if (currentPattern < numberOfExpectedPatterns) {
                MARTe::uint32 i;
                bool ok = true;
                for (i=0u; (i<patternSize) && (ok); i++) {
                    ok = (dataSignal[i] == expectedPatterns[currentPattern][i]);
                }
                if (ok) {
                    MARTe::Vector<MARTe::uint8> vecTemp(dataSignal, patternSize);
                    REPORT_ERROR(MARTe::ErrorManagement::Information, "Read %d %d ...", *validSignal, vecTemp);
                    currentPattern++;
                }
            }
        }
        executedAtLeastOnce = true;
        return true;
    }

    bool Setup () {
        bool ok = (GetNumberOfInputSignals() == 2);
        return ok;
    }

    MARTe::uint8 **expectedPatterns;
    MARTe::uint8 numberOfExpectedPatterns;
    MARTe::uint8 numberOfPatternsFound;
    MARTe::uint8 currentPattern;
    MARTe::uint8 patternSize;
    bool executedAtLeastOnce;
};
CLASS_REGISTER(CRIOUARTDataSourceTestGAM, "1.0")

/**
 * Starts a MARTe application that uses this driver instance.
 */
static bool TestIntegratedInApplication(const MARTe::char8 * const config, MARTe::TimeoutType maxTimeout) {
    using namespace MARTe;

    ConfigurationDatabase cdb;
    StreamString configStream = config;
    configStream.Seek(0);
    StandardParser parser(configStream, cdb);

    bool ok = parser.Parse();

    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();

    if (ok) {
        god->Purge();
        ok = god->Initialise(cdb);
    }
    ReferenceT<RealTimeApplication> application;
    if (ok) {
        application = god->Find("Test");
        ok = application.IsValid();
    }
    if (ok) {
        ok = application->ConfigureApplication();
    }
    if (ok) {
        ok = application->PrepareNextState("State1");
    }
    if (ok) {
        application->StartNextStateExecution();
    }

    ReferenceT<CRIOUARTDataSourceTestGAM> gam = application->Find("Functions.GAM1");
    if (ok) {
        ok = gam.IsValid();
    }
    CRIOUARTSerial uart;
    if (ok) {
        uint32 speed = 115200;
        ok = uart.SetSpeed(speed);
    }
    if (ok) {
        ok = uart.Open("/dev/ttyUSB0");
    }
    if (ok) {
        while (!gam->executedAtLeastOnce) {
            Sleep::Sec(0.1);
        }
        uint32 i = 0u;
        bool done = false;
        uint64 timeoutTicks = HighResolutionTimer::Counter();
        uint64 maxTimeoutTicks = (timeoutTicks + maxTimeout.HighResolutionTimerTicks());
        while ((timeoutTicks < maxTimeoutTicks) && (!done) && (ok)) {
            done = (gam->currentPattern == gam->numberOfExpectedPatterns);
            timeoutTicks = HighResolutionTimer::Counter();
            Sleep::Sec(0.1);
            if (i < gam->numberOfExpectedPatterns) {
                uart.Write(reinterpret_cast<char8 *>(gam->expectedPatterns[i]), gam->patternSize);
                MARTe::Vector<MARTe::uint8> vecTemp(gam->expectedPatterns[i], gam->patternSize);
                REPORT_ERROR_STATIC(MARTe::ErrorManagement::Information, "Wrote %d", vecTemp);
                i++;
            }
        }
        ok = done;
    }

    if (ok) {
        application->StopCurrentStateExecution();
    }

    god->Purge();
    return ok;
}

//Standard configuration
const MARTe::char8 * const config1 = ""
        "$Test = {"
        "    Class = RealTimeApplication"
        "    +Functions = {"
        "        Class = ReferenceContainer"
        "        +GAM1 = {"
        "            Class = CRIOUARTDataSourceTestGAM"
        "            ExpectedPatterns = {"
        "                {2   3  3  3  3  3  3  6  3  3  3  3  3  3  2}"
        "                {4   5  5  5  5  5  5  8  5  5  5  5  5  5  4}"
        "                {6   7  7  7  7  7  7  9  7  7  7  7  7  7  6}"
        "                {8   9  9  9  9  9  9 11  9  9  9  9  9  9  8}"
        "                {10 11 11 11 11 11 11 13 11 11 11 11 11 11 10}"
        "                {12 13 13 13 13 13 13 15 13 13 13 13 13 13 12}"
        "                {14 15 15 15 15 15 15 17 15 15 15 15 15 15 14}"
        "            }"
        "            InputSignals = {"
        "               DataOK = {"
        "                   DataSource = CRIOUART"
        "                   Type = uint8"
        "                   NumberOfElements = 1"
        "               }"
        "               Packet = {"
        "                   DataSource = CRIOUART"
        "                   Type = uint8"
        "                   NumberOfElements = 15"
        "               }"
        "            }"
        "        }"
        "    }"
        "    +Data = {"
        "        Class = ReferenceContainer"
        "        DefaultDataSource = DDB1"
        "        +Timings = {"
        "            Class = TimingDataSource"
        "        }"
        "        +CRIOUART = {"
        "            Class = CRIOUARTDataSource"
        "            NumberOfBuffers = 3"
        "            PortName = \"/dev/ttyUSB0\""
        "            BaudRate = 115200"
        "            Timeout = 200000"
        "            CPUMask = 8"
        "            Signals = {"
        "                DataOK = {"
        "                    Type = uint8"
        "                    NumberOfElements = 1"
        "                }"
        "                Packet = {"
        "                    Type = uint8"
        "                    NumberOfElements = 15"
        "                }"
        "            }"
        "        }"
        "    }"
        "    +States = {"
        "        Class = ReferenceContainer"
        "        +State1 = {"
        "            Class = RealTimeState"
        "            +Threads = {"
        "                Class = ReferenceContainer"
        "                +Thread1 = {"
        "                    Class = RealTimeThread"
        "                    Functions = {GAM1}"
        "                }"
        "            }"
        "        }"
        "    }"
        "    +Scheduler = {"
        "        Class = GAMScheduler"
        "        TimingDataSource = Timings"
        "    }"
        "}";

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

bool CRIOUARTDataSourceTest::TestConstructor() {
    CRIOUARTDataSource ds;
    return (ds.GetNumberOfSignals() == 0);
}

bool CRIOUARTDataSourceTest::TestCRIOThreadCallback() {
    MARTe::TimeoutType timeout(5000);
    return TestIntegratedInApplication(config1, timeout);
}

