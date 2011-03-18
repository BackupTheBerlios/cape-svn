/* 
 * File:   commands.h
 * Author: matt
 *
 * Created on November 4, 2010, 11:51 AM
 */

#include <pthread.h>
#include "data_types.h"

#ifndef _COMMAND_HANDLING_H
#define	_COMMAND_HANDLING_H

/* Command Destination Identifiers */
#define DEST_CDH  0
#define DEST_COMM 1
#define DEST_PWR  2
#define DEST_SDR  3

/* The interface which dispatches all commands to their respective subsystems
 * on the satellite.
 *
 * NOTE: This is a "static class". There is no point in instantiating it. To
 * access a member simply use CommandHandler::member right in your code.
 * (i.e 'CommandHandler::Dispatch(command)' to dispatch a command)
 */
class CommandHandler
{
public:

    /* Dispatches a command to its respective subsystem. */
    static int Dispatch(CommandInfo*);

private:

    /* synchronizes access to the command bus */
    static pthread_mutex_t busLock;

    /* synchronizes access to internal CDH resources */
    static pthread_mutex_t cdhLock;
};

#endif

