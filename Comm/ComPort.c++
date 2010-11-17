/* 
 * File:   ComPort.c++
 * Author: jeremy
 * 
 * Created on November 12, 2010, 3:00 PM
 */

#include "ComPort.h"

/**
* Correctly establishes the correct baud rate, and sets up the signal handler.
 */
ComPort::ComPort(int port)
{
    currentByte_ = 0;
        struct termios oldtio,newtio;
	struct sigaction saio;           /* definition of signal action */
        switch (port){
                case(0):
                    fileDescriptor_ = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
                    break;
                case(1):
                    fileDescriptor_ = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
                    break;
                case(2):
                    fileDescriptor_ = open("/dev/ttyS2", O_RDWR | O_NOCTTY | O_NDELAY);
                    break;
        }
        if (fileDescriptor_ < 0)
	{
		perror("Error opening file:");
		exit(-1);
	}
	/* install the signal handler before making the device asynchronous */
	saio.sa_handler = signalHandler1;
	//saio.sa_mask = 0;
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);

	/* allow the process to receive SIGIO */
	fcntl(fileDescriptor_, F_SETOWN, getpid());
	/* Make the file descriptor asynchronous (the manual page says only
	   O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	fcntl(fileDescriptor_, F_SETFL, FASYNC);


	tcgetattr(fileDescriptor_,&oldtio); /* save current serial port settings */
	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

        /*
          BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
          CRTSCTS : output hardware flow control (only used if the cable has
                    all necessary lines. See sect. 7 of Serial-HOWTO)
          CS8     : 8n1 (8bit,no parity,1 stopbit)
          CLOCAL  : local connection, no modem contol
          CREAD   : enable receiving characters
        */
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;

        /*
          IGNPAR  : ignore bytes with parity errors
          ICRNL   : map CR to NL (otherwise a CR input on the other computer
                    will not terminate input)
          otherwise make device raw (no other input processing)
        */
	newtio.c_iflag = IGNPAR | ICRNL;

 /*
         Raw output.
        */
	newtio.c_oflag = 0;

        /*
          ICANON  : enable canonical input
          disable all echo functionality, and don't send signals to calling program
        */
	newtio.c_lflag = 0;//ICANON;

        /*
          initialize all control characters
          default values can be found in /usr/include/termios.h, and are given
          in the comments, but we don't need them here
        */


        /*
          now clean the modem line and activate the settings for the port
        */
	tcflush(fileDescriptor_, TCIFLUSH);

	if(!tcsetattr(fileDescriptor_,TCSANOW,&newtio))
	{
		printf("Port set successfully.\n");
	}

	else
	{
		printf("There was a problem setting the port!\n");
	}

}

void ComPort::signalHandler()
{
    uchar buff[100];
    int i, ret = read(fileDescriptor_, buff, 99);
    buff[ret] = 0;
    for(i = 0; i < ret; i+=1)
    {
        if (currentByte_ > 22)
        {
            if(isIncomingPacketFile_)
            {
                dataBuffer_[currentByte_ - 23] = buff[i];
            }
            else
            {
                dataBuffer_[currentByte_ - 19] = buff[i];
            }
        }



        if (currentByte_ ==  22)
        {
            if(isIncomingPacketFile_)
            {
                blockId_[0] = buff[i];
            }
            else
            {
                dataBuffer_[3] = buff[i];
            }

        }
        if (currentByte_ == 21)
        {
            if(isIncomingPacketFile_)
            {
                blockId_[1] = buff[i];
            }
            else
            {
                dataBuffer_[2] = buff[i];
            }

        }


        if (currentByte_ == 20)
        {
            if(isIncomingPacketFile_)
            {
                blockId_[2] = buff[i];
            }
            else
            {
                //printf("\nLength: %u\n",buff[i]);
                incomingPacketLength_ = (int)buff[i];

                dataBuffer_[1] = buff[i];
            }

        }


        if (currentByte_ == 19)
        {
            if(isIncomingPacketFile_)
            {
                sessionId_ = buff[i];
            }
            else
            {
                dataBuffer_[0] = buff[i];
            }
        }
        if (currentByte_ == 18) //first byte of our packet
        {
            //printf("\nCharacter 18: %.2x\n", buff[i]);
            if (buff[i] > 0x80)
            {
                //printf("\nFile Packet Detected.\n");
                isIncomingPacketFile_ = true;
                incomingPacketLength_ = (int)buff[i] - 128;
            }
            else
            {
                //printf("\nCommand Packet Detected.\n");
                isIncomingPacketFile_ = false;
                //printf("Destination: %.2x\n", buff[i]);
                destination_ = buff[i];
            }
        }

        if (currentByte_ != 0)
        {
            //printf("%.2x %i\n", buff[i], currentByte_);
            currentByte_++;
            //if (buff[i] == 0xC0)
            //{

                if(((currentByte_ == incomingPacketLength_+ 22) && (!isIncomingPacketFile_))||((currentByte_ == incomingPacketLength_+ 24) && (isIncomingPacketFile_)))
                //if(currentByte_ > incomingPacketLength_ + 21)
                {
                    //TODO: Check what type of packet,Load dataBuffer into queue
                    //printf("\nPacket Completed. isFile?%i length:%i \n", isIncomingPacketFile_, incomingPacketLength_);
                    isFilePacket = isIncomingPacketFile_;
                    ses = (int)sessionId_;
                    block = (int)blockId_[0];
                    leng = incomingPacketLength_;


                    tempDataBuffer = dataBuffer_;
                    dataBuffer_ = NULL;
                    tempDataBuffer[incomingPacketLength_+2]=0x00;
                    command_received = true;
                    currentByte_ = 0;
                    incomingPacketLength_ = 0;

                }
                //else
                //{
                    //TODO: this is an error point
                 //   printf("\nUnexepected end of packet.\n");
                 //   currentByte_ = 0;

                //}
            //}
        }
        else if(buff[i] == 0xC0)
        {
            dataBuffer_ = new uchar[64];
            //printf("\nBeginning %.2x %i\n", buff[i], currentByte_);
            currentByte_++;
        }

        if (isIncomingPacketFile_)
        {
            if (currentByte_> incomingPacketLength_ + 23)
            {

                //printf("\nFile Packet Completed.\n");
                currentByte_ = 0;

            }
        }
    }
}

int ComPort::sendCommandPacket(uchar* buffer, int length)
{
    write(fileDescriptor_, buffer, length);

}

ComPort::~ComPort()
{
    close(fileDescriptor_);
}


