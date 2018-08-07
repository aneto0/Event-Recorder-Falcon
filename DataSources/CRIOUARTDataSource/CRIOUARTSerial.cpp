/**
 * @file CRIOUARTSerial.cpp
 * @brief Source file for class CRIOUARTSerial
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
 * the class CRIOUARTSerial (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "CRIOUARTSerial.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
struct SpeedTable {
    MARTe::uint32 code;
    MARTe::uint32 speed;
};

SpeedTable speedTable[] = { { B0, 0 }, { B50, 50 }, { B75, 75 }, { B110, 110 }, { B134, 134 }, { B150, 150 }, { B200, 200 }, { B300, 300 }, { B600, 600 }, { B1200, 1200 }, { B1800, 1800 }, { B2400,
        2400 }, { B4800, 4800 }, { B9600, 9600 }, { B19200, 19200 }, { B38400, 38400 }, { B57600, 57600 }, { B115200, 115200 }, { B230400, 230400 }, { B460800, 460800 }, { B500000, 500000 }, {
B576000, 576000 }, { B921600, 921600 }, { B1000000, 1000000 }, { B1152000, 1152000 }, { B1500000, 1500000 }, { B2000000, 2000000 }, { B2500000, 2500000 }, { B3000000, 3000000 }, { B3500000, 3500000 },
        { B4000000, 4000000 } };

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
CRIOUARTSerial::CRIOUARTSerial() {
    fileDescriptor = -1;
    speedCode = B19200;
    FD_ZERO(&readFDS);
    FD_ZERO(&readFDS_done);
    FD_ZERO(&writeFDS);
    FD_ZERO(&writeFDS_done);
}

CRIOUARTSerial::~CRIOUARTSerial() {
    Close();
}

bool CRIOUARTSerial::SetSpeed(MARTe::uint32& speed) {
    using namespace MARTe;
    bool ok = (fileDescriptor == -1);
    if (ok) {
        int32 ix = 0;
        while ((speedTable[ix].code != __MAX_BAUD) && (speed > speedTable[ix].speed)) {
            ix++;
        }

        if ((speedTable[ix].speed - speed) > (speed - speedTable[ix - 1].speed)) {
            ix--;
        }

        speedCode = speedTable[ix].code;
        speed = speedTable[ix].speed;

    }
    else {
        int32 ix = 0;
        while (speedTable[ix].code != speedCode) {
            ix++;
        }
        speed = speedTable[ix].speed;

    }
    return ok;
}

bool CRIOUARTSerial::Open(const MARTe::char8* name) {
    using namespace MARTe;
    bool ok = (fileDescriptor == -1);

    if (ok) {
        ok = (name != NULL);
    }

    if (ok) {
        fileDescriptor = open(name, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fileDescriptor == -1) {
            REPORT_ERROR_STATIC(ErrorManagement::OSError, "Cannot open serial %s", name);
            ok = false;
        }
        FD_SET(fileDescriptor, &readFDS);
        FD_SET(fileDescriptor, &writeFDS);
    }

    if (ok) {

        struct termios newtio;
        cfmakeraw(&newtio); // Clean all settings
        newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8 | speedCode; // 8 databits
        newtio.c_cflag |= (CLOCAL | CREAD);
        newtio.c_cflag &= ~(PARENB | PARODD); // No parity
        newtio.c_cflag &= ~CRTSCTS; // No hardware handshake
        newtio.c_cflag &= ~CSTOPB; // 1 stopbit


        //newtio.c_iflag = IGNBRK;
        //            newtio.c_iflag &= ~(IXON | IXOFF | IXANY); // No software handshake
        //newtio.c_iflag |= (IXON | IXOFF | IXANY);  // software handshake
        newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
        newtio.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

        newtio.c_lflag = 0;
        newtio.c_oflag = 0;
        newtio.c_cc[VMIN] = 1;
        newtio.c_cc[VTIME] = 5;

        ok = (cfsetspeed(&newtio, speedCode) == 0);
        if (!ok) {
            REPORT_ERROR_STATIC(ErrorManagement::OSError, "Cannot set serial speed of %s to %d", name, speedCode);
        }

        if (ok) {
            if (tcsetattr(fileDescriptor, TCSANOW, &newtio) == -1) {
                REPORT_ERROR_STATIC(ErrorManagement::OSError, "tcsetattr failed");
                ok = false;
            }
        }
    }

    return ok;
}

void CRIOUARTSerial::Close() {
    using namespace MARTe;
    if (fileDescriptor != -1) {
        int32 err = close(fileDescriptor);
        if (err != 0) {
            REPORT_ERROR_STATIC(ErrorManagement::OSError, "Failed to close serial interface");
        }
        fileDescriptor = -1;
    }
}

bool CRIOUARTSerial::Read(MARTe::char8* buffer, MARTe::uint32 &size) {
    MARTe::int32 readBytes = read(fileDescriptor, buffer, size);
    size = readBytes;
    //REPORT_ERROR_STATIC(MARTe::ErrorManagement::Debug, "Read %d bytes", readBytes);
    bool ok = (readBytes == static_cast<MARTe::int32>(size));
    return ok;
}

bool CRIOUARTSerial::Read(MARTe::char8* buffer, MARTe::uint32 &size, MARTe::uint32 timeoutUsec) {
    using namespace MARTe;
    bool ok = true;
    MARTe::uint32 leftToRead = size;
    void *rbuffer = &buffer[0u];
    while ((leftToRead > 0u) && (ok)) {
        ok = WaitRead(timeoutUsec);
        if (ok) {
            MARTe::int32 readBytes = read(fileDescriptor, rbuffer, leftToRead);
            if (readBytes > 0) {
                rbuffer = &buffer[readBytes];
                leftToRead -= readBytes;
                //REPORT_ERROR_STATIC(MARTe::ErrorManagement::OSError, "Read %d bytes from serial", readBytes);
            }
            else {
                REPORT_ERROR_STATIC(MARTe::ErrorManagement::OSError, "Failed to read from serial %s", strerror(errno));
                ok = false;
            }
        }
    }
    size = (size - leftToRead);

    //RsEPORT_ERROR_STATIC(MARTe::ErrorManagement::OSError, "@END Read %d bytes from serial [%d] ... [%d]", size, temp[0], temp[14]);
    return ok;
}

bool CRIOUARTSerial::Write(MARTe::char8 *buffer, MARTe::uint32 size) {
    MARTe::int32 writtenBytes = write(fileDescriptor, buffer, size);
    //REPORT_ERROR_STATIC(MARTe::ErrorManagement::Debug, "Wrote %d bytes", writtenBytes);
    bool ok = (writtenBytes == static_cast<MARTe::int32>(size));
    return ok;
}

bool CRIOUARTSerial::WaitRead(MARTe::uint32 timeoutUsec) {
    using namespace MARTe;
    struct timeval timeWait;

    timeWait.tv_sec = timeoutUsec / 1000000u;
    timeWait.tv_usec = timeoutUsec % 1000000u;
    readFDS_done = readFDS;
    MARTe::int32 readyCount = select(fileDescriptor + 1, &readFDS_done, NULL_PTR(fd_set *), NULL_PTR(fd_set *), &timeWait);

    return (readyCount > 0);
}

bool CRIOUARTSerial::WaitWrite(MARTe::uint32 timeoutUsec) {
    using namespace MARTe;
    struct timeval timeWait;

    timeWait.tv_sec = timeoutUsec / 1000000u;
    timeWait.tv_usec = timeoutUsec % 1000000u;
    writeFDS_done = writeFDS;
    MARTe::int32 readyCount = select(fileDescriptor + 1, NULL_PTR(fd_set *), &writeFDS_done, NULL_PTR(fd_set *), &timeWait);

    return (readyCount > 0);
}

