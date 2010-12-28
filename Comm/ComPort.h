/**
 * @file   ComPort.h
 * Contains the prototype for the ComPort class.
 * @author
 * Jeremy Guillory
 * @date
 * Created on November 12, 2010, 3:00 PM
 */
#ifndef CAPE_CDH_COMPORT_H
#define CAPE_CDH_COMPORT_H


#ifndef uchar
#define uchar unsigned char
#endif
#define BAUDRATE B9600

#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/signal.h>

#include <fstream>
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::fstream;



//this is just for testing!
extern uchar* tempDataBuffer;
extern bool isCommandReceived;
extern void signalHandler1(int status);

/**
 * A class to handle data transfer over a serial port.
 *
 * This Linux specific class properly configures a serial port to handle USART
 * communication between the CDH subsystem of CAPE2 and the Power,
 * Communication, and Software Defined Radio subsystems.
 */
class ComPort{
public:

    ///Parameterized constructor that will support opening multiple serial
    ///ports.
    ComPort(int);

    ///Handles incoming data on the serial port.
    void signalHandler();

    ///Sends data to the corresponding serial port for transmission.
    int sendPacket(uchar* buffer, int length);

    ///Standard destructor function.
    ~ComPort();

private:

    ///Member variable that references the port to be opened.
    int fileDescriptor_;

    ///Keeps track of which byte of a packet is being currently transmitted.
    int currentByte_;

    ///Buffer which stores incoming packet data.
    uchar* dataBuffer_;

    ///Used to reference the total length of an incoming packet.
    uchar incomingPacketLength_;

    ///Stores the session which the incoming file packet belongs to.
    uchar sessionId_;

    ///Holds the identifier of a subsystem that the incoming command will be
    ///redirected to upon completion.
    uchar destination_;

    ///Contains the identifier for which block of a file is being currently
    ///received.
    uchar blockId_[3];

    ///Indicates if the current packet is a file or a command.
    bool isIncomingPacketFile_;
};


#endif	/* CAPE_CDH_COMPORT_H */

