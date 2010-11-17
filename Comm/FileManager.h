/* 
 * File:   FileManager.h
 * Author: jeremy
 *
 * Created on November 12, 2010, 2:06 PM
 */
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef _FILEMANAGER_H
#define	_FILEMANAGER_H

//iostream for testing purposes only!!!!
#include <iostream>
using std::cout;

#include <stdio.h>
#include <pthread.h>
#include <string>
#include <fstream>
#include "ComPort.h"
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;

extern ComPort* port1;
extern const int MAX_PACKET_SIZE;

class FileManager {
public:

    FileManager();

    void writeBlock(int sessionId, int blockId, int length, uchar* data);
    
    void readBlock(uchar sessionId, uchar blockId);
    //TODO string for testing, should be uchar*
    void startSession(string fileName);

    void endSession(uchar sessionId);

    ~FileManager();
private:

    void acquireLock();

    void releaseLock();

    pthread_mutex_t* fileLock_;

    string sessionFiles_[20];

};

#endif	/* _FILEMANAGER_H */

