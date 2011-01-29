/* 
 * File:   logger.h
 * Author: matt
 *
 * Created on November 4, 2010, 11:38 AM
 */

#include <string>
#include <pthread.h>
using namespace std;

#ifndef  _LOGGER_H
#define	 _LOGGER_H

class Logger
{
public:
    Logger(string error_f, string event_f, string diag_f);
    ~Logger();
    
    void Error(int, int,int);
    void Event(string, int);
    void Diag(int, char*);

private:
    pthread_mutex_t errorLock, eventLock, diagLock;
    string errorFilename, eventFilename, diagFilename;
};

#endif