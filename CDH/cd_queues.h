/* 
 * File:   recieve_queues.h
 * Author: matt
 *
 * Created on January 27, 2011, 9:12 PM
 */

#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include "data_types.h"
using namespace std;

#ifndef _RECIEVE_QUEUES_H
#define	_RECIEVE_QUEUES_H

/* This class encapsulates the command and data queues.
 *
 * All classes which need access to the CDQs (Command and Data Queues) will need
 * to inherit this class.
 */
class CDQueues
{
public:
    /* Initialize data members. */
    static void Init();

protected:
    /* The command and data queues. */
    static sem_t                 execHalt;  // Data availability semaphore.
    static queue<CommandInfo*>   cmdQueue;  // The command recieve queue.
    static queue<ScheduleEntry*> schQueue;  // The schedule queue.
    static queue<FileTransInfo*> fileQueue; // The file transfer queue.
};


#endif

