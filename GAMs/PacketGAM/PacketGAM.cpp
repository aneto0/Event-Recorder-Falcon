/**
 * @file PacketGAM.cpp
 * @brief Source file for class PacketGAM
 * @date 07/08/2018
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
 * the class PacketGAM (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "PacketGAM.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
PacketGAM::PacketGAM() {

}

PacketGAM::~PacketGAM() {

}

bool PacketGAM::Setup() {
    using namespace MARTe;
    bool ok = (GetNumberOfInputSignals() == 1u);
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "The number of input signals shall be 1");
    }

    return ok;
}

bool PacketGAM::Execute() {
    using namespace MARTe;
    bool ok = true;
    uint8 *inputSignal = reinterpret_cast<uint8 *>(GetInputSignalMemory(0u));
    uint8 *timeSignal = reinterpret_cast<uint8 *>(GetOutputSignalMemory(0u));
    timeSignal[4u] = inputSignal[14u];
    timeSignal[3u] = inputSignal[13u];
    timeSignal[2u] = inputSignal[12u];
    timeSignal[1u] = inputSignal[11u];
    timeSignal[0u] = inputSignal[10u];

    return ok;
}


CLASS_REGISTER(PacketGAM, "1.0")


