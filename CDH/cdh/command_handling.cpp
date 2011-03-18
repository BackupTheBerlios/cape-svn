/* File:    command_handling.cpp
 * Author:  matt
 * Purpose: Implements the command handler class.
 */

#include "command_handling.h"

/* Matt: initialize data members here */
pthread_mutex_t CommandHandler::busLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t CommandHandler::cdhLock = PTHREAD_MUTEX_INITIALIZER;

int CommandHandler::Dispatch(CommandInfo *cmd)
{
    /* Matt: The command is for the CDH subsystem. */
    if(cmd->dest == DEST_CDH)
    {
        /* Matt: Lock access to internal CDH resources. */
        pthread_mutex_lock(&cdhLock);



        /* Matt: Unlock access to internal CDH resources. */
        pthread_mutex_lock(&cdhLock);
    }

    /* Matt: The command is for another subsystem. */
    else
    {
        /* Matt: Lock access to the command bus. */
        pthread_mutex_lock(&busLock);

        

        /* Matt: Unlock access to the command bus. */
        pthread_mutex_lock(&busLock);
    }
}