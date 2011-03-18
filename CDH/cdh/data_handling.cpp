#include <stdlib.h>
#include <stdio.h>
#include "data_handling.h"
#include "data_link.h"

extern Logger *log;

/* Matt: Allocates memory for a CommandInfo structure.
 * id   - the id of the command
 * len  - the length of command data present
 * data - the command data buffer, a deep copy is made
 */
CommandInfo *createCommandInfo(uchar id, uchar len, uchar* data)
{
    CommandInfo *cmd = new CommandInfo;
    cmd->id = id;
    cmd->len = len;
    cmd->data = new uchar[len];

    for(int i = 0; i < len; i++)
        cmd->data[i] = data[i];

    return cmd;
}

/* Matt: Deallocates memory held by a CommandInfo structure.
 * cmd - the pointer to the command structure to deallocate
 */
void destroyCommandInfo(CommandInfo *cmd)
{
    /* Matt: First insure cmd->data has been deallocated. */
    delete [] cmd->data;
    delete cmd;
}

/* Matt: Allocates memory for a ScheduleEntry structure.
 * tte - the system time in which to execute the command
 * cmd - the pointer to the command which should be executed
 */
ScheduleEntry *createScheduleEntry(int tte, CommandInfo *cmd)
{
    ScheduleEntry *sch = new ScheduleEntry;
    sch->tte = tte;
    sch->cmd = cmd;
    return sch;
}

/* Matt: Deallocates memory held by a ScheduleEntry structure.
 * sch - the pointer to the schedule ScheduleEntry structure to deallocate
 */
void destroyScheduleEntry(ScheduleEntry *sch)
{
    /* Matt: First ensure the command is deallocated. */
    destroyCommandInfo(sch->cmd);

    /* Matt: Deallocate the ScheduleEntry structure. */
    delete sch;
}

/* Matt: Allocates memory for a FileTransInfo structure.
 * sessId  - the session ID of the transfer
 * len     - the number of data bytes in the file transfer packet
 * data    - the pointer to the buffer holding the packet data
 *           (a deep copy of this data is made)
 * blockId - identifies which block of a file the contained data belongs to
 */
FileTransInfo *createFileTransInfo(uchar sessId,
                                   uchar len,
                                   uchar *data,
                                   uint  blockId)
{
    FileTransInfo *trans = new FileTransInfo;
    trans->sessId  = sessId;
    trans->len     = len;
    trans->blockId = blockId;
    trans->data = new uchar[len];

    for(int i = 0; i < len; i++)
        trans->data[i] = data[i];

    return trans;
}

/* Matt: Deallocates memory held by a FileTransInfo structure.
 * trans - the pointer to the FileTransInfo structure to deallocate
 */
void destroyFileTransInfo(FileTransInfo *trans)
{
    /* Matt: First insure trans->data has been deallocated. */
    delete [] trans->data;

    /* Matt: Deallocate the FileTransInfo structure. */
    delete trans;
}

/* Matt: Executive thread function implimentation. */
void *exec_func(void *arg)
{
    DataHandler dataHandler;

    log->Diag(__LINE__, "Exec: Ready!");

    /* Matt: The executive function's main loop.
     * Matt: TODO: Something needs to break this loop. For testing purposes,
     * its left infinite.
     */
    while(true)
    {
        log->Diag(__LINE__, "Exec: Data handling starting...");

        dataHandler.HandleData();
        
        log->Diag(__LINE__, "Exec: Data handleing finished.");
    }
}

/* -------------------------------------------------------------------------- */
/* class DataHandler implementation */

DataHandler::DataHandler()
{
    /* Matt: Create the low speed data link. */
    lowLink = LowSpeedLink::GetInstance();
    
    /* Matt: Just for testing. */
    if(lowLink->Open() != DLINK_NONE)
    {
        log->Diag(__LINE__, "Exec: Error opening the low speed data link!");
        exit(1);
    }
}

DataHandler::~DataHandler()
{

}

void DataHandler::HandleData()
{
    log->Diag(__LINE__, "Exec: Waiting for data...");
    // Matt: Wait for data to become available in one of the three queues.
    sem_wait(&execHalt);

    // Matt: Check for and handle schedule queue elements.
    if(checkScheduleQueue())      handleScheduleQueue();
    else if(checkCommandQueue())  handleCommandQueue();
    else if(checkFileQueue())     handleFileQueue();

    /* --- Start Error DP 2 --- */
    /* -- Error Cond 1 -- */
    else
    {
        /* Matt: This error occured because the execHalt semaphore was > 0
         * while no queue data needed to be handled. The execHalt semaphore
         * should only be > 0 when there is queue data ready to be handled.
         */
        /* Matt: TODO: Determine if any action should be taken when this
         * error occurs.
         */
        log->Error(__LINE__, 2, 1);
    }
    /* --- Stop Error DP 2 --- */

}

void DataHandler::handleScheduleQueue()
{
    log->Diag(__LINE__, "Exec: Handling schedule data...");
}

void DataHandler::handleCommandQueue()
{
    log->Diag(__LINE__, "Exec: Handling command data...");
    CommandInfo *info = cmdQueue.front();
    cmdQueue.pop();

    char msg[200];
    sprintf(msg, "Exec: Command -> ID:0x%X LEN:0x%X DATA:0x%X", info->id, info->len, info->data[0]);
    log->Diag(__LINE__, msg);
    log->Diag(__LINE__, "Exec: Deleteing command data...");
    destroyCommandInfo(info);
    log->Diag(__LINE__, "Exec: Command data deleted.");
    info = 0;
}

void DataHandler::handleFileQueue()
{
    log->Diag(__LINE__, "Exec: Handling file data...");
    FileTransInfo *info = fileQueue.front();
    fileQueue.pop();

    char msg[200];
    sprintf(msg, "Exec: File -> SESS:0x%X LEN:0x%X DATA:0x%X BLOCK:0x%X", info->sessId, info->len, info->data[0], info->blockId);
    log->Diag(__LINE__, msg);
    log->Diag(__LINE__, "Exec: Deleteing file data...");
    destroyFileTransInfo(info);
    log->Diag(__LINE__, "Exec: File data deleted.");
    info = 0;
}

bool DataHandler::checkScheduleQueue()
{
    // Matt: NOTE: If a queue is empty, queue->empty() returns true
    return !schQueue.empty();
}

bool DataHandler::checkCommandQueue()
{
    return !cmdQueue.empty();
}

bool DataHandler::checkFileQueue()
{
    return !fileQueue.empty();
}