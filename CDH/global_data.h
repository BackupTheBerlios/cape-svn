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
pthread_mutex_t printLock;

// Matt: This sets whether or not error logging event are sent to stdout.
bool printError;

// Matt: This sets whether or not diagnostic logging event are sent to stdout.
bool printDiag;

// Matt: Controls the attempted handling of queued data by the executive thread.
sem_t execHalt;

/* Matt: Controls the logging of information about events, errors, and
 * diagnostics which are observed in the CDH software.
 */
Logger *log;

#endif

