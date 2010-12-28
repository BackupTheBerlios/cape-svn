/**
 * @file   ComPort.c++
 * Contains the implementation of the ComPort class.
 * @author
 * Jeremy Guillory
 * @date
 * Created on November 12, 2010, 3:00 PM
 */
#include "ComPort.h"

/**
 * Parameterized constructor which establishes the correct configuration for the
 * port and sets up the signal handler to trigger on incoming data.
 */
ComPort::ComPort(int port)
{
    currentByte_ = 0;
    dataBuffer_ = new uchar[64];
    struct termios newtio;
    struct sigaction saio;
    
    //Open the port of the parameter passed to the constructor
    switch (port)
    {
        case(0):
            fileDescriptor_ = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
            break;
        case(1):
            //For testing purposes on a laptop with a USB to serial dongle
            fileDescriptor_ = open("/dev/ttyS1", O_RDWR | O_NOCTTY | O_NDELAY);
            break;
        case(2):
            fileDescriptor_ = open("/dev/ttyS2", O_RDWR | O_NOCTTY | O_NDELAY);
            break;
        default:
            //TODO: Error point, tried to specify a port which does no exist
            break;
    }

    //Configure the port

    //Install the signal handler before making the device asynchronous
    saio.sa_handler = signalHandler1;
    saio.sa_flags = 0;
    saio.sa_restorer = NULL;
    sigaction(SIGIO,&saio,NULL);
    
    //Allow the process to receive SIGIO
    fcntl(fileDescriptor_, F_SETOWN, getpid());
    
    //Make the port asynchronous
    fcntl(fileDescriptor_, F_SETFL, FASYNC);

    //Save current serial port settings
    //tcgetattr(fileDescriptor_,&oldtio);
    
    //Clear struct for new settings
    bzero(&newtio, sizeof(newtio));

    /*BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
     *CRTSCTS : output hardware flow control (only used if the cable has all
     *          necessary lines. See sect. 7 of Serial-HOWTO)
     *CS8     : 8n1 (8bit,no parity,1 stopbit)
     *CLOCAL  : local connection, no modem contol
     *CREAD   : enable receiving characters
     */
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;

    /*IGNPAR  : ignore bytes with parity errors
     *ICRNL   : map CR to NL (otherwise a CR input on the other computer
     *           will not terminate input)otherwise make device raw (no other
     *          input processing)
     */
    newtio.c_iflag = IGNPAR;

    //Raw output
    newtio.c_oflag = 0;

    //Enable canonical input
    newtio.c_lflag = 0;

    //Configure time out values
    newtio.c_cc[VMIN]=1;
    newtio.c_cc[VTIME]=0;

    //Flush the modem line
    tcflush(fileDescriptor_, TCIFLUSH);

    //Activate the settings for the port
    if(!tcsetattr(fileDescriptor_,TCSANOW,&newtio))
    {
        //TODO: Remove after done testing
        printf("Port set successfully.\n");
    }
    else
    {
        //TODO: Error point here, couldn not open the serial port
        printf("There was a problem setting the port!\n");
    }
}

/**
 * Equivalent of an interrupt service routine in Linux.  Interrupts in Linux are
 * implemented using ANSI C, so handlers must not be member functions.  To
 * maintain encapsulation, this member function is called from within the
 * C-style handler function.
 */
void ComPort::signalHandler()
{

    //Buffer which will read characters out of the receive buffer, allowing
    //for an entire packet to be buffered before this handler is called with
    //no problems
    uchar buffer[100];

    //Read data off the port, and the number of bytes read is stored in
    //returnedLength
    int returnedLength = read(fileDescriptor_, buffer, 99);

    //Null terminate the buffer
    //buffer[returnedLength] = 0;

  
    //Parse each byte of data
    for(int i = 0; i < returnedLength; i++)
    {
        //printf("%i %i %2x\n",currentByte_, returnedLength,buffer[i]);
        dataBuffer_[currentByte_] = (uchar)buffer[i];
        currentByte_++;
        if(currentByte_ == 64)
        {
            ::tempDataBuffer = dataBuffer_;
            dataBuffer_ = NULL;
            dataBuffer_ = new uchar[64];
            currentByte_ = 0;
            ::isCommandReceived = true;
        }
   
    }
}

/**
 * Writes 'length' amount of bytes from buffer to the port.  The data in buffer
 * must be properly formatted using protocols outlined in the CDH Protocols
 * document before being this function is called.
 */
int ComPort::sendPacket(uchar* buffer, int length)
{

    write(fileDescriptor_, buffer, length);

}

/**
 * Deletes all dynamically created data and closes the serial port.
 */
ComPort::~ComPort()
{
    delete[] dataBuffer_;
    delete[] blockId_;
    close(fileDescriptor_);
}


