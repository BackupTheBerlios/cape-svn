/* 
 * File:   ComPort.h
 * Author: jeremy
 *
 * Created on November 12, 2010, 3:00 PM
 */

#ifndef uchar
#define uchar unsigned char
#endif
#define BAUDRATE B9600

#include <termios.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifndef _COMPORT_H
#define	_COMPORT_H

//this is just for testing!
extern uchar* tempDataBuffer;
//testing!
extern bool isFilePacket;
//testing!
extern int ses, block, leng;


extern bool command_received;
extern void signalHandler1(int status);

class ComPort{
public:

    ComPort(int);

    void signalHandler();

    int sendFilePacket(uchar* buffer, int length);

    int sendCommandPacket(uchar* buffer, int length);

    ~ComPort();

private:

    int fileDescriptor_;

    int currentByte_;

    uchar* dataBuffer_;

    uchar incomingPacketLength_;

    uchar sessionId_;

    uchar destination_;

    uchar blockId_[3];

    bool isIncomingPacketFile_;

    bool isIncomingPacketCommand_;
};


#endif	/* _COMPORT_H */

