/* 
 * File:   main.cpp
 * Author: matt
 *
 * Created on November 4, 2010, 11:03 AM
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "logger.h"
#include "global_data.h"
#include "data_link.h"
#include "config.h"
using namespace std;
/*
 * 
 */

int main(int argc, char** argv)
{
    /* Matt: Check main function arguments. */
    readArgs(argc, argv);

    if(confAbsent)
    {
        printf("No configuration file was specified.\n");
    }

    readConf(confPath);

    /* -----------------------------------------------------------------------*/
    /* Matt: Start initial diagnostic output. */

    log->Diag(__LINE__, "Starting the CDH subsystem user space software..");

    /* Matt: Indicate what the command line arguments were and the starting
     * state. */
    if(printDiag)
        log->Diag(__LINE__, "printDiag  = true");
    else
        log->Diag(__LINE__, "printDiag  = false");

    if(printError)
        log->Diag(__LINE__, "printError = true");
    else
        log->Diag(__LINE__, "printError = false");

    log->Diag(__LINE__, "----------------------------------------");

    /* -----------------------------------------------------------------------*/
    /* Matt: Local data structure initilization. */
    pthread_t execThread; // The executive thread handle.
    int thread_ret;
    
    /* Matt: Initialize global data. */
    CDQueues::Init();
    log       = new Logger("/home/cdh/log/error",
                           "/home/cdh/log/event",
                           "/home/cdh/log/diag");

    log->Diag(__LINE__, "Attempting to create executive thread...");

    /* Matt: Create the executive thread. */
    thread_ret = pthread_create(&execThread,
                                NULL,
                                exec_func, 
                                NULL);

    /* --- Start Error DP 1 --- */

    if(thread_ret)
    {
        /* Matt: Check if an error occured while creating the executive thread. */
        switch(thread_ret)
        {
            /* -- Error Cond 1 -- */
            /* The system lacked the necessary resources to create another
             * thread, or the system-imposed limit on the total number of threads
             * in a process {PTHREAD_THREADS_MAX} would be exceeded.
             */
            case EAGAIN:
                log->Error(__LINE__, 1, 1);
                /* Matt: TODO: Implement the condition response outlined in the
                 * error document.
                 */
                exit(1);
                break;

            /* -- Error Cond 2 -- */
            /* The caller does not have appropriate permission to set the required
             * scheduling parameters or scheduling policy.
             */
            case EPERM:
                log->Error(__LINE__, 1, 2);
                /* Matt: TODO: Implement the condition response outlined in the
                 * error document.
                 */
                exit(1);
                break;

            /* -- Error Cond 3 -- */
            /* The attributes specified by attr are invalid. */
            case EINVAL:
                log->Error(__LINE__, 1, 3);
                /* Matt: TODO: Implement the condition response outlined in the
                 * error document.
                 */
                exit(1);
                break;

            /* -- Error Cond 4 -- */
            /* Matt: An undefined value was returned by 'pthread_create'. */
             default:
                 log->Error(__LINE__, 1, 3);
                 /* Matt: TODO: Implement the condition response outlined in the
                 * error document.
                 */
                 exit(1);
                 break;
        }
    }

    else
    {
        log->Diag(__LINE__, "Executive thread created successfully.");
    }

    /* --- Stop Error DP 1 --- */


    /* Matt: Program initilization is over. */
    /* -----------------------------------------------------------------------*/
    /* Matt: Begin Health Thread operations. */

    /* Matt: End Health Thread operations. */

    log->Diag(__LINE__, "Joining with executive thread...");

    /* Matt: Wait for the executive thread to end. */
    pthread_join(execThread, NULL);

    log->Diag(__LINE__, "Joined with executive thread.");

    /* Matt: Health Thread operations have ended. */
    /* -----------------------------------------------------------------------*/
    /* Matt: Deinitialize local and global data. */

    delete log;
    log = NULL;

    log->Diag(__LINE__, "Returning from 'main'...");

    return (EXIT_SUCCESS);
}
