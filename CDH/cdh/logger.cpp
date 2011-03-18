/* File:    logger.cpp
 * Author:  matt
 * Purpose: Implements logging class.
 */

#include <stdio.h>
#include "logger.h"
using namespace std;

extern bool printDiag;
extern bool printError;
extern pthread_mutex_t printLock;

Logger::Logger(string error_f, string event_f, string diag_f)
{
    errorFilename = error_f;
    eventFilename = event_f;
    diagFilename  = diag_f;

    /*
    errorLock = PTHREAD_MUTEX_INITIALIZER;
    eventLock = PTHREAD_MUTEX_INITIALIZER;
    diagLock  = PTHREAD_MUTEX_INITIALIZER;
    */
}

Logger::~Logger()
{
    /* Matt: All pthread items need to be destoryed at some point. */
    pthread_mutex_destroy(&errorLock);
    pthread_mutex_destroy(&eventLock);
    pthread_mutex_destroy(&diagLock);
}

void Logger::Error(int line, int point, int cond)
{
    /* Matt: TODO: The code for logging an error needs to be completed. Right
     * now it will only print errors if printError is set.
     */
    if(printError)
    {
        pthread_mutex_lock(&printLock);
        printf("[%4i] error %i.%i\n", line, point, cond);
        pthread_mutex_unlock(&printLock);
    }
}

void Logger::Event(string msg, int lineNum)
{
    pthread_mutex_lock(&eventLock);

    // Matt: TODO: The code for logging an event needs to be completed.

    pthread_mutex_unlock(&eventLock);
}

void Logger::Diag(int line, char *msg)
{
    /* Matt: TODO: The code for logging a diagnostice value update needs to be 
     * completed. Right now it will only print messages as if printDiag is set.
     */
   
    if(printDiag)
    {
        pthread_mutex_lock(&printLock);
        printf("[%4i] %s\n", line, msg);
        pthread_mutex_unlock(&printLock);
    }
}