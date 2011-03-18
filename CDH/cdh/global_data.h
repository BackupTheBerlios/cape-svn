/* 
 * File:   global_data.h
 * Author: matt
 *
 * Created on November 6, 2010, 7:52 PM
 */

#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include "logger.h"
#include "data_handling.h"

#ifndef _GLOBAL_DATA_H
#define	_GLOBAL_DATA_H

/* Matt: This lock synchronizes output to stdout. */
pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;

// Matt: This sets whether or not error logging event are sent to stdout.
bool printError = false;

// Matt: This sets whether or not diagnostic logging event are sent to stdout.
bool printDiag = false;

// Matt: Controls the attempted handling of queued data by the executive thread.
sem_t execHalt;

/* Matt: Controls the logging of information about events, errors, and
 * diagnostics which are observed in the CDH software.
 */
Logger *log;

/* Matt: Sets whether a configuration file has been specified. */
bool confAbsent = true;

/*Matt: The config file path. */
char confPath[128];

/* The device paths for the low and high speed tty. */
char lowDevice[128];
char hiDevice[128];

#endif

