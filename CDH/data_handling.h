/* 
 * File:   data_handling.h
 * Author: matt
 *
 * Created on November 4, 2010, 11:49 AM
 */

#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include "data_types.h"
#include "logger.h"
#include "data_link.h"
#include "cd_queues.h"
#include "data_types.h"
using namespace std;

#ifndef _DATA_HANDLING_H
#define	_DATA_HANDLING_H

/* -------------------------------------------------------------------------- */
/* functions for the CDQ support structures. */

CommandInfo *createCommandInfo(uchar, uchar, uchar*);
void destroyCommandInfo(CommandInfo*);

ScheduleEntry *createScheduleEntry(int, CommandInfo*);
void destroyScheduleEntry(ScheduleEntry*);

FileTransInfo *createFileTransInfo(uchar, uchar, uchar*, uint);
void destroyFileTransInfo(FileTransInfo*);


/* -------------------------------------------------------------------------- */
/* Data structures and functions for the executive thread. */


/* Matt: This structure is used to hold paramaters which need to be passed to
 * the executive thread.
 */
struct ExecThreadParams
{
    sem_t *queueCtrl;                  // pointer to the queue control semaphore
    Logger *log;                       // pointer to the logging interface
    queue<CommandInfo*> *cmdQueue;     // pointer to the command queue
    queue<ScheduleEntry*> *schQueue;   // pointer to the schedule queue
    queue<FileTransInfo*> *fileQueue;  // pointer to the file queue
};

void *exec_func(void *);

/* -------------------------------------------------------------------------- */
/* class DataHandler definition. */


/* This class encapsulates all the data and operations needed to implement
 * all the data handling operations of the executive thread.
 */
class DataHandler : public CDQueues
{
public:
    
    /* The constructor. It's passed pointers to all external data the class
     * might need to access.
     */
    DataHandler();

    /* The destructor. */
    ~DataHandler();

    /* Makes the class handle one element from one of the three data queues. */
    void HandleData();
    
private:

    /* Handles schedule queue data. */
    void handleScheduleQueue();

    /* Handles command queue data. */
    void handleCommandQueue();

    /* Handles file queue data. */
    void handleFileQueue();

    /* Returns true if data on the schedule queue needs to be handled. */
    bool checkScheduleQueue();

    /* Returns true if data on the command queue needs to be handled. */
    bool checkCommandQueue();

    /* Returns true if data on the file queue needs to be handled. */
    bool checkFileQueue();

    DataLink *lowLink;
};


#endif