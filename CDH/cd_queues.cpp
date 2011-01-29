
#include "cd_queues.h"

sem_t CDQueues::execHalt;
queue<CommandInfo*>   CDQueues::cmdQueue;
queue<ScheduleEntry*> CDQueues::schQueue;
queue<FileTransInfo*> CDQueues::fileQueue;

void CDQueues::Init()
{
    sem_init(&CDQueues::execHalt, 0, 0);
}