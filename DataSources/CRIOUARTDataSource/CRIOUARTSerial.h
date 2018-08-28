/**
 * @file CRIOUARTSerial.h
 * @brief Header file for class CRIOUARTSerial
 * @date 07/08/2018
 * @author Filippo Sartori
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

 * @details This header file contains the declaration of the class CRIOUARTSerial
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef DATASOURCES_CRIOUARTDATASOURCE_CRIOUARTSERIAL_H_
#define DATASOURCES_CRIOUARTDATASOURCE_CRIOUARTSERIAL_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/
#include <fcntl.h>

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "CompilerTypes.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
/**
 * @brief UART implementation based on the Linux general terminal interface that is provided to control asynchronous communications ports.
 * @details Some of termio parameters are hard-coded (e.g. parity, number of stop bits, ...) for the CRIO serial interface use-case.
 */
class CRIOUARTSerial {
public:
    /**
     * @brief FD_ZERO the file descriptors.
     */
    CRIOUARTSerial();

    /**
     * @brief Calls Close()
     */
    virtual ~CRIOUARTSerial();

    /**
     * @brief Sets the speed of the UART. Shall be called before the Open method.
     * @param[in,out] serial the speed to set and the speed that was actually set.
     * @return true if the speed was successfully updated.
     */
    bool SetSpeed(MARTe::uint32 &speed);

    /**
     * @brief Opens the UART described by the provided \a name.
     * @details The UART is open in read/write mode. Many of the UART properties are
     *  hard-coded for the CRIO use-case.
     * @param[in] name the UART file descriptor location.
     * @pre
     *   SetSpeed
     */
    bool Open(const MARTe::char8 *name);

    /**
     * @brief Closes the UART.
     * @pre
     *   Open
     */
    void Close();

    /**
     * @brief Reads \a size bytes into the \a buffer.
     * @param[in] buffer the memory where to write the read bytes.
     * @param[in,out] size the number of bytes to read and the number of bytes that were actually read.
     * @return true if the Read operation completes successfully (and reads all the requested bytes).
     * @pre
     *   Open
     */
    bool Read(MARTe::char8 *buffer, MARTe::uint32 &size);

    /**
     * @brief Reads \a size bytes into the \a buffer within \a timeoutUsec micro-seconds.
     * @param[in] buffer the memory where to write the read bytes.
     * @param[in,out] size the number of bytes to read and the number of bytes that were actually read.
     * @param[in] timeoutUsec the maximum time to complete the read operation.
     * @return true if the Read operation completes successfully within the timeout (and reads all the requested bytes).
     * @pre
     *   Open
     */
    bool Read(MARTe::char8 *buffer, MARTe::uint32 &size, MARTe::uint32 timeoutUsec);

    /**
     * @brief Waits \a timeoutUsec micro-seconds for data to be available for reading in the UART.
     * @param[in] timeoutUsec the maximum time to wait for data to be available.
     * @return true if data is available to be read within the timeout.
     */
    bool WaitRead(MARTe::uint32 timeoutUsec);

    /**
     * @brief Writes \a size bytes from the \a buffer.
     * @param[in] buffer the memory where to read the bytes to write.
     * @param[in] size the number of bytes to write.
     * @param[in] timeoutUsec the maximum time to complete the read operation.
     * @return true if the Read operation completes successfully within the timeout (and reads all the requested bytes).
     * @pre
     *   Open
     */
    bool Write(MARTe::char8 *buffer, MARTe::uint32 size);

    /**
     * @brief Waits \a timeoutUsec micro-seconds for the UART to be available for writing data.
     * @param[in] timeoutUsec the maximum time to wait for the write to be available.
     * @return true if the UART is available to write within the timeout.
     */
    bool WaitWrite(MARTe::uint32 timeoutUsec);
private:
    /**
     * The file descriptor associated to the Open operation.
     */
    MARTe::int32 fileDescriptor;

    /**
     * The speed set with SetSpeed.
     */
    MARTe::int32 speedCode;

    /**
     * The file descriptor array for the read in the select.
     */
    fd_set readFDS;

    /**
     * The file descriptor array ready to be read as returned by the select.
     */
    fd_set readFDS_done;

    /**
     * The file descriptor array for the write in the select.
     */
    fd_set writeFDS;

    /**
     * The file descriptor array ready to be written as returned by the select.
     */
    fd_set writeFDS_done;
};
/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* DATASOURCES_CRIOUARTDATASOURCE_CRIOUARTSERIAL_H_ */

