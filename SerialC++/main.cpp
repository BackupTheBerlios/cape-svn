/**
 * @file
 * main.cpp
 * 
 * @author
 * Jeremy Guillory
 *
 * @date
 * Last modified on September 14, 2010, 11:30 AM
 */

//test NEW DIFFERENCES

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B9600

/**
 *Handles signals generated by serial communication.
 *
 * Checks if valid data is coming across the serial port, and buffers it into
 * an array so that it may be decoded.
 *
 * @param status
 * int representing the signal number.
 *
 *
 */
void signal_handler_IO (int status);
int sig = 0;
int fd;

int command_received = 0;
char command[5];


/**
 * Configures the serial port.
 */
void configure_port(void);

int main(int argc, char** argv)
{

    configure_port();
    write(fd, "Starting session...\n\r", 21);

	while(command_received != 1)
	{
            sleep(100);
            if (command_received > 1)
            {
                printf("%s\n", command);
                if (command_received == 3)
                {
                    close(fd);
                    system("sudo /sbin/getty ttyUSB0 9600 2>>/home/jeremy/NetBeansProjects/SerialCommunication/stderr");
                    configure_port();
                }

                command_received = 0;
                fflush(stdout);
            }

        }
	printf("Done.\n");
	return 0;
}
/**
* Correctly establishes the correct baud rate, and sets up the signal handler.
 */
void configure_port()
{
	struct termios oldtio,newtio;
	struct sigaction saio;           /* definition of signal action */
	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0)
	{
		perror("Error opening file:");
		exit(-1);
	}

	/* install the signal handler before making the device asynchronous */
	saio.sa_handler = signal_handler_IO;
	//saio.sa_mask = 0;
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);

	/* allow the process to receive SIGIO */
	fcntl(fd, F_SETOWN, getpid());
	/* Make the file descriptor asynchronous (the manual page says only
	   O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	fcntl(fd, F_SETFL, FASYNC);


	tcgetattr(fd,&oldtio); /* save current serial port settings */
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
	tcflush(fd, TCIFLUSH);

	if(!tcsetattr(fd,TCSANOW,&newtio))
	{
		printf("Port set successfully.\n");
	}

	else
	{
		printf("There was a problem setting the port!\n");
	}


}



void signal_handler_IO (int status)
{
        char buff[100];
	int i, ret = read(fd, buff, 99);
	buff[ret] = 0;

	for(i = 0; i < ret; i+=1)
	{

             if ((-6<command_received)&&(command_received<0))
             {
                 //printf("%i", command_received);

                 //printf("%c", buff[i]);

                 command[abs(command_received)-1] = buff[i];

                 command_received--;
             }
             if (buff[i] == '$')
                 command_received = -1;

             if (command_received < -5)
             {
                 if (!strncmp(command, "time1",5))
                     command_received = 1;
                 else if (!strncmp(command, "gsens",5))
                     command_received = 3;
                 else
                     command_received = 2;
             }

	}

        fflush(stdout);

}

