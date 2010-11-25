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
            fileDescriptor_ = open("/dev/ttyS1", O_RDWR | O_NOCTTY | O_NONBLOCK);
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
    buffer[returnedLength] = 0;

    //Parse each byte of data
    for(int i = 0; i < returnedLength; i++)
    {
        fstream file;
        file.open("log", ios::app| ios::out);
        int n;
        char test[30];
        n = sprintf(test,"\nCharacter %i: %.2x\n",currentByte_, buffer[i]);
        file.write(test, n);
        file.close();
        //All header and information collected, simply store byte in buffer
        if (currentByte_ > 22)
        {
            if(isIncomingPacketFile_)
            {
                dataBuffer_[currentByte_ - 23] = buffer[i];
            }
            else
            {
                dataBuffer_[currentByte_ - 19] = buffer[i];
            }
        }

        else if (currentByte_ ==  22)
        {
            if(isIncomingPacketFile_)
            {
                //Store byte of blockId
                blockId_[0] = buffer[i];
            }
            else
            {
                dataBuffer_[3] = buffer[i];
            }
        }

        else if (currentByte_ == 21)
        {
            if(isIncomingPacketFile_)
            {
                //Store byte of blockId
                blockId_[1] = buffer[i];
            }
            else
            {
                dataBuffer_[2] = buffer[i];
            }

        }

        else if (currentByte_ == 20)
        {
            if(isIncomingPacketFile_)
            {
                //Store byte of blockId
                blockId_[2] = buffer[i];
            }
            else
            {
                //Store length of command data
                //printf("\nLength: %u\n",buff[i]);
                incomingPacketLength_ = (int)buffer[i];
                dataBuffer_[1] = buffer[i];
            }

        }

        else if (currentByte_ == 19)
        {
            if(isIncomingPacketFile_)
            {
                //Store session ID
                sessionId_ = buffer[i];
            }
            else
            {
                //This byte represents the command ID
                dataBuffer_[0] = buffer[i];
            }
        }

        else if (currentByte_ == 18) //first byte of our packet
        {
            //printf("\nCharacter 18: %.2x\n", buffer[i]);

            //We determine a file or command packet by the value of the first
            //bit (1 or 0) which corresponds to a hex value of 0x80
            if (buffer[i] > 0x80)
            {
                //File packet

                //printf("\nFile Packet Detected.\n");
                isIncomingPacketFile_ = true;

                //The remaining 7 bits are the length of the file packet, but
                //the 8th digit needs to be removed
                incomingPacketLength_ = (int)buffer[i] - 0x80;
            }
            else
            {
                //Command packet

                //printf("\nCommand Packet Detected.\n");
                isIncomingPacketFile_ = false;

                //The remaining 7 bits are the destinatino of the command
                destination_ = buffer[i];
            }
        }

        //Check for the end of the packet
        if (currentByte_ != 0)
        {
            //printf("%.2x %i\n", buff[i], currentByte_);
            currentByte_++;
                
            if(((currentByte_ == incomingPacketLength_+ 22) &&
                    (!isIncomingPacketFile_))||
                    ((currentByte_ == incomingPacketLength_+ 24) &&
                    (isIncomingPacketFile_)))
            {
                

                //TODO: Check what type of packet,Load dataBuffer into queue
                printf("\nPacket Completed. isFile?%i length:%i \n",
                isIncomingPacketFile_, incomingPacketLength_);
                
                //Load all of the received packets data into queue
                ::isFilePacket = isIncomingPacketFile_;

                ::ses = (int)sessionId_;

                ::block = (int)blockId_[0];

                ::leng = incomingPacketLength_;

                ::tempDataBuffer = dataBuffer_;

                dataBuffer_ = NULL;

                ::tempDataBuffer[incomingPacketLength_+2]=0x00;

                ::isCommandReceived = true;
                //delete [] buffer;
                //buffer = new uchar[100];
                //Prepare for next packet by reinitialiazing
                currentByte_ = 0;
                incomingPacketLength_ = 0;
            }
        }
        //Check for the start of a new packet
        else if(buffer[i] == 0xC0)
        {
            //dataBuffer_ will get sent to a queue, so it must be recreated
            //between every packet
            dataBuffer_ = new uchar[64];
            //printf("\nBeginning %.2x %i\n", buffer[i], currentByte_);
            currentByte_++;
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


