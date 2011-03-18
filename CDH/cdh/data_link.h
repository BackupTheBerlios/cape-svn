/* 
 * File:   data_link.h
 * Author: matt
 *
 * Created on January 22, 2011, 11:51 AM
 */

#include <termios.h>
#include <fcntl.h>
#include <sys/signal.h>
#include "data_types.h"
#include "cd_queues.h"

#ifndef _DATA_LINK_H
#define	_DATA_LINK_H

/* Error return codes for a data link. */
#define DLINK_NONE        0 // no error occured
#define DLINK_OPEN_FAILED 1 // the opening process of a data link failed
#define DLINK_REOPEN      2 // the data link was already open
#define DLINK_FULL        3 // recieve buffer is full
#define DLINK_RECLOSE     4 // the data link was already closed

/* Data link interface.
 *
 * This is an abstract class which class implements a data link interface to the
 * communication subsystem or SDR.
 */
class DataLink
{
public:

    /* The class constructor. */
    //DataLink() { };

    /* The class destructor. */
    //~DataLink();

    /* Send data down the link. */
    virtual int Send(uchar*, int) = 0;

    /* Open the data link and make it active. */
    virtual int Open() = 0;

    /* Close the data link and make it inactive. */
    virtual int Close() = 0;
};


/* The low speed data link interface.
 *
 * This class implements the data link interface to the low speed modem.
 */

class LowSpeedLink : public DataLink, public CDQueues
{
public:
    /* The class destructor. */
    ~LowSpeedLink();

    /* Send data down the link. */
    int Send(uchar*, int);

    /* Open the data link and make it active. */
    int Open();

    /* Close the data link and make it inactive. */
    int Close();

    /* Get the singleton instance pointer for this class. */
    static DataLink* GetInstance();

private:
    /* The class constructor. */
    LowSpeedLink();

    /* The recieve handler for the low speed tty device. */
    static void Handler(int);

    /* data members related to the tty device */
    static struct termios    term; // the tty device configuration
    static struct sigaction  sig;  // defines the signal for a character recieve
    static int               fd;   // The file descriptor for the low speed tty.

    /* data memebers related to the singleton */
    static bool           notInst; // indicates if instantiation has not occured
    static LowSpeedLink*  self;    // the singleton instance pointer
};

#endif
