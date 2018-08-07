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
#include "GAMScheduler.h"
#include "MemoryDataSourceI.h"
#include "MemoryMapInputBroker.h"
#include "MemoryMapOutputBroker.h"
#include "MemoryOperationsHelper.h"
#include "ObjectRegistryDatabase.h"
#include "PacketGAM.h"
#include "PacketGAMTest.h"
#include "RealTimeApplication.h"
#include "StandardParser.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
/**
 * Gives access to the PacketGAM memory for the Execute test
 */
class PacketGAMHelper: public PacketGAM {
public:
    CLASS_REGISTER_DECLARATION()PacketGAMHelper() : PacketGAM() {
    }

    virtual ~PacketGAMHelper() {

    }

    void *GetInputSignalsMemory() {
        return PacketGAM::GetInputSignalsMemory();
    }

    void *GetOutputSignalsMemory(MARTe::uint32 idx) {
        return PacketGAM::GetOutputSignalMemory(idx);
    }

    void *GetOutputSignalsMemory() {
        return PacketGAM::GetOutputSignalsMemory();
    }
};
CLASS_REGISTER(PacketGAMHelper, "1.0");

/**
 * A dummy PacketGAM which can be used to test different configuration interfaces
 *  to the PacketGAM
 */
class PacketGAMDataSourceHelper: public MARTe::MemoryDataSourceI {
public:
    CLASS_REGISTER_DECLARATION()

PacketGAMDataSourceHelper    () : MARTe::MemoryDataSourceI() {
    }

    virtual ~PacketGAMDataSourceHelper() {

    }

    virtual const MARTe::char8 *GetBrokerName(MARTe::StructuredDataI &data,
            const MARTe::SignalDirection direction) {
        if (direction == MARTe::InputSignals) {
            return "MemoryMapInputBroker";
        }
        return "MemoryMapOutputBroker";
    }

    virtual bool PrepareNextState(const MARTe::char8 * const currentStateName,
            const MARTe::char8 * const nextStateName) {
        return true;
    }

    virtual bool Synchronise() {
        return true;
    }

};

CLASS_REGISTER(PacketGAMDataSourceHelper, "1.0");

/**
 * Starts a MARTe application that uses this GAM instance.
 */
static bool TestIntegratedInApplication(const MARTe::char8 * const config, bool destroy = true) {
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

    if (destroy) {
        god->Purge();
    }
    return ok;
}

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

bool PacketGAMTest::TestConstructor() {
    PacketGAM gam;
    return true;
}

bool PacketGAMTest::TestExecute() {
    using namespace MARTe;
    const MARTe::char8 * const config1 = ""
            "$Test = {"
            "    Class = RealTimeApplication"
            "    +Functions = {"
            "        Class = ReferenceContainer"
            "        +GAM1 = {"
            "            Class = PacketGAMHelper"
            "            InputSignals = {"
            "               Packet = {"
            "                   DataSource = Drv1"
            "                   Type = uint8"
            "                   NumberOfElements = 5"
            "               }"
            "            }"
            "            OutputSignals = {"
            "               Time = {"
            "                   DataSource = DDB1"
            "                   Type = uint64"
            "                   NumberOfElements = 1"
            "               }"
            "            }"
            "        }"
            "    }"
            "    +Data = {"
            "        Class = ReferenceContainer"
            "        DefaultDataSource = DDB1"
            "        +DDB1 = {"
            "            Class = GAMDataSource"
            "        }"
            "        +Timings = {"
            "            Class = TimingDataSource"
            "        }"
            "        +Drv1 = {"
            "            Class = PacketGAMDataSourceHelper"
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
    bool ok = TestIntegratedInApplication(config1, false);
    ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
    ReferenceT<PacketGAMHelper> gam = god->Find("Test.Functions.GAM1");
    if (ok) {
        ok = gam.IsValid();
    }
    if (ok) {
        /*uint32 *inMem = static_cast<uint32 *>(gam->GetInputSignalsMemory());
        uint32 *outMem = static_cast<uint32 *>(gam->GetOutputSignalsMemory());
        uint32 n;
         for (n = 0; n < 10; n++) {
         inMem[n] = (n * n + 1);

         }
         for (n = 0; n < 10; n++) {
         outMem[n] = 0;
         }*/
    }
    if (ok) {
        ok = gam->Execute();
    }
    if (ok) {
        //uint64 *outMem = static_cast<uint64 *>(gam->GetOutputSignalMemory(0u));
        /*uint32 n;
         for (n = 0; (n < 10) && (ok); n++) {
         ok = (outMem[n] == (n * n + 1));
         }*/
    }
    god->Purge();
    return ok;

    ObjectRegistryDatabase::Instance()->Purge();
    return true;
}

