/* File:    cd_queue.cpp
 * Author:  matt
 * Purpose: Implements the static base class which controls and distributes
 *          access to the command and data queues.
 */

#include "cd_queues.h"

sem_t CDQueues::execHalt;
queue<CommandInfo*>   CDQueues::cmdQueue;
queue<ScheduleEntry*> CDQueues::schQueue;
queue<FileTransInfo*> CDQueues::fileQueue;

void CDQueues::Init()
{
    sem_init(&CDQueues::execHalt, 0, 0);
}