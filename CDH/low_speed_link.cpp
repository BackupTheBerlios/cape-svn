#include <queue>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "data_link.h"
#include "logger.h"
#include "data_handling.h"
#include "data_types.h"

/* Defines the string literal for the low speed modem's serial port device.
 * Matt: TODO: This needs to be changed to the correct value.
 */
#define LOWSPEED_TTY "/dev/pts/2"

/* Defines the baud rate of the low speed data link. */
#define LOWSPEED_BAUD B9600

/* Matt: The static data member of LowSpeedLink have to be defined. */
struct termios   LowSpeedLink::term;
struct sigaction LowSpeedLink::sig;
int              LowSpeedLink::fd;
bool             LowSpeedLink::notInst = true;
LowSpeedLink    *LowSpeedLink::self    = NULL;

void LowSpeedLink::Handler(int status)
{
    /* Matt: Just for testing! */
    char buff[100];

    /* Matt: Ensure the file descriptor for the low speed device is valid.
     * If lsfd = 0, then it isn't valid. Return from the handler.
     */
    if(fd == 0) return;

    /* Matt: Read all avaiable characters on the tty device. */
    int ret = read(fd, buff, 64);

    for(int i = 0; i < ret; i+=1)
    {

        /*--------------------------------------------------------------------*/
        /* Matt: This section is just for testing. */

        if(buff[i] == 'x') exit(1);

        /* Matt: This is a rough example of steps which need to be taken when a
         * complete command packet has been recieved.
         */
        else if(buff[i] == 'c')
        {
            /* Matt: The "pretend" command. */
            uchar id      = 0x55;    // command id
            uchar len     = 0x01;    // length of the command data
            uchar data[1] = {0xAA};  // command data

            /* Matt: Each of these three steps MUST be carried out for each
             * command packet recieved.
             *
             * 1.) Create a new commmand info structure in which each piece of
             *     the command will be packed.
             *     NOTE: createCommandInfo makes a DEEP COPY of the command data
             * 2.) Push the pointer to the command info structure into the
             *     command queue.
             * 3.) Increment the data availability semaphore.
             */
            CommandInfo *info = createCommandInfo(id, len, data); // step 1
            cmdQueue.push(info);                                  // step 2
            sem_post(&execHalt);                                  // step 3
        }

        /* Matt: This is a rough example of steps which need to be taken when a
         * complete file transfer packet has been recieved.
         */
        else if(buff[i] == 'f')
        {
            /* Matt: The "pretend" command. */
            uchar sess    = 0x55;    // session id
            uchar len     = 0x01;    // length of the file transfer data
            uchar data[1] = {0xAA};  // file transfer data
            uchar block   = 0x03;    // block id

            /* Matt: Each of these three steps MUST be carried out for each
             * file transfer packet recieved.
             *
             * 1.) Create a new file transfer info structure in which each piece
             *     of the file transfer will be packed.
             *     NOTE: createFileTransInfo makes a DEEP COPY of the file
             *           transfer data
             * 2.) Push the pointer to the file transfer info structure into the
             *     file queue.
             * 3.) Increment the data availability semaphore.
             */
            FileTransInfo *info = createFileTransInfo(sess, len,    // step 1
                                                      data, block);
            fileQueue.push(info);                                   // step 2
            sem_post(&execHalt);                                    // step 3
        }

        /* Matt: End of the testing section. */
        /*--------------------------------------------------------------------*/
    }
}

LowSpeedLink::LowSpeedLink()
{

}

LowSpeedLink::~LowSpeedLink()
{

}

int LowSpeedLink::Open()
{
    // ensure the low speed tty device isn't already open
    if(fd) return DLINK_REOPEN;

    // attempt to open the tty device
    fd = open(LOWSPEED_TTY, O_RDWR | O_NOCTTY | O_NDELAY);
    if(!fd)
    {
        // can't open the tty device, return the appropriate error code
        return DLINK_OPEN_FAILED;
    }

    /* configure the port
     * NOTE: The signal handler must be set up before the device can be made
     * asynchronous.
     */
    //signalAct.sa_handler = lowSpeedHandler;
    sig.sa_handler = LowSpeedLink::Handler;
    sig.sa_flags = 0;
    sig.sa_restorer = NULL;
    sigaction(SIGIO,&sig,NULL);

    //Allow the process to receive SIGIO
    fcntl(fd, F_SETOWN, getpid());

    //Make the port asynchronous
    fcntl(fd, F_SETFL, FASYNC);

    //Save current serial port settings
    //tcgetattr(fileDescriptor_,&oldtio);

    //Clear struct for new settings
    bzero(&term, sizeof(term));

    /*BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
     *CRTSCTS : output hardware flow control (only used if the cable has all
     *          necessary lines. See sect. 7 of Serial-HOWTO)
     *CS8     : 8n1 (8bit,no parity,1 stopbit)
     *CLOCAL  : local connection, no modem contol
     *CREAD   : enable receiving characters
     */
    term.c_cflag = LOWSPEED_BAUD | CS8 | CLOCAL | CREAD;

    /*IGNPAR  : ignore bytes with parity errors
     *ICRNL   : map CR to NL (otherwise a CR input on the other computer
     *           will not terminate input)otherwise make device raw (no other
     *          input processing)
     */
    term.c_iflag = IGNPAR;

    //Raw output
    term.c_oflag = 0;

    //Enable canonical input
    term.c_lflag = 0;

    //Configure time out values
    term.c_cc[VMIN]=1;
    term.c_cc[VTIME]=0;

    //Flush the modem line
    tcflush(fd, TCIFLUSH);

    //Activate the settings for the port

    /*
    if(!tcsetattr(fd,TCSANOW,&term))
    {
        //TODO: Remove after done testing
        printf("Port set successfully.\n");
        exit(1);
    }
    else
    {
        //TODO: Error point here, couldn not open the serial port
        printf("There was a problem setting the port!\n");
        exit(1);
    }
    */
   

    return DLINK_NONE;
}

int LowSpeedLink::Close()
{
    /* Ensure that the file descriptor for the low speed tty device is open
     * before trying to close it.
     */
    if(fd)
    {
        close(fd);
        return DLINK_NONE;
    }

    else
    {
        // the low speed tty device was already closed
        return DLINK_RECLOSE;
    }
}

int LowSpeedLink::Send(uchar *buff, int size)
{
    // Matt: TODO: Needs to be implimented.
    return 0;
}

DataLink *LowSpeedLink::GetInstance()
{
    if(notInst)
    {
        notInst = false;
        self    = new LowSpeedLink();
    }
    
    return self;
}
