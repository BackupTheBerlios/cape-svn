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
#define LOWSPEED_TTY "/dev/pts/4"

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

    /* Matt: This is just for testing! */
    for(int i = 0; i < ret; i+=1)
    {
        if((buff[i] >= 97) && (buff[i] <= 122))
        {
            /*
            char ch[2] = {0, 0};
            ch[0] = buff[i];
            printf("%s", ch);
            fflush(stdout);
            */

            if(buff[i] == 'x') exit(1);

            else if(buff[i] == 'c')
            {
                uchar *data = new uchar[1];
                data[0] = 0xAA;
                CommandInfo *info = createCommandInfo(0x55, 0x1, data);
                cmdQueue.push(info);
                sem_post(&execHalt);
            }

            else if(buff[i] == 'f')
            {
                uchar *data = new uchar[1];
                data[0] = 0xAA;
                FileTransInfo *info = createFileTransInfo(0, 0, data, 0);
                fileQueue.push(info);
                sem_post(&execHalt);
            }
        }
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
