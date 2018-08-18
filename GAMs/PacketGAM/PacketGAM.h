/**
 * @file PacketGAM.h
 * @brief Header file for class PacketGAM
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

 * @details This header file contains the declaration of the class PacketGAM
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef GAMS_PACKETGAM_PACKETGAM_H_
#define GAMS_PACKETGAM_PACKETGAM_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "GAM.h"
#include "PacketCRC.h"
#include "StatefulI.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

/**
 * @brief GAM which copies the CRIO input packet to different output signals.
 * @details This GAM copies the input packet signal received from the CRIO UART interface and
 *  verifies the correctness of the packet, namely that:
 * - the CRC is valid (see PacketCRC);
 * - the packet counter is incremented by 1 with respect to the last packet;
 * - the synchronism byte alternates between 0xAA and 0x55;
 * - the packet time increases monotonically;
 *
 * Each information encoded inside the packet is written to a different signal as described
 * in the configuration example below.
 *
 * These signals are followed by an output trigger signal, which is set to 1 if the packet is valid;
 *  a "MARTe error code" which indicates the type of error (if any) and a signal containing the original packet received.
 *
 * The GAM also expects an input signal (the first signal) that indicates if the packet should be analysed. If not,
 *  (i.e. when the input signal value is zero), all the outputs (including the trigger signal) will be set to 0.
 *
 * The MARTe error code will be assigned one of the following values:
 * - 0x0 => No error - trigger will be set to 1;;
 * - 0x1 => Fatal error code (invalid CRC) - trigger will be set to 0;
 * - 0x2 => Packet counter did not increment - trigger will be set to 1;
 * - 0x4 => Synchronism byte repeated twice in a row - trigger will be set to 1;
 * - 0x8 => Invalid synchronism byte (i.e. neither 0xAA nor 0x55) - trigger will be set to 0;
 *
 * The configuration syntax is (names are only given as an example, but the size and the exact number of signals shall be respected):
 *
 * <pre>
 *   +PacketGAM = {
 *       Class = PacketGAM
 *       InputSignals = {
 *          CheckPacket = { //Should this packet be checked? This is to take into account possible time-outs of the serial
 *              DataSource = Drv1
 *              Type = uint8
 *          }
 *          Packet = {
 *              DataSource = Drv1
 *              Type = uint8
 *              NumberOfElements = 15
 *          }
 *       }
 *       OutputSignals = {
 *          TIME = {
 *              DataSource = DDB1
 *              Type = uint64
 *          }
 *          START_HVPS = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          MODULATE = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          RESET_HVPS = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          EU_ARC = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          RU_ARC = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PLC_TRIP_1 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PLC_TRIP_2 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          START_TIME = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          CRITICAL_ERR = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          ADMIN_MODE = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          MODE_LOSS = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          STRAY_1 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          STRAY_2 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          RESET_FPGA = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_EU_ARC = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_RU_ARC = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_PLC_TRIP_1 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_PLC_TRIP_2 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_MODE_LOSS = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_STRAY_1 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_STRAY_2 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_L_EU_ARC = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_L_RU_ARC = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_L_PLC_TRIP_1 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_L_PLC_TRIP_2 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_L_MODE_LOSS = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_L_STRAY_1 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_L_STRAY_2 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_B_EU_ARC = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_B_RU_ARC = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_B_PLC_TRIP_1 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_B_PLC_TRIP_2 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_B_MODE_LOSS = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_B_STRAY_1 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          PRE_PROC_OUT_B_STRAY_2 = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          ADHOC_TRIP_OUT = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          OUT_FLS = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          HVPS_OUT_TRIP_OUT = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          HVPS_OUT_ON_MPS = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          HVPS_OUT_ON_BPS = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          HVPS_STATE_OUT = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          OUT_IF_TRIP_PLC = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          OUT_IF_TRIP_MPS = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          OUT_IF_TRIP_BPS = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          FLS_MANAGER_STATE_OUT = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          ERROR_CODE_VALUE = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          LOGIC_MODE_VALUE = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          DEBUG_PACKET_COUNTER = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          DEBUG_SYNCH_BYTE = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          DEBUG_CRC = {
 *              DataSource = DDB1
 *              Type = uint16
 *          }
 *          TRIGGER = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          MARTE_ERROR_CODE = {
 *              DataSource = DDB1
 *              Type = uint8
 *          }
 *          ORIGINAL_PACKET = {
 *              DataSource = DDB1
 *              Type = uint8
 *              NumberOfElements = 15
 *          }
 *       }
 *   }
 * </pre>
 */
class PacketGAM: public MARTe::GAM, public MARTe::StatefulI {

public:
    CLASS_REGISTER_DECLARATION()

    /**
     * @brief Default constructor. NOOP.
     */
PacketGAM    ();

    /**
     * @brief Destructor
     */
    virtual ~PacketGAM();

    /**
     * @brief Verifies that the configuration described above is correctly set.
     * @return true if the configuration respects all the rules described in the class description.
     */
    virtual bool Setup();

    /**
     * @brief If the first signal (enable) is set to one, the input packet is discombobulated into individual output signals and all the rules listed
     *  in the class description are verified. The trigger and MARTe error code are then set appropriately.
     *  @return true if the memory assignment and copy operations are successful.
     */
    virtual bool Execute();

    /**
     * @brief Initialises the last read time to 0xFFFFFFFFFFFFFFFFu so that the first packet is allowed to have a time that is inferior to the second packet.
     * @returns true.
     */
    virtual bool PrepareNextState(const MARTe::char8 * const currentStateName, const MARTe::char8 * const nextStateName);

private:
    /**
     * @brief Check if the signal is of the expected type, number of elements and dimension.
     * @param[in] signalIdx the index of the signal to check.
     * @param[in] direction the signal direction.
     * @param[in] expectedType the signal expected type.
     * @param[in] expectedDimensions the signal expected number of dimensions.
     * @param[in] expectedElements the signal expected number of elements.
     * @return true if the signal respects the expected value.
     */
    bool CheckSignal(MARTe::uint32 signalIdx, MARTe::SignalDirection direction, MARTe::TypeDescriptor expectedType, MARTe::uint32 expectedDimensions, MARTe::uint32 expectedElements);

    /**
     * TODO
     */
    PacketCRC packetCRC;

    /**
     * TODO
     */
    MARTe::int8 lastPacketCounter;

    /**
     * TODO
     */
    MARTe::int8 lastSynchronismByte;

    /**
     * TODO
     */
    MARTe::uint64 lastTime;

    /**
     * TODO
     */
    bool firstTime;

    /**
     * Total size of the output memory
     */
    MARTe::uint32 outputMemorySize;

};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* GAMS_PACKETGAM_PACKETGAM_H_ */

