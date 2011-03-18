/* File:    data_types.h
 * Author:  matt
 * Purpose: Defines data types used throughout the source code.
 *
 * Created on November 6, 2010, 7:05 PM
 */

#ifndef _DATA_TYPES_H
#define	_DATA_TYPES_H

typedef unsigned char uchar;
typedef unsigned int  uint;

/* Matt: This structure contains all information needed about a command. */
struct CommandInfo
{
    uchar dest,    /* command destination */
          id,      /* command id */
          len,     /* command data length */
          *data;   /* command data buffer */

};

/* Matt: This structure contains all information needed about a scheduled
 * command.
 */
struct ScheduleEntry
{
    int tte;          /* time to execute, in terms of the system time */
    CommandInfo *cmd; /* the scheduled command */
};

/* Matt: This structure contains all information needed about file transfers. */
struct FileTransInfo
{
    uchar sessId, len;
    uchar *data;
    uint  blockId;
};

#endif
